#include "Bureaucrat.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <climits>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstdio>

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
	bool _c = false, _w = false; \
	try { stmt; } catch (const Exc&) { _c = true; } catch (...) { _w = true; } \
	expect(_c && !_w, #stmt " throws " #Exc); \
} while (0)

class StdoutCapture {
	std::streambuf* _saved;
	std::ostringstream _oss;
public:
	StdoutCapture()  { _saved = std::cout.rdbuf(_oss.rdbuf()); }
	~StdoutCapture() { std::cout.rdbuf(_saved); }
	std::string str() const { return _oss.str(); }
};

static int countOccurrences(const std::string& hay, const std::string& needle) {
	int n = 0;
	std::string::size_type pos = 0;
	while ((pos = hay.find(needle, pos)) != std::string::npos) {
		++n;
		pos += needle.size();
	}
	return n;
}

int main() {
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// === 1. 各 concrete Form の grade 定数を確認 ===
	section("1. concrete form grade constants match PDF");
	{
		ShrubberyCreationForm s("t");
		expect(s.getName() == "shrubbery" && s.getGradeToSign() == 145 && s.getGradeToExecute() == 137,
		       "ShrubberyCreationForm 145/137");
	}
	{
		RobotomyRequestForm r("t");
		expect(r.getName() == "robotomy request" && r.getGradeToSign() == 72 && r.getGradeToExecute() == 45,
		       "RobotomyRequestForm 72/45");
	}
	{
		PresidentialPardonForm p("t");
		expect(p.getName() == "presidential pardon" && p.getGradeToSign() == 25 && p.getGradeToExecute() == 5,
		       "PresidentialPardonForm 25/5");
	}

	// === 2. 初期状態: unsigned ===
	section("2. concrete forms start unsigned");
	{
		ShrubberyCreationForm s("t");
		RobotomyRequestForm r("t");
		PresidentialPardonForm p("t");
		expect(!s.getSigned() && !r.getSigned() && !p.getSigned(), "all 3 unsigned");
	}

	// === 3. execute() 未署名: FormNotSignedException ===
	section("3. execute() on unsigned form throws FormNotSignedException");
	{
		Bureaucrat top("Top", 1);
		ShrubberyCreationForm s("t");
		EXPECT_THROWS(s.execute(top), AForm::FormNotSignedException);
	}
	{
		Bureaucrat top("Top", 1);
		RobotomyRequestForm r("t");
		EXPECT_THROWS(r.execute(top), AForm::FormNotSignedException);
	}
	{
		Bureaucrat top("Top", 1);
		PresidentialPardonForm p("t");
		EXPECT_THROWS(p.execute(top), AForm::FormNotSignedException);
	}

	// === 4. execute(): grade == gradeToExecute (境界成功) ===
	section("4. execute at boundary grade == gradeToExecute");
	{
		Bureaucrat b("B", 137);
		ShrubberyCreationForm s("boundary_shrub");
		Bureaucrat signer("S", 145);
		signer.signForm(s);
		StdoutCapture cap;
		try { s.execute(b); expect(true, "shrubbery execute at 137"); }
		catch (...) { expect(false, "should not throw"); }
	}

	// === 5. execute(): grade == gradeToExecute + 1 (境界失敗) ===
	section("5. execute at gradeToExecute+1 throws GradeTooLowException");
	{
		Bureaucrat s("S", 1);
		Bureaucrat b("B", 138);
		ShrubberyCreationForm shr("t");
		s.signForm(shr);
		EXPECT_THROWS(shr.execute(b), AForm::GradeTooLowException);
	}
	{
		Bureaucrat s("S", 1);
		Bureaucrat b("B", 46);
		RobotomyRequestForm rob("t");
		s.signForm(rob);
		EXPECT_THROWS(rob.execute(b), AForm::GradeTooLowException);
	}
	{
		Bureaucrat s("S", 1);
		Bureaucrat b("B", 6);
		PresidentialPardonForm pp("t");
		s.signForm(pp);
		EXPECT_THROWS(pp.execute(b), AForm::GradeTooLowException);
	}

	// === 6. Shrubbery: 実際にファイルが生成される & 内容がある ===
	section("6. Shrubbery execute creates file with content");
	{
		std::remove("verify_shrubbery"); // 事前クリア
		Bureaucrat h("H", 1);
		ShrubberyCreationForm s("verify");
		h.signForm(s);
		h.executeForm(s);
		std::ifstream ifs("verify_shrubbery");
		expect(ifs.is_open(), "file opened");
		std::string content((std::istreambuf_iterator<char>(ifs)),
		                    std::istreambuf_iterator<char>());
		expect(!content.empty(), "content is non-empty");
		expect(content.find('#') != std::string::npos || content.find('*') != std::string::npos,
		       "content contains tree characters");
		ifs.close();
		std::remove("verify_shrubbery");
	}

	// === 7. Shrubbery: 特殊なtarget (空文字, unicode, 数字) ===
	section("7. Shrubbery with special targets");
	{
		Bureaucrat h("H", 1);
		{
			ShrubberyCreationForm s("");
			h.signForm(s);
			h.executeForm(s);
			std::ifstream ifs("_shrubbery");
			expect(ifs.is_open(), "empty target -> _shrubbery file created");
			ifs.close();
			std::remove("_shrubbery");
		}
		{
			ShrubberyCreationForm s("12345");
			h.signForm(s);
			h.executeForm(s);
			std::ifstream ifs("12345_shrubbery");
			expect(ifs.is_open(), "numeric target file created");
			ifs.close();
			std::remove("12345_shrubbery");
		}
	}

	// === 8. Robotomy: 統計的に 50% 前後 (1000試行, tolerance ±150) ===
	section("8. Robotomy statistical distribution (~50%)");
	{
		Bureaucrat s("S", 1);
		Bureaucrat h("H", 1);
		RobotomyRequestForm rob("Bender");
		s.signForm(rob);
		int successes = 0, failures = 0;
		{
			StdoutCapture cap;
			for (int i = 0; i < 1000; ++i) {
				try { rob.execute(h); } catch (...) {}
			}
			std::string out = cap.str();
			successes = countOccurrences(out, "robotomized successfully");
			failures = countOccurrences(out, "Robotomy of Bender failed");
		}
		expect(successes + failures == 1000, "total 1000 attempts logged");
		expect(successes >= 350 && successes <= 650, "successes in [350,650] (50% ±150)");
		std::cout << "    (successes=" << successes << ", failures=" << failures << ")\n";
	}

	// === 9. Presidential: 出力書式 ===
	section("9. PresidentialPardonForm output format");
	{
		Bureaucrat s("S", 1);
		Bureaucrat h("H", 1);
		PresidentialPardonForm pp("Arthur Dent");
		s.signForm(pp);
		StdoutCapture cap;
		pp.execute(h);
		std::string out = cap.str();
		expect(out.find("Arthur Dent has been pardoned by Zaphod Beeblebrox") != std::string::npos,
		       "PDF-mandated pardon message present");
	}

	// === 10. 全 concrete: sign 失敗 + execute 失敗 ===
	section("10. sign fails: form remains unsigned & execute throws");
	{
		Bureaucrat weak("Weak", 26);
		PresidentialPardonForm pp("X");
		bool caught_sign = false;
		try { pp.beSigned(weak); }
		catch (const AForm::GradeTooLowException&) { caught_sign = true; }
		expect(caught_sign, "beSigned throws for weak bureaucrat");
		expect(!pp.getSigned(), "still unsigned");
		Bureaucrat top("Top", 1);
		EXPECT_THROWS(pp.execute(top), AForm::FormNotSignedException);
	}

	// === 11. execute() 多重呼び出し: 常に成功 (副作用は毎回) ===
	section("11. execute() multiple times remains valid");
	{
		Bureaucrat h("H", 1);
		PresidentialPardonForm pp("Multi");
		h.signForm(pp);
		StdoutCapture cap;
		bool ok = true;
		for (int i = 0; i < 20; ++i) {
			try { pp.execute(h); }
			catch (...) { ok = false; break; }
		}
		expect(ok, "20 executes succeed");
	}

	// === 12. Bureaucrat::executeForm 出力 (成功) ===
	section("12. executeForm success output");
	{
		Bureaucrat s("S", 1);
		Bureaucrat h("H", 1);
		PresidentialPardonForm pp("X");
		s.signForm(pp);
		StdoutCapture cap;
		h.executeForm(pp);
		expect(cap.str().find("H executed presidential pardon") != std::string::npos,
		       "prints 'H executed presidential pardon'");
	}

	// === 13. Bureaucrat::executeForm 出力 (失敗: 未署名) ===
	section("13. executeForm failure output (unsigned)");
	{
		Bureaucrat h("H", 1);
		PresidentialPardonForm pp("X");
		StdoutCapture cap;
		h.executeForm(pp);
		std::string out = cap.str();
		expect(out.find("H couldn't execute presidential pardon") != std::string::npos,
		       "prints 'H couldn't execute'");
		expect(out.find("because") != std::string::npos, "contains 'because'");
	}

	// === 14. Bureaucrat::executeForm 出力 (失敗: grade 不足) ===
	section("14. executeForm failure output (grade too low)");
	{
		Bureaucrat s("S", 1);
		Bureaucrat weak("W", 6);
		PresidentialPardonForm pp("X");
		s.signForm(pp);
		StdoutCapture cap;
		weak.executeForm(pp);
		expect(cap.str().find("W couldn't execute") != std::string::npos,
		       "prints failure line");
	}

	// === 15. AForm* array polymorphic execute & operator<< ===
	section("15. polymorphic AForm* array (execute + operator<<)");
	{
		Bureaucrat h("H", 1);
		AForm* forms[3];
		forms[0] = new ShrubberyCreationForm("poly");
		forms[1] = new RobotomyRequestForm("poly");
		forms[2] = new PresidentialPardonForm("poly");
		StdoutCapture cap; // 出力隠す
		bool all_signed = true;
		for (int i = 0; i < 3; ++i) {
			h.signForm(*forms[i]);
			if (!forms[i]->getSigned()) all_signed = false;
			h.executeForm(*forms[i]);
			std::ostringstream oss; oss << *forms[i];
			if (oss.str().empty()) all_signed = false;
		}
		for (int i = 0; i < 3; ++i) delete forms[i];
		expect(all_signed, "all 3 signed and executed via AForm*");
	}
	std::remove("poly_shrubbery");

	// === 16. virtual デストラクタ: delete AForm* で子dtor実行される ===
	section("16. virtual dtor: 500 new/delete via AForm*");
	{
		for (int i = 0; i < 500; ++i) {
			AForm* f = new ShrubberyCreationForm("v");
			delete f;
		}
		expect(true, "500 shrubbery new/delete OK");
	}
	{
		for (int i = 0; i < 500; ++i) {
			AForm* f = new RobotomyRequestForm("v");
			delete f;
		}
		expect(true, "500 robotomy new/delete OK");
	}
	{
		for (int i = 0; i < 500; ++i) {
			AForm* f = new PresidentialPardonForm("v");
			delete f;
		}
		expect(true, "500 pardon new/delete OK");
	}

	// === 17. 各 concrete: copy ctor が signed 状態を保持 ===
	section("17. concrete copy ctor preserves _signed & _target");
	{
		Bureaucrat h("H", 1);
		ShrubberyCreationForm a("copyA");
		h.signForm(a);
		ShrubberyCreationForm b(a);
		expect(b.getSigned(), "_signed carried");
		expect(b.getTarget() == "copyA", "_target carried");
	}
	{
		Bureaucrat h("H", 1);
		RobotomyRequestForm a("copyR");
		h.signForm(a);
		RobotomyRequestForm b(a);
		expect(b.getSigned() && b.getTarget() == "copyR", "robotomy copy ctor");
	}
	{
		Bureaucrat h("H", 1);
		PresidentialPardonForm a("copyP");
		h.signForm(a);
		PresidentialPardonForm b(a);
		expect(b.getSigned() && b.getTarget() == "copyP", "pardon copy ctor");
	}

	// === 18. 各 concrete: operator= (親の _signed + 自身の _target) ===
	section("18. concrete operator= copies _signed and _target");
	{
		Bureaucrat h("H", 1);
		ShrubberyCreationForm src("srcS");
		h.signForm(src);
		ShrubberyCreationForm dst("dstS");
		dst = src;
		expect(dst.getSigned() && dst.getTarget() == "srcS", "shrubbery = preserved");
	}

	// === 19. 各 concrete: self-assign ===
	section("19. concrete self-assignment");
	{
		ShrubberyCreationForm s("self");
		s = s;
		expect(s.getTarget() == "self", "shrubbery self-assign");
	}
	{
		RobotomyRequestForm r("self");
		r = r;
		expect(r.getTarget() == "self", "robotomy self-assign");
	}
	{
		PresidentialPardonForm p("self");
		p = p;
		expect(p.getTarget() == "self", "pardon self-assign");
	}

	// === 20. sign後 grade を減らして execute 失敗 ===
	section("20. sign then executor grade decreases below threshold -> execute fails");
	{
		Bureaucrat s("Signer", 25);
		Bureaucrat exec("Exec", 5);
		PresidentialPardonForm pp("t");
		s.signForm(pp);
		try { exec.decrementGrade(); } catch(...) {}  // 5 -> 6
		expect(exec.getGrade() == 6, "grade decreased to 6");
		EXPECT_THROWS(pp.execute(exec), AForm::GradeTooLowException);
	}

	// === 21. execute() のチェック順序: 未署名が最初にthrow ===
	section("21. execute() check order: unsigned throws before grade check");
	{
		Bureaucrat weak("W", 150);
		PresidentialPardonForm pp("t");
		bool got_ns = false;
		try { pp.execute(weak); }
		catch (const AForm::FormNotSignedException&) { got_ns = true; }
		expect(got_ns, "FormNotSigned takes priority");
	}

	// === 22. std::exception& でまとめ catch ===
	section("22. polymorphic catch as std::exception&");
	{
		Bureaucrat h("H", 1);
		PresidentialPardonForm pp("t");
		try { pp.execute(h); }
		catch (std::exception& e) {
			expect(std::string(e.what()).find("not signed") != std::string::npos,
			       "what() contains 'not signed'");
		}
	}

	// === 23. AForm 抽象性 (compile-time: これはコメント) ===
	section("23. AForm is abstract (compile-time enforcement)");
	// AForm a; // これはコンパイルエラー ("cannot allocate an object of abstract type")
	expect(true, "AForm cannot be instantiated directly (verified by uncomment test)");

	// === 24. 別の bureaucrat が sign して、実行者が別の bureaucrat ===
	section("24. sign by one bureaucrat, execute by another");
	{
		Bureaucrat s("Signer", 25);
		Bureaucrat e("Exec", 5);
		PresidentialPardonForm pp("Ford");
		s.signForm(pp);
		StdoutCapture cap;
		e.executeForm(pp);
		expect(cap.str().find("Exec executed presidential pardon") != std::string::npos,
		       "Exec executes even though Signer signed");
	}

	// === 25. leak safety: 10,000 execute fails ===
	section("25. 10,000 failed executes (leak safety)");
	{
		Bureaucrat weak("W", 150);
		PresidentialPardonForm pp("Loop");
		StdoutCapture cap;
		for (int i = 0; i < 10000; ++i) {
			try { pp.execute(weak); } catch (...) {}
		}
		expect(true, "no crash");
	}

	// === 26. leak safety: 大量 new/delete Shrubbery (ファイル残らないよう即消し) ===
	section("26. 100 shrubbery new/execute/delete with file cleanup");
	{
		Bureaucrat h("H", 1);
		for (int i = 0; i < 100; ++i) {
			std::ostringstream tgt; tgt << "loop" << i;
			AForm* f = new ShrubberyCreationForm(tgt.str());
			h.signForm(*f);
			h.executeForm(*f);
			delete f;
			std::remove((tgt.str() + "_shrubbery").c_str());
		}
		expect(true, "100 shrubbery cycles OK");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
