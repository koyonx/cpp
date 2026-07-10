#include "Bureaucrat.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include "Intern.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

static void banner(const std::string& title) {
	std::cout << "\n===== " << title << " =====" << std::endl;
}

int main() {
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// [1] PDF 例通り: robotomy request for Bender
	banner("[1] Intern makes robotomy request for Bender");
	{
		Intern someRandomIntern;
		AForm* rrf = someRandomIntern.makeForm("robotomy request", "Bender");
		if (rrf) {
			std::cout << *rrf << std::endl;
			delete rrf;
		}
	}

	// [2] shrubbery creation
	banner("[2] Intern makes shrubbery creation for garden");
	{
		Intern intern;
		AForm* f = intern.makeForm("shrubbery creation", "garden");
		if (f) {
			std::cout << *f << std::endl;
			delete f;
		}
	}

	// [3] presidential pardon
	banner("[3] Intern makes presidential pardon for Arthur");
	{
		Intern intern;
		AForm* f = intern.makeForm("presidential pardon", "Arthur");
		if (f) {
			std::cout << *f << std::endl;
			delete f;
		}
	}

	// [4] 不明なフォーム名 → NULL & エラー出力
	banner("[4] Intern makes unknown form (returns NULL)");
	{
		Intern intern;
		AForm* f = intern.makeForm("coffee break", "target");
		if (f == NULL)
			std::cout << "OK: got NULL for unknown form name" << std::endl;
		else {
			std::cout << "unexpected non-null" << std::endl;
			delete f;
		}
	}

	// [5] 空文字のフォーム名
	banner("[5] Intern with empty form name");
	{
		Intern intern;
		AForm* f = intern.makeForm("", "target");
		if (f == NULL)
			std::cout << "OK: got NULL for empty form name" << std::endl;
		else delete f;
	}

	// [6] タイポ (大小・スペース違い) はマッチしない
	banner("[6] typo does not match (case sensitive, space sensitive)");
	{
		Intern intern;
		const char* typos[] = {
			"Robotomy Request",
			"robotomyrequest",
			" robotomy request",
			"robotomy  request"
		};
		for (int i = 0; i < 4; ++i) {
			AForm* f = intern.makeForm(typos[i], "T");
			std::cout << "  \"" << typos[i] << "\" -> " << (f ? "matched?!" : "NULL (expected)") << std::endl;
			delete f;  // safe on NULL
		}
	}

	// [7] 完全なワークフロー: Intern -> Bureaucrat sign -> execute -> delete
	banner("[7] full workflow via Intern");
	{
		Intern intern;
		Bureaucrat high("High", 1);
		const char* forms[] = { "shrubbery creation", "robotomy request", "presidential pardon" };
		const char* targets[] = { "workflow_garden", "R2D2", "Zaphod" };
		for (int i = 0; i < 3; ++i) {
			AForm* f = intern.makeForm(forms[i], targets[i]);
			if (f) {
				high.signForm(*f);
				high.executeForm(*f);
				delete f;
			}
		}
	}

	// [8] リーク耐性: makeForm を 1000 回
	banner("[8] 1000 makeForm calls (leak safety)");
	{
		Intern intern;
		int created = 0, unknown = 0;
		const char* names[] = {
			"shrubbery creation",
			"robotomy request",
			"presidential pardon",
			"bogus"
		};
		for (int i = 0; i < 1000; ++i) {
			AForm* f = intern.makeForm(names[i % 4], "loop");
			if (f) { ++created; delete f; }
			else ++unknown;
		}
		std::cout << "created=" << created << " unknown=" << unknown << std::endl;
	}

	// [9] Intern のコピー/代入 (OCF)
	banner("[9] Intern copy/assign (stateless but OCF)");
	{
		Intern a;
		Intern b(a);       // copy ctor
		Intern c;
		c = a;             // operator=
		c = c;             // self-assign
		AForm* f = b.makeForm("presidential pardon", "Ford");
		if (f) { std::cout << *f << std::endl; delete f; }
	}

	return 0;
}
