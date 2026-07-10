#include "Functions.hpp"
#include "A.hpp"
#include "B.hpp"
#include "C.hpp"
#include <iostream>
#include <cstdlib>

Base* generate(void) {
	switch (std::rand() % 3) {
		case 0:	return new A();
		case 1:	return new B();
		case 2:	return new C();
	}
	return NULL;
}

void identify(Base* p) {
	if (dynamic_cast<A*>(p)) { std::cout << "A" << std::endl; return; }
	if (dynamic_cast<B*>(p)) { std::cout << "B" << std::endl; return; }
	if (dynamic_cast<C*>(p)) { std::cout << "C" << std::endl; return; }
	std::cout << "Unknown" << std::endl;
}

void identify(Base& p) {
	// PDF: reference 版では pointer 使用禁止 -> dynamic_cast<T&> + catch を用いる
	// <typeinfo> は禁止のため std::bad_cast を明示せず catch(...) で捕捉
	try { A& a = dynamic_cast<A&>(p); (void)a; std::cout << "A" << std::endl; return; }
	catch (...) {}
	try { B& b = dynamic_cast<B&>(p); (void)b; std::cout << "B" << std::endl; return; }
	catch (...) {}
	try { C& c = dynamic_cast<C&>(p); (void)c; std::cout << "C" << std::endl; return; }
	catch (...) {}
	std::cout << "Unknown" << std::endl;
}
