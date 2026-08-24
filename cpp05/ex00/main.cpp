#include "Bureaucrat.hpp"
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

#define EXPECT_NO_THROW(stmt) do { \
	bool _threw = false; \
	try { stmt; } catch (...) { _threw = true; } \
	expect(!_threw, #stmt " does not throw"); \
} while (0)

int main() {
	// === 1. Construction: 全ての有効grade [1..150] を受け入れる ===
	section("1. valid grade [1..150] all accepted");
	{
		bool ok = true;
		for (int g = 1; g <= 150; ++g) {
			try {
				Bureaucrat b("V", g);
				if (b.getGrade() != g) { ok = false; break; }
			} catch (...) { ok = false; break; }
		}
		expect(ok, "150 valid grades all constructed with correct value");
	}

	// === 2. Construction: too high (grade <= 0) ===
	section("2. grade <= 0 throws GradeTooHighException");
	for (int g = -5; g <= 0; ++g) {
		bool caught = false;
		try { Bureaucrat b("X", g); }
		catch (const Bureaucrat::GradeTooHighException&) { caught = true; }
		catch (...) {}
		std::ostringstream m; m << "grade " << g;
		expect(caught, m.str());
	}

	// === 3. Construction: too low (grade > 150) ===
	section("3. grade > 150 throws GradeTooLowException");
	for (int g = 151; g <= 155; ++g) {
		bool caught = false;
		try { Bureaucrat b("X", g); }
		catch (const Bureaucrat::GradeTooLowException&) { caught = true; }
		catch (...) {}
		std::ostringstream m; m << "grade " << g;
		expect(caught, m.str());
	}

	// === 4. INT_MIN / INT_MAX ===
	section("4. INT_MIN / INT_MAX bounds");
	EXPECT_THROWS(Bureaucrat b("X", INT_MIN); (void)b, Bureaucrat::GradeTooHighException);
	EXPECT_THROWS(Bureaucrat b("X", INT_MAX); (void)b, Bureaucrat::GradeTooLowException);

	// === 5. const Bureaucrat: getters ===
	section("5. getters work on const Bureaucrat");
	{
		const Bureaucrat b("Alice", 42);
		expect(b.getName() == "Alice", "getName const-correct");
		expect(b.getGrade() == 42, "getGrade const-correct");
	}

	// === 6. name edge cases ===
	section("6. name edge cases");
	{
		Bureaucrat empty("", 1);
		expect(empty.getName().empty(), "empty name accepted");
	}
	{
		std::string longName(1000, 'X');
		Bureaucrat lng(longName, 150);
		expect(lng.getName().size() == 1000, "1000-char name accepted");
	}
	{
		Bureaucrat sp("has spaces and 記号!", 75);
		expect(sp.getName() == "has spaces and 記号!", "unicode/spaces accepted");
	}

	// === 7. increment (境界外を除く全ての正常動作) ===
	section("7. incrementGrade normal");
	{
		Bureaucrat b("Inc", 100);
		b.incrementGrade();
		expect(b.getGrade() == 99, "100 -> 99");
		for (int i = 0; i < 10; ++i) b.incrementGrade();
		expect(b.getGrade() == 89, "10 more -> 89");
	}

	// === 8. increment at grade 1 throws ===
	section("8. increment at grade 1 throws, state preserved");
	{
		Bureaucrat b("Top", 1);
		EXPECT_THROWS(b.incrementGrade(), Bureaucrat::GradeTooHighException);
		expect(b.getGrade() == 1, "grade unchanged after failed increment");
	}

	// === 9. decrement normal ===
	section("9. decrementGrade normal");
	{
		Bureaucrat b("Dec", 50);
		b.decrementGrade();
		expect(b.getGrade() == 51, "50 -> 51");
	}

	// === 10. decrement at grade 150 throws ===
	section("10. decrement at 150 throws, state preserved");
	{
		Bureaucrat b("Bot", 150);
		EXPECT_THROWS(b.decrementGrade(), Bureaucrat::GradeTooLowException);
		expect(b.getGrade() == 150, "grade unchanged after failed decrement");
	}

	// === 11. Walk full range: 150 → 1 → 150 ===
	section("11. walk 150 -> 1 -> 150 (149 increments then 149 decrements)");
	{
		Bureaucrat b("Walk", 150);
		bool ok = true;
		for (int i = 0; i < 149; ++i) {
			try { b.incrementGrade(); }
			catch (...) { ok = false; break; }
		}
		expect(ok && b.getGrade() == 1, "walked to 1");
		ok = true;
		for (int i = 0; i < 149; ++i) {
			try { b.decrementGrade(); }
			catch (...) { ok = false; break; }
		}
		expect(ok && b.getGrade() == 150, "walked back to 150");
	}

	// === 12. copy constructor is deep ===
	section("12. copy constructor produces independent object");
	{
		Bureaucrat orig("Orig", 50);
		Bureaucrat copy(orig);
		expect(copy.getName() == "Orig", "name copied");
		expect(copy.getGrade() == 50, "grade copied");
		copy.incrementGrade();
		expect(orig.getGrade() == 50, "orig unchanged after copy.increment");
		expect(copy.getGrade() == 49, "copy changed");
	}

	// === 13. operator= (const name → 不変, grade のみ) ===
	section("13. operator= copies grade only (name is const)");
	{
		Bureaucrat a("A", 10);
		Bureaucrat b("B", 100);
		a = b;
		expect(a.getName() == "A", "a name (const) unchanged");
		expect(a.getGrade() == 100, "a grade copied from b");
		expect(b.getName() == "B", "b unchanged");
		expect(b.getGrade() == 100, "b grade unchanged");
	}

	// === 14. self-assignment ===
	section("14. self-assignment safe");
	{
		Bureaucrat s("Self", 42);
		Bureaucrat* self = &s;
		s = *self;
		expect(s.getName() == "Self" && s.getGrade() == 42, "state preserved");
	}

	// === 15. chained assignment ===
	section("15. chained assignment a = b = c");
	{
		Bureaucrat a("A", 10), b("B", 20), c("C", 30);
		a = b = c;
		expect(a.getGrade() == 30 && b.getGrade() == 30 && c.getGrade() == 30,
		       "all take c's grade");
	}

	// === 16. operator= returns *this ===
	section("16. operator= returns reference to lhs");
	{
		Bureaucrat a("A", 10), b("B", 20);
		Bureaucrat& r = (a = b);
		expect(&r == &a, "returns lhs reference");
	}

	// === 17. 例外階層: 具体型で catch ===
	section("17. specific exception types catchable");
	{
		bool got_high = false;
		try { Bureaucrat b("X", 0); }
		catch (Bureaucrat::GradeTooHighException&) { got_high = true; }
		expect(got_high, "GradeTooHighException specific catch");

		bool got_low = false;
		try { Bureaucrat b("X", 200); }
		catch (Bureaucrat::GradeTooLowException&) { got_low = true; }
		expect(got_low, "GradeTooLowException specific catch");
	}

	// === 18. std::exception 経由 (polymorphic what()) ===
	section("18. polymorphic catch via std::exception&");
	{
		try { Bureaucrat b("X", -1); }
		catch (std::exception& e) {
			std::string msg = e.what();
			expect(msg.find("too high") != std::string::npos, "what() contains 'too high'");
		}
		try { Bureaucrat b("X", 999); }
		catch (std::exception& e) {
			std::string msg = e.what();
			expect(msg.find("too low") != std::string::npos, "what() contains 'too low'");
		}
	}

	// === 19. what() のメッセージ安定性 ===
	section("19. what() message stable across calls");
	{
		try { Bureaucrat b("X", 0); }
		catch (std::exception& e) {
			std::string a = e.what();
			std::string b = e.what();
			expect(a == b, "same message on repeated calls");
		}
	}

	// === 20. increment/decrement も同じ例外型 ===
	section("20. increment/decrement throw same exception types");
	{
		Bureaucrat b("X", 1);
		bool ok = false;
		try { b.incrementGrade(); }
		catch (Bureaucrat::GradeTooHighException&) { ok = true; }
		expect(ok, "increment throws GradeTooHighException");
	}
	{
		Bureaucrat b("X", 150);
		bool ok = false;
		try { b.decrementGrade(); }
		catch (Bureaucrat::GradeTooLowException&) { ok = true; }
		expect(ok, "decrement throws GradeTooLowException");
	}

	// === 21. operator<< 書式厳密一致 ===
	section("21. operator<< exact format");
	{
		Bureaucrat b("FormatTest", 42);
		std::ostringstream oss;
		oss << b;
		expect(oss.str() == "FormatTest, bureaucrat grade 42.", "exact format");
	}
	{
		Bureaucrat b("", 150);
		std::ostringstream oss;
		oss << b;
		expect(oss.str() == ", bureaucrat grade 150.", "empty name format");
	}
	{
		Bureaucrat b("Boundary", 1);
		std::ostringstream oss;
		oss << b;
		expect(oss.str() == "Boundary, bureaucrat grade 1.", "grade 1 format");
	}

	// === 22. operator<< chainable ===
	section("22. operator<< chainable");
	{
		Bureaucrat a("A", 1), b("B", 150);
		std::ostringstream oss;
		oss << a << " | " << b;
		expect(oss.str() == "A, bureaucrat grade 1. | B, bureaucrat grade 150.",
		       "chained output");
	}
	{
		Bureaucrat b("X", 42);
		std::ostringstream oss;
		std::ostream& r = (oss << b);
		expect(&r == &oss, "operator<< returns ostream&");
	}

	// === 23. default constructor (OCF) ===
	section("23. default constructor");
	{
		Bureaucrat d;
		expect(d.getGrade() >= 1 && d.getGrade() <= 150, "default grade valid");
	}

	// === 24. 10,000 回失敗コンストラクタ (leak耐性) ===
	section("24. 10,000 failed constructions - leak safety");
	{
		for (int i = 0; i < 10000; ++i) {
			try { Bureaucrat b("Loop", (i % 2 ? 200 : -1)); } catch (...) {}
		}
		expect(true, "no crash after 10000 fail-loops");
	}

	// === 25. increment/decrement stress ===
	section("25. increment/decrement 1000 pairs (stress)");
	{
		Bureaucrat b("Stress", 75);
		for (int i = 0; i < 1000; ++i) {
			try { b.incrementGrade(); } catch (...) {}
			try { b.decrementGrade(); } catch (...) {}
		}
		expect(b.getGrade() == 75, "returned to 75");
	}

	// === 26. コピー→変更→独立検証 ===
	section("26. copy independence chain (A -> B -> C)");
	{
		Bureaucrat A("A", 100);
		Bureaucrat B(A);
		Bureaucrat C(B);
		A = B; // no-op grade wise but exercises operator=
		C.incrementGrade();
		expect(A.getGrade() == 100 && B.getGrade() == 100 && C.getGrade() == 99,
		       "A,B unchanged, C changed");
	}

	// === SUMMARY ===
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
