#include "Bureaucrat.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

static void banner(const std::string& title) {
	std::cout << "\n===== " << title << " =====" << std::endl;
}

int main() {
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// [1] Shrubbery: 未署名で execute しようとする -> FormNotSigned
	banner("[1] Shrubbery execute on unsigned form throws");
	{
		Bureaucrat high("Boss", 1);
		ShrubberyCreationForm shr("garden");
		high.executeForm(shr);
	}

	// [2] Shrubbery: 署名成功 & execute 成功 -> ファイル生成
	banner("[2] Shrubbery signed & executed -> creates garden_shrubbery");
	{
		Bureaucrat mid("Mid", 100);
		Bureaucrat high("High", 1);
		ShrubberyCreationForm shr("garden");
		mid.signForm(shr);      // 100 <= 145 -> OK
		high.executeForm(shr);  // 1 <= 137 -> OK
	}

	// [3] Shrubbery: 署名は出来るが execute grade 不足
	banner("[3] Shrubbery signed but executor grade too low");
	{
		Bureaucrat lo("Lo", 145);
		Bureaucrat med("Med", 140);
		ShrubberyCreationForm shr("home");
		lo.signForm(shr);
		med.executeForm(shr);  // 140 > 137 -> fail
	}

	// [4] Shrubbery: sign 自体が失敗
	banner("[4] Shrubbery sign fails (grade > gradeToSign)");
	{
		Bureaucrat bot("Bot", 149);
		ShrubberyCreationForm shr("home2");
		bot.signForm(shr);         // 149 > 145 -> fail
		bot.executeForm(shr);      // 未署名なので FormNotSigned
	}

	// [5] Robotomy: 署名 & execute (何度も呼んで 50% を体感)
	banner("[5] Robotomy signed & executed x8 (roughly 50% success)");
	{
		Bureaucrat mid("Mid", 60);
		Bureaucrat high("High", 40);
		RobotomyRequestForm rob("Bender");
		mid.signForm(rob);  // 60 <= 72 OK
		for (int i = 0; i < 8; ++i)
			high.executeForm(rob);
	}

	// [6] Robotomy: execute grade 不足
	banner("[6] Robotomy execute grade too low");
	{
		Bureaucrat mid("Mid", 60);
		Bureaucrat weak("Weak", 46);
		RobotomyRequestForm rob("Fry");
		mid.signForm(rob);
		weak.executeForm(rob); // 46 > 45 -> fail
	}

	// [7] Presidential: 完全成功
	banner("[7] PresidentialPardon signed & executed");
	{
		Bureaucrat mid("Mid", 20);
		Bureaucrat high("High", 3);
		PresidentialPardonForm pp("Arthur Dent");
		mid.signForm(pp);  // 20 <= 25 OK
		high.executeForm(pp); // 3 <= 5 OK
	}

	// [8] Presidential: sign 不足
	banner("[8] PresidentialPardon sign fails");
	{
		Bureaucrat weak("Weak", 26);
		PresidentialPardonForm pp("Ford Prefect");
		weak.signForm(pp);  // 26 > 25 -> fail
	}

	// [9] Presidential: sign OK, execute 不足
	banner("[9] PresidentialPardon execute grade too low");
	{
		Bureaucrat mid("Mid", 20);
		Bureaucrat medExec("MedExec", 6);
		PresidentialPardonForm pp("Trillian");
		mid.signForm(pp);
		medExec.executeForm(pp); // 6 > 5 -> fail
	}

	// [10] AForm 抽象性: AForm* 経由での polymorphic execute
	banner("[10] polymorphic execute via AForm*");
	{
		Bureaucrat high("High", 1);
		AForm* forms[3];
		forms[0] = new ShrubberyCreationForm("polymorphic");
		forms[1] = new RobotomyRequestForm("polymorphic");
		forms[2] = new PresidentialPardonForm("polymorphic");
		for (int i = 0; i < 3; ++i) {
			high.signForm(*forms[i]);
			high.executeForm(*forms[i]);
		}
		for (int i = 0; i < 3; ++i)
			delete forms[i];   // 仮想デストラクタで正しく破棄されるか
	}

	// [11] コピーコンストラクタ / 代入 (各 concrete)
	banner("[11] copy/assign of concrete forms preserves polymorphic behavior");
	{
		Bureaucrat high("High", 1);
		ShrubberyCreationForm a("copyA");
		high.signForm(a);
		ShrubberyCreationForm b(a);      // copy ctor
		ShrubberyCreationForm c("copyC");
		c = a;                            // operator= (_signed のみ)
		high.executeForm(b);
		high.executeForm(c);
		std::cout << "b: " << b << std::endl;
		std::cout << "c: " << c << std::endl;
	}

	// [12] 自己代入
	banner("[12] self-assignment");
	{
		PresidentialPardonForm p("Self");
		p = p;
		std::cout << p << std::endl;
	}

	// [13] catch as std::exception&
	banner("[13] execute() throws FormNotSigned, catch as std::exception&");
	{
		Bureaucrat high("High", 1);
		PresidentialPardonForm p("NotSigned");
		try {
			p.execute(high);
		} catch (std::exception& e) {
			std::cout << "std::exception&: " << e.what() << std::endl;
		}
	}

	// [14] リーク耐性: 例外送出パスを1000回
	banner("[14] 1000 failed executes (leak safety)");
	{
		Bureaucrat weak("Weak", 150);
		PresidentialPardonForm p("Loop");
		for (int i = 0; i < 1000; ++i) {
			try {
				p.execute(weak);
			} catch (std::exception&) {}
		}
		std::cout << "1000 failed executes done." << std::endl;
	}

	// [15] リーク耐性: new/delete ループ
	banner("[15] 500 new/delete cycles via AForm* (virtual dtor)");
	{
		for (int i = 0; i < 500; ++i) {
			AForm* f = new RobotomyRequestForm("iter");
			delete f;
		}
		std::cout << "500 new/delete done." << std::endl;
	}

	return 0;
}
