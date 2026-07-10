#include "Bureaucrat.hpp"
#include "Form.hpp"
#include <iostream>

static void banner(const std::string& title) {
	std::cout << "\n===== " << title << " =====" << std::endl;
}

int main() {
	// [1] 正常な Form 構築
	banner("[1] Form valid construction");
	try {
		Form f("Contract", 50, 25);
		std::cout << f << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [2] Form 例外: gradeToSign が 0
	banner("[2] Form gradeToSign 0 (too high)");
	try {
		Form f("BadHi", 0, 10);
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [3] Form 例外: gradeToExecute が 151
	banner("[3] Form gradeToExecute 151 (too low)");
	try {
		Form f("BadLo", 10, 151);
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [4] Form 例外: 両方無効
	banner("[4] Form both invalid (high wins)");
	try {
		Form f("BadBoth", -1, 200);
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [5] 境界: gradeToSign=1, gradeToExecute=150
	banner("[5] Form boundary (1, 150)");
	try {
		Form f("Boundary", 1, 150);
		std::cout << f << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [6] beSigned 成功 (grade == gradeToSign)
	banner("[6] beSigned success (grade == gradeToSign)");
	try {
		Bureaucrat b("Signer", 50);
		Form f("Contract", 50, 25);
		f.beSigned(b);
		std::cout << f << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [7] beSigned 失敗 (grade > gradeToSign, i.e. lower rank)
	banner("[7] beSigned fail (grade too low)");
	try {
		Bureaucrat b("Low", 100);
		Form f("Contract", 50, 25);
		f.beSigned(b);
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [8] Bureaucrat::signForm 成功
	banner("[8] Bureaucrat::signForm success");
	try {
		Bureaucrat b("Alice", 10);
		Form f("TaxForm", 50, 25);
		b.signForm(f);
		std::cout << f << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [9] Bureaucrat::signForm 失敗 (grade too low)
	banner("[9] Bureaucrat::signForm fail (grade too low)");
	try {
		Bureaucrat b("Bob", 100);
		Form f("TaxForm", 50, 25);
		b.signForm(f);
		std::cout << f << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [10] 二重署名は冪等 (再度呼んでも例外にならず、signed=true のまま)
	banner("[10] double sign is idempotent");
	try {
		Bureaucrat b("Alice", 10);
		Form f("TaxForm", 50, 25);
		b.signForm(f);
		b.signForm(f); // 2回目
		std::cout << f << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [11] Form コピーコンストラクタ
	banner("[11] Form copy constructor");
	try {
		Bureaucrat b("Signer", 10);
		Form orig("Orig", 50, 25);
		b.signForm(orig);
		Form copy(orig);
		std::cout << "orig: " << orig << std::endl;
		std::cout << "copy: " << copy << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [12] Form 代入 (name/grade は const、_signed のみコピー)
	banner("[12] Form operator= copies _signed only");
	try {
		Bureaucrat b("Signer", 10);
		Form a("A", 10, 5);
		Form c("C", 100, 90);
		b.signForm(a);
		std::cout << "before: a=" << a << " | c=" << c << std::endl;
		c = a;
		std::cout << "after c=a: a=" << a << " | c=" << c << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [13] 自己代入
	banner("[13] Form self-assignment");
	try {
		Form f("Self", 42, 42);
		f = f;
		std::cout << f << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [14] std::exception& でまとめて catch できるか
	banner("[14] catchable as std::exception&");
	try {
		Form f("Poly", 999, 5);
	} catch (std::exception& e) {
		std::cout << "std::exception&: " << e.what() << std::endl;
	}

	// [15] リーク耐性: 大量の失敗パス
	banner("[15] 1000 failed constructions/signs (leak safety)");
	for (int i = 0; i < 1000; ++i) {
		try {
			Form f("Loop", (i % 2 == 0) ? 0 : 200, 50);
		} catch (std::exception&) {}
	}
	{
		Bureaucrat weak("Weak", 149);
		for (int i = 0; i < 1000; ++i) {
			try {
				Form f("Loop2", 1, 1);
				weak.signForm(f); // 失敗パス
			} catch (std::exception&) {}
		}
	}
	std::cout << "1000+1000 iterations done." << std::endl;

	return 0;
}
