#include "Fixed.hpp"
#include <iostream>

void testDefaultConstructor() {
	std::cout << "\n=== TEST: Default Constructor ===" << std::endl;
	Fixed a;
	std::cout << "Value: " << a.getRawBits() << " (expected: 0)" << std::endl;
}

void testCopyConstructor() {
	std::cout << "\n=== TEST: Copy Constructor ===" << std::endl;
	Fixed a;
	a.setRawBits(42);
	Fixed b(a);
	std::cout << "Original: " << a.getRawBits() << std::endl;
	std::cout << "Copy: " << b.getRawBits() << " (expected: 42)" << std::endl;
}

void testCopyAssignment() {
	std::cout << "\n=== TEST: Copy Assignment Operator ===" << std::endl;
	Fixed a;
	Fixed b;
	a.setRawBits(100);
	b = a;
	std::cout << "After assignment, b = " << b.getRawBits() << " (expected: 100)" << std::endl;
}

void testSelfAssignment() {
	std::cout << "\n=== TEST: Self Assignment ===" << std::endl;
	Fixed a;
	a.setRawBits(50);
	Fixed& ref = a;
	a = ref;
	std::cout << "After self-assignment: " << a.getRawBits() << " (expected: 50)" << std::endl;
}

void testChainedAssignment() {
	std::cout << "\n=== TEST: Chained Assignment ===" << std::endl;
	Fixed a, b, c;
	a.setRawBits(75);
	c = b = a;
	std::cout << "a = " << a.getRawBits() << std::endl;
	std::cout << "b = " << b.getRawBits() << " (expected: 75)" << std::endl;
	std::cout << "c = " << c.getRawBits() << " (expected: 75)" << std::endl;
}

void testSetGetRawBits() {
	std::cout << "\n=== TEST: Set/Get RawBits ===" << std::endl;
	Fixed a;
	a.setRawBits(-1);
	std::cout << "Negative value: " << a.getRawBits() << " (expected: -1)" << std::endl;
	a.setRawBits(2147483647);
	std::cout << "Max int: " << a.getRawBits() << " (expected: 2147483647)" << std::endl;
	a.setRawBits(-2147483648);
	std::cout << "Min int: " << a.getRawBits() << " (expected: -2147483648)" << std::endl;
}

int main(void) {
	std::cout << "======================================" << std::endl;
	std::cout << "  EX00 COMPREHENSIVE TEST SUITE" << std::endl;
	std::cout << "======================================" << std::endl;

	testDefaultConstructor();
	testCopyConstructor();
	testCopyAssignment();
	testSelfAssignment();
	testChainedAssignment();
	testSetGetRawBits();

	std::cout << "\n======================================" << std::endl;
	std::cout << "  ALL TESTS COMPLETED" << std::endl;
	std::cout << "======================================" << std::endl;

	return 0;
}
