#include "Bureaucrat.hpp"
#include "Form.hpp"
#include <iostream>
#include <sstream>
#include <climits>
#include <string>

static int g_pass = 0;
static int g_fail = 0;

static void section(const std::string& t) {
	std::cout << "\n=== " << t << " ===" << std::endl;
}

static void expect(bool cond, const std::string& what) {
	if (cond) { ++g_pass; std::cout << "  [PASS] " << what << std::endl; }
	else      { ++g_fail; std::cout << "  [FAIL] " << what << std::endl; }
}

#define EXPECT_THROWS(stmt, Exc) do { \
	bool _caught = false; bool _wrong = false; \
	try { stmt; } \
	catch (const Exc&) { _caught = true; } \
	catch (...) { _wrong = true; } \
	expect(_caught && !_wrong, #stmt " throws " #Exc); \
} while (0)

// stdout を捕捉して文字列に取り込む helper
class StdoutCapture {
	std::streambuf* _saved;
	std::ostringstream _oss;
public:
	StdoutCapture()  { _saved = std::cout.rdbuf(_oss.rdbuf()); }
	~StdoutCapture() { std::cout.rdbuf(_saved); }
	std::string str() const { return _oss.str(); }
};

int main() {
	// === 1. Form: 有効grade全組み合わせサンプル ===
	section("1. Form valid construction (sampled)");
	{
		int samples[][2] = { {1,1}, {1,150}, {150,150}, {50,25}, {145,137}, {72,45}, {25,5} };
		bool ok = true;
		for (int i = 0; i < 7; ++i) {
			try {
				Form f("Sample", samples[i][0], samples[i][1]);
				if (f.getGradeToSign() != samples[i][0] || f.getGradeToExecute() != samples[i][1])
					ok = false;
			} catch (...) { ok = false; break; }
		}
		expect(ok, "7 valid (sign, exec) tuples constructed correctly");
	}

	// === 2. Form: invalid gradeToSign ===
	section("2. Form invalid gradeToSign");
	EXPECT_THROWS(Form f("X", 0, 50), Form::GradeTooHighException);
	EXPECT_THROWS(Form f("X", -1, 50), Form::GradeTooHighException);
	EXPECT_THROWS(Form f("X", 151, 50), Form::GradeTooLowException);
	EXPECT_THROWS(Form f("X", 999, 50), Form::GradeTooLowException);
	EXPECT_THROWS(Form f("X", INT_MIN, 50), Form::GradeTooHighException);
	EXPECT_THROWS(Form f("X", INT_MAX, 50), Form::GradeTooLowException);

	// === 3. Form: invalid gradeToExecute ===
	section("3. Form invalid gradeToExecute");
	EXPECT_THROWS(Form f("X", 50, 0), Form::GradeTooHighException);
	EXPECT_THROWS(Form f("X", 50, 151), Form::GradeTooLowException);
	EXPECT_THROWS(Form f("X", 50, INT_MIN), Form::GradeTooHighException);
	EXPECT_THROWS(Form f("X", 50, INT_MAX), Form::GradeTooLowException);

	// === 4. Form: 両方 invalid ===
	section("4. Form both grades invalid");
	EXPECT_THROWS(Form f("X", 0, 0), Form::GradeTooHighException);
	EXPECT_THROWS(Form f("X", 200, 200), Form::GradeTooLowException);
	EXPECT_THROWS(Form f("X", 0, 200), Form::GradeTooHighException);
	EXPECT_THROWS(Form f("X", 200, 0), Form::GradeTooHighException); // 実装は high check 先

	// === 5. Form initial state ===
	section("5. Form: initial state");
	{
		Form f("Init", 50, 25);
		expect(f.getName() == "Init", "name preserved");
		expect(f.getSigned() == false, "not signed by default");
		expect(f.getGradeToSign() == 50, "gradeToSign preserved");
		expect(f.getGradeToExecute() == 25, "gradeToExecute preserved");
	}

	// === 6. const Form: getters ===
	section("6. const Form getters");
	{
		const Form f("Const", 50, 25);
		expect(f.getName() == "Const", "getName const");
		expect(f.getSigned() == false, "getSigned const");
		expect(f.getGradeToSign() == 50, "getGradeToSign const");
		expect(f.getGradeToExecute() == 25, "getGradeToExecute const");
	}

	// === 7. Form name edge cases ===
	section("7. Form name edge cases");
	{
		Form empty("", 1, 1);
		expect(empty.getName().empty(), "empty name accepted");
	}
	{
		std::string lng(1000, 'A');
		Form f(lng, 150, 150);
		expect(f.getName().size() == 1000, "1000-char name accepted");
	}

	// === 8. beSigned success at boundary (grade == gradeToSign) ===
	section("8. beSigned: grade == gradeToSign (boundary success)");
	{
		Bureaucrat b("Boundary", 50);
		Form f("F", 50, 25);
		try {
			f.beSigned(b);
			expect(f.getSigned() == true, "signed after grade==gradeToSign");
		} catch (...) { expect(false, "should not throw"); }
	}

	// === 9. beSigned success (grade < gradeToSign, i.e. higher rank) ===
	section("9. beSigned: grade < gradeToSign (higher rank)");
	{
		Bureaucrat b("High", 1);
		Form f("F", 50, 25);
		try {
			f.beSigned(b);
			expect(f.getSigned(), "grade 1 can sign gradeToSign 50");
		} catch (...) { expect(false, "should not throw"); }
	}

	// === 10. beSigned fail (grade > gradeToSign) ===
	section("10. beSigned: grade > gradeToSign throws GradeTooLowException");
	{
		Bureaucrat b("Low", 100);
		Form f("F", 50, 25);
		bool caught = false;
		try { f.beSigned(b); }
		catch (const Form::GradeTooLowException&) { caught = true; }
		expect(caught, "throws GradeTooLowException");
		expect(f.getSigned() == false, "still unsigned after fail");
	}

	// === 11. beSigned boundary: grade == gradeToSign + 1 (just below required) ===
	section("11. beSigned: grade == gradeToSign + 1 fails");
	{
		Bureaucrat b("JustBelow", 51);
		Form f("F", 50, 25);
		EXPECT_THROWS(f.beSigned(b), Form::GradeTooLowException);
		expect(f.getSigned() == false, "still unsigned");
	}

	// === 12. beSigned idempotent: 既に signed でも再度成功でOK ===
	section("12. beSigned on already signed form remains signed");
	{
		Bureaucrat b("H", 1);
		Form f("F", 50, 25);
		f.beSigned(b);
		f.beSigned(b);
		expect(f.getSigned(), "still signed after 2nd sign");
	}

	// === 13. Bureaucrat::signForm success: 出力書式 ===
	section("13. signForm success output format");
	{
		Bureaucrat b("Alice", 10);
		Form f("Contract", 50, 25);
		StdoutCapture cap;
		b.signForm(f);
		expect(cap.str().find("Alice signed Contract") != std::string::npos,
		       "prints 'Alice signed Contract'");
	}

	// === 14. Bureaucrat::signForm failure: 出力書式 ===
	section("14. signForm failure output format");
	{
		Bureaucrat b("Bob", 100);
		Form f("Contract", 50, 25);
		StdoutCapture cap;
		b.signForm(f);
		std::string out = cap.str();
		expect(out.find("Bob couldn't sign Contract") != std::string::npos,
		       "prints 'Bob couldn't sign Contract'");
		expect(out.find("because") != std::string::npos, "contains 'because'");
	}

	// === 15. signForm on same Form by multiple bureaucrats ===
	section("15. multiple bureaucrats sign same form");
	{
		Bureaucrat a("A", 10);
		Bureaucrat b("B", 30);
		Bureaucrat c("C", 60); // 失敗
		Form f("Multi", 50, 25);
		a.signForm(f);
		expect(f.getSigned(), "signed after A");
		b.signForm(f);
		expect(f.getSigned(), "still signed after B");
		c.signForm(f);
		expect(f.getSigned(), "still signed even if C failed to sign");
	}

	// === 16. copy constructor: preserves signed status ===
	section("16. copy ctor preserves _signed & fields");
	{
		Bureaucrat h("H", 1);
		Form orig("Orig", 50, 25);
		h.signForm(orig);
		Form copy(orig);
		expect(copy.getName() == "Orig", "name copied");
		expect(copy.getSigned(), "_signed=true copied");
		expect(copy.getGradeToSign() == 50, "gradeToSign copied");
		expect(copy.getGradeToExecute() == 25, "gradeToExecute copied");
	}

	// === 17. copy is independent ===
	section("17. copy independence");
	{
		Bureaucrat h("H", 1);
		Form orig("Orig", 50, 25);
		Form copy(orig); // both unsigned
		h.signForm(orig);
		expect(orig.getSigned() == true, "orig signed");
		expect(copy.getSigned() == false, "copy still unsigned");
	}

	// === 18. operator= copies _signed only ===
	section("18. operator= copies _signed only");
	{
		Bureaucrat h("H", 1);
		Form src("Src", 10, 5);
		Form dst("Dst", 100, 90);
		h.signForm(src);
		dst = src;
		expect(dst.getName() == "Dst", "name (const) unchanged");
		expect(dst.getGradeToSign() == 100, "gradeToSign (const) unchanged");
		expect(dst.getGradeToExecute() == 90, "gradeToExecute (const) unchanged");
		expect(dst.getSigned() == true, "_signed copied");
	}

	// === 19. self-assignment ===
	section("19. Form self-assignment");
	{
		Form f("Self", 42, 42);
		f = f;
		expect(f.getName() == "Self" && f.getGradeToSign() == 42 && f.getGradeToExecute() == 42,
		       "state preserved");
	}

	// === 20. operator= returns *this ===
	section("20. Form operator= returns *this");
	{
		Form a("A", 10, 5);
		Form b("B", 100, 90);
		Form& r = (a = b);
		expect(&r == &a, "returns reference to lhs");
	}

	// === 21. operator<< format ===
	section("21. operator<< format");
	{
		Form f("Fmt", 42, 21);
		std::ostringstream oss;
		oss << f;
		std::string s = oss.str();
		expect(s.find("\"Fmt\"") != std::string::npos, "contains name in quotes");
		expect(s.find("signed=no") != std::string::npos, "shows unsigned");
		expect(s.find("gradeToSign=42") != std::string::npos, "shows gradeToSign");
		expect(s.find("gradeToExecute=21") != std::string::npos, "shows gradeToExecute");
	}
	{
		Bureaucrat h("H", 1);
		Form f("Fmt2", 42, 21);
		h.signForm(f);
		std::ostringstream oss;
		oss << f;
		expect(oss.str().find("signed=yes") != std::string::npos, "shows signed after signing");
	}

	// === 22. operator<< returns ostream& ===
	section("22. operator<< returns ostream&");
	{
		Form f("X", 1, 1);
		std::ostringstream oss;
		std::ostream& r = (oss << f);
		expect(&r == &oss, "returns lhs stream");
	}

	// === 23. exception hierarchy: specific catch ===
	section("23. Form exceptions catchable as specific types");
	{
		bool ok = false;
		try { Form f("X", 0, 5); }
		catch (Form::GradeTooHighException&) { ok = true; }
		expect(ok, "GradeTooHighException specific");
	}
	{
		bool ok = false;
		try { Form f("X", 200, 5); }
		catch (Form::GradeTooLowException&) { ok = true; }
		expect(ok, "GradeTooLowException specific");
	}
	{
		Bureaucrat b("X", 100);
		Form f("Y", 50, 25);
		bool ok = false;
		try { f.beSigned(b); }
		catch (Form::GradeTooLowException&) { ok = true; }
		expect(ok, "beSigned GradeTooLowException specific");
	}

	// === 24. exception hierarchy: std::exception ===
	section("24. Form exceptions catchable as std::exception&");
	{
		try { Form f("X", 0, 5); }
		catch (std::exception& e) {
			expect(std::string(e.what()).find("too high") != std::string::npos,
			       "what() contains 'too high'");
		}
	}

	// === 25. signForm doesn't propagate exception (Bureaucrat catches internally) ===
	section("25. signForm never propagates exception");
	{
		Bureaucrat b("Low", 149);
		Form f("F", 1, 1);
		bool propagated = false;
		try { b.signForm(f); }
		catch (...) { propagated = true; }
		expect(!propagated, "signForm catches internally");
	}

	// === 26. leak safety: 10,000 fail-signs ===
	section("26. 10,000 failing sign attempts (leak safety)");
	{
		Bureaucrat weak("W", 149);
		StdoutCapture cap; // 出力抑制
		for (int i = 0; i < 10000; ++i) {
			try {
				Form f("Loop", 1, 1);
				weak.signForm(f);
			} catch (...) {}
		}
		expect(true, "no crash after 10000 iterations");
	}

	// === 27. leak safety: 10,000 Form construction failures ===
	section("27. 10,000 Form construction failures (leak safety)");
	{
		for (int i = 0; i < 10000; ++i) {
			try { Form f("L", (i % 2 ? 200 : 0), 50); } catch (...) {}
		}
		expect(true, "no crash after 10000 iterations");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
