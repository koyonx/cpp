#include "Bureaucrat.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include "Intern.hpp"
#include <iostream>
#include <sstream>
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

class StdoutCapture {
	std::streambuf* _saved;
	std::ostringstream _oss;
public:
	StdoutCapture()  { _saved = std::cout.rdbuf(_oss.rdbuf()); }
	~StdoutCapture() { std::cout.rdbuf(_saved); }
	std::string str() const { return _oss.str(); }
};

int main() {
	std::srand(static_cast<unsigned int>(std::time(NULL)));
	Intern intern;

	// === 1. shrubbery creation を作成 ===
	section("1. makeForm 'shrubbery creation' returns Shrubbery-typed form");
	{
		AForm* f = intern.makeForm("shrubbery creation", "garden");
		expect(f != NULL, "not NULL");
		if (f) {
			expect(f->getName() == "shrubbery", "name is 'shrubbery'");
			expect(f->getGradeToSign() == 145, "gradeToSign 145");
			expect(f->getGradeToExecute() == 137, "gradeToExecute 137");
			expect(!f->getSigned(), "starts unsigned");
			delete f;
		}
	}

	// === 2. robotomy request ===
	section("2. makeForm 'robotomy request' returns Robotomy-typed form");
	{
		AForm* f = intern.makeForm("robotomy request", "Bender");
		expect(f != NULL, "not NULL");
		if (f) {
			expect(f->getName() == "robotomy request", "name matches");
			expect(f->getGradeToSign() == 72 && f->getGradeToExecute() == 45, "72/45");
			delete f;
		}
	}

	// === 3. presidential pardon ===
	section("3. makeForm 'presidential pardon' returns Pardon-typed form");
	{
		AForm* f = intern.makeForm("presidential pardon", "Arthur");
		expect(f != NULL, "not NULL");
		if (f) {
			expect(f->getName() == "presidential pardon", "name matches");
			expect(f->getGradeToSign() == 25 && f->getGradeToExecute() == 5, "25/5");
			delete f;
		}
	}

	// === 4. Unknown form name -> NULL ===
	section("4. unknown form name returns NULL");
	{
		StdoutCapture cap; // stdout も stderr は捕捉されないのでエラーは表示される
		AForm* f = intern.makeForm("coffee break", "somebody");
		expect(f == NULL, "returns NULL");
		delete f; // delete NULL は安全
	}
	{
		AForm* f = intern.makeForm("random garbage", "x");
		expect(f == NULL, "another unknown form is NULL");
		delete f;
	}

	// === 5. 空文字のフォーム名 ===
	section("5. empty form name returns NULL");
	{
		AForm* f = intern.makeForm("", "x");
		expect(f == NULL, "empty name is NULL");
		delete f;
	}

	// === 6. タイポは matchしない (大小・スペース・境界) ===
	section("6. typo variants do not match");
	{
		const char* typos[] = {
			"Robotomy Request",       // 大文字
			"robotomyrequest",         // スペース欠落
			" robotomy request",       // 先頭スペース
			"robotomy request ",       // 末尾スペース
			"robotomy  request",       // 二重スペース
			"shrubbery",               // 部分一致
			"presidential"             // 部分一致
		};
		bool all_null = true;
		for (int i = 0; i < 7; ++i) {
			StdoutCapture cap;
			AForm* f = intern.makeForm(typos[i], "t");
			if (f != NULL) { all_null = false; delete f; }
		}
		expect(all_null, "all 7 typo variants -> NULL");
	}

	// === 7. Empty target ===
	section("7. empty target accepted");
	{
		AForm* f = intern.makeForm("presidential pardon", "");
		expect(f != NULL, "empty target creates form");
		delete f;
	}

	// === 8. 非常に長い target ===
	section("8. 10,000-char target accepted");
	{
		std::string longTarget(10000, 'X');
		AForm* f = intern.makeForm("robotomy request", longTarget);
		expect(f != NULL, "long target creates form");
		delete f;
	}

	// === 9. 同じ名前を複数回呼ぶ -> 独立オブジェクト ===
	section("9. same name multiple times returns independent instances");
	{
		AForm* a = intern.makeForm("presidential pardon", "A");
		AForm* b = intern.makeForm("presidential pardon", "B");
		expect(a != NULL && b != NULL, "both non-null");
		expect(a != b, "different pointers");
		delete a;
		delete b;
	}

	// === 10. 3種同時所有・順序制御 ===
	section("10. hold 3 forms concurrently");
	{
		AForm* s = intern.makeForm("shrubbery creation", "t");
		AForm* r = intern.makeForm("robotomy request", "t");
		AForm* p = intern.makeForm("presidential pardon", "t");
		expect(s && r && p, "3 forms concurrent alloc OK");
		delete p; delete r; delete s;
		expect(true, "reverse-order delete OK");
	}

	// === 11. 完全ワークフロー: makeForm -> signForm -> executeForm -> delete ===
	section("11. full workflow via Intern for each form type");
	{
		Bureaucrat high("High", 1);
		const char* names[] = {"shrubbery creation", "robotomy request", "presidential pardon"};
		const char* targets[] = {"wf_garden", "wf_R2D2", "wf_Zaphod"};
		bool ok = true;
		StdoutCapture cap;
		for (int i = 0; i < 3; ++i) {
			AForm* f = intern.makeForm(names[i], targets[i]);
			if (!f) { ok = false; break; }
			high.signForm(*f);
			if (!f->getSigned()) { ok = false; delete f; break; }
			high.executeForm(*f);
			delete f;
		}
		expect(ok, "all 3 workflows succeeded");
		std::remove("wf_garden_shrubbery");
	}

	// === 12. Intern OCF: copy ctor / operator= / self-assign ===
	section("12. Intern OCF (stateless but compiles)");
	{
		Intern a;
		Intern b(a);
		Intern c;
		c = a;
		Intern* pc = &c;
		c = *pc;
		AForm* f = b.makeForm("robotomy request", "OCF");
		expect(f != NULL, "b can still makeForm after copy");
		delete f;
	}

	// === 13. 複数の Intern インスタンスが独立 ===
	section("13. multiple Intern instances independent");
	{
		Intern i1, i2, i3;
		AForm* a = i1.makeForm("shrubbery creation", "i1");
		AForm* b = i2.makeForm("shrubbery creation", "i2");
		AForm* c = i3.makeForm("shrubbery creation", "i3");
		expect(a != NULL && b != NULL && c != NULL, "3 interns all make OK");
		expect(a != b && b != c && a != c, "distinct pointers");
		delete a; delete b; delete c;
	}

	// === 14. makeForm 出力書式 ===
	section("14. makeForm success prints 'Intern creates <name>'");
	{
		StdoutCapture cap;
		AForm* f = intern.makeForm("shrubbery creation", "print_test");
		delete f;
		expect(cap.str().find("Intern creates shrubbery creation") != std::string::npos,
		       "prints 'Intern creates shrubbery creation'");
	}

	// === 15. leak safety: 10,000 makeForm ===
	section("15. 10,000 makeForm iterations (leak safety)");
	{
		const char* rot[] = {
			"shrubbery creation", "robotomy request",
			"presidential pardon", "bogus", "another_bogus"
		};
		int created = 0, unknown = 0;
		StdoutCapture cap;
		for (int i = 0; i < 10000; ++i) {
			AForm* f = intern.makeForm(rot[i % 5], "loop");
			if (f) { ++created; delete f; }
			else ++unknown;
		}
		expect(created == 6000, "6000 successful creations");
		expect(unknown == 4000, "4000 NULL returns");
	}

	// === 16. delete NULL は安全 (C++規格) ===
	section("16. delete NULL is safe (C++ standard)");
	{
		AForm* f = NULL;
		delete f;  // no-op per C++ spec
		expect(true, "delete NULL didn't crash");
	}

	// === 17. Intern が返した形式は operator<< で正しく print される ===
	section("17. returned AForm* prints via operator<<");
	{
		AForm* f = intern.makeForm("presidential pardon", "printtest");
		std::ostringstream oss;
		if (f) oss << *f;
		expect(oss.str().find("presidential pardon") != std::string::npos, "name shown");
		expect(oss.str().find("gradeToSign=25") != std::string::npos, "gradeToSign shown");
		delete f;
	}

	// === 18. const Intern からも makeForm 呼べる ===
	section("18. const Intern.makeForm callable");
	{
		const Intern ci;
		AForm* f = ci.makeForm("robotomy request", "const");
		expect(f != NULL, "const Intern makes form");
		delete f;
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
