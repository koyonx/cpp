#include "Bureaucrat.hpp"
#include <iostream>

static void banner(const std::string& title) {
	std::cout << "\n===== " << title << " =====" << std::endl;
}

int main() {
	// [1] 正常構築
	banner("[1] valid construction");
	try {
		Bureaucrat b("Alice", 42);
		std::cout << b << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [2] grade too high: 0
	banner("[2] grade 0 (too high)");
	try {
		Bureaucrat b("Zero", 0);
		std::cout << b << std::endl;
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [3] grade too high: 負数
	banner("[3] grade -5 (too high)");
	try {
		Bureaucrat b("Neg", -5);
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [4] grade too low: 151
	banner("[4] grade 151 (too low)");
	try {
		Bureaucrat b("OneOverLimit", 151);
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [5] grade too low: 巨大
	banner("[5] grade 999 (too low)");
	try {
		Bureaucrat b("Huge", 999);
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [6] 境界: grade 1 で increment すると例外
	banner("[6] grade 1 boundary: increment throws");
	try {
		Bureaucrat b("Top", 1);
		std::cout << b << std::endl;
		b.incrementGrade();
		std::cout << "no throw?? " << b << std::endl;
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [7] 境界: grade 150 で decrement すると例外
	banner("[7] grade 150 boundary: decrement throws");
	try {
		Bureaucrat b("Bottom", 150);
		std::cout << b << std::endl;
		b.decrementGrade();
		std::cout << "no throw?? " << b << std::endl;
	} catch (std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	// [8] 通常の increment/decrement
	banner("[8] normal increment/decrement (3 -> 2, 100 -> 101)");
	try {
		Bureaucrat b("Mid", 100);
		std::cout << b << std::endl;
		b.incrementGrade();
		std::cout << "after increment: " << b << std::endl; // 99
		b.decrementGrade();
		std::cout << "after decrement: " << b << std::endl; // 100
		b.decrementGrade();
		std::cout << "after decrement: " << b << std::endl; // 101
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [9] コピーコンストラクタ (深いコピー確認)
	banner("[9] copy constructor is independent");
	try {
		Bureaucrat orig("Orig", 50);
		Bureaucrat copy(orig);
		std::cout << "orig: " << orig << std::endl;
		std::cout << "copy: " << copy << std::endl;
		copy.incrementGrade();
		std::cout << "after copy.increment -> orig: " << orig << std::endl;
		std::cout << "after copy.increment -> copy: " << copy << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [10] コピー代入 (name は const なので不変, grade のみ複製)
	banner("[10] operator= copies grade only (name is const)");
	try {
		Bureaucrat a("A", 10);
		Bureaucrat b("B", 100);
		std::cout << "before: a=" << a << " | b=" << b << std::endl;
		a = b;
		std::cout << "after a=b: a=" << a << " | b=" << b << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [11] 自己代入
	banner("[11] self-assignment safety");
	try {
		Bureaucrat s("Self", 42);
		s = s;
		std::cout << s << std::endl;
	} catch (std::exception& e) {
		std::cerr << "Unexpected: " << e.what() << std::endl;
	}

	// [12] std::exception& でまとめて catch できるか
	banner("[12] catchable as std::exception&");
	try {
		Bureaucrat b("Poly", 200);
	} catch (std::exception& e) {
		std::cout << "std::exception&: " << e.what() << std::endl;
	}

	// [13] デフォルトコンストラクタ (OCF)
	banner("[13] default constructor");
	Bureaucrat def;
	std::cout << def << std::endl;

	// [14] 例外送出時、コンストラクタでリークしない事の確認 (例外投げてもnew未使用でOK)
	banner("[14] many failed constructions in loop (leak safety)");
	for (int i = 0; i < 1000; ++i) {
		try {
			Bureaucrat b("Loop", (i % 2 == 0) ? 0 : 200);
		} catch (std::exception&) {
			// silently ignore
		}
	}
	std::cout << "1000 failed constructions done." << std::endl;

	return 0;
}
