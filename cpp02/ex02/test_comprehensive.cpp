#include "Fixed.hpp"
#include <iostream>

void testComparisonOperators() {
	std::cout << "\n=== TEST: Comparison Operators ===" << std::endl;
	Fixed a(42.42f);
	Fixed b(42.42f);
	Fixed c(10.5f);
	Fixed d(-5.25f);

	std::cout << "a(42.42) == b(42.42): " << (a == b) << " (expected: 1)" << std::endl;
	std::cout << "a(42.42) != c(10.5): " << (a != c) << " (expected: 1)" << std::endl;
	std::cout << "a(42.42) > c(10.5): " << (a > c) << " (expected: 1)" << std::endl;
	std::cout << "c(10.5) < a(42.42): " << (c < a) << " (expected: 1)" << std::endl;
	std::cout << "a(42.42) >= b(42.42): " << (a >= b) << " (expected: 1)" << std::endl;
	std::cout << "c(10.5) <= a(42.42): " << (c <= a) << " (expected: 1)" << std::endl;
	std::cout << "d(-5.25) < c(10.5): " << (d < c) << " (expected: 1)" << std::endl;
}

void testArithmeticOperators() {
	std::cout << "\n=== TEST: Arithmetic Operators ===" << std::endl;
	Fixed a(10.0f);
	Fixed b(5.0f);
	Fixed c(2.5f);

	std::cout << "10 + 5 = " << (a + b) << " (expected: 15)" << std::endl;
	std::cout << "10 - 5 = " << (a - b) << " (expected: 5)" << std::endl;
	std::cout << "10 * 5 = " << (a * b) << " (expected: 50)" << std::endl;
	std::cout << "10 / 5 = " << (a / b) << " (expected: 2)" << std::endl;
	std::cout << "10 / 2.5 = " << (a / c) << " (expected: 4)" << std::endl;
	std::cout << "2.5 * 2.5 = " << (c * c) << " (expected: 6.25)" << std::endl;
}

void testNegativeArithmetic() {
	std::cout << "\n=== TEST: Negative Number Arithmetic ===" << std::endl;
	Fixed a(-10.0f);
	Fixed b(5.0f);
	Fixed c(-5.0f);

	std::cout << "-10 + 5 = " << (a + b) << " (expected: -5)" << std::endl;
	std::cout << "-10 - 5 = " << (a - b) << " (expected: -15)" << std::endl;
	std::cout << "-10 * 5 = " << (a * b) << " (expected: -50)" << std::endl;
	std::cout << "-10 / 5 = " << (a / b) << " (expected: -2)" << std::endl;
	std::cout << "-10 * -5 = " << (a * c) << " (expected: 50)" << std::endl;
	std::cout << "-10 / -5 = " << (a / c) << " (expected: 2)" << std::endl;
}

void testIncrementDecrement() {
	std::cout << "\n=== TEST: Increment/Decrement Operators ===" << std::endl;
	Fixed a(5.0f);
	Fixed b;

	std::cout << "Initial value: " << a << std::endl;
	b = ++a;
	std::cout << "After ++a, a = " << a << ", b = " << b << std::endl;
	b = a++;
	std::cout << "After a++, a = " << a << ", b = " << b << std::endl;
	b = --a;
	std::cout << "After --a, a = " << a << ", b = " << b << std::endl;
	b = a--;
	std::cout << "After a--, a = " << a << ", b = " << b << std::endl;
}

void testSmallestIncrement() {
	std::cout << "\n=== TEST: Smallest Representable Epsilon ===" << std::endl;
	Fixed a(1.0f);
	Fixed b = a;

	++b;
	std::cout << "a = " << a << std::endl;
	std::cout << "a + epsilon = " << b << std::endl;
	std::cout << "Epsilon = " << (b - a) << " (expected: 0.00390625 or 1/256)" << std::endl;
	std::cout << "a < (a + epsilon): " << (a < b) << " (expected: 1)" << std::endl;
}

void testMinMax() {
	std::cout << "\n=== TEST: Min/Max Functions ===" << std::endl;
	Fixed a(42.42f);
	Fixed b(10.5f);
	const Fixed ca(100.0f);
	const Fixed cb(50.0f);

	std::cout << "min(42.42, 10.5) = " << Fixed::min(a, b) << " (expected: 10.5)" << std::endl;
	std::cout << "max(42.42, 10.5) = " << Fixed::max(a, b) << " (expected: 42.42)" << std::endl;
	std::cout << "min(const 100, const 50) = " << Fixed::min(ca, cb) << " (expected: 50)" << std::endl;
	std::cout << "max(const 100, const 50) = " << Fixed::max(ca, cb) << " (expected: 100)" << std::endl;
}

void testMinMaxWithNegatives() {
	std::cout << "\n=== TEST: Min/Max with Negative Numbers ===" << std::endl;
	Fixed a(-10.0f);
	Fixed b(5.0f);
	Fixed c(-20.0f);

	std::cout << "min(-10, 5) = " << Fixed::min(a, b) << " (expected: -10)" << std::endl;
	std::cout << "max(-10, 5) = " << Fixed::max(a, b) << " (expected: 5)" << std::endl;
	std::cout << "min(-10, -20) = " << Fixed::min(a, c) << " (expected: -20)" << std::endl;
	std::cout << "max(-10, -20) = " << Fixed::max(a, c) << " (expected: -10)" << std::endl;
}

void testChainedOperations() {
	std::cout << "\n=== TEST: Chained Operations ===" << std::endl;
	Fixed a(10.0f);
	Fixed b(5.0f);
	Fixed c(2.0f);

	std::cout << "(10 + 5) * 2 = " << ((a + b) * c) << " (expected: 30)" << std::endl;
	std::cout << "10 / 5 + 2 = " << ((a / b) + c) << " (expected: 4)" << std::endl;
	std::cout << "(10 - 5) / 2 = " << ((a - b) / c) << " (expected: 2.5)" << std::endl;
}

void testEdgeCases() {
	std::cout << "\n=== TEST: Edge Cases ===" << std::endl;
	Fixed zero(0.0f);
	Fixed one(1.0f);
	Fixed neg_one(-1.0f);

	std::cout << "0 + 1 = " << (zero + one) << " (expected: 1)" << std::endl;
	std::cout << "0 * 1 = " << (zero * one) << " (expected: 0)" << std::endl;
	std::cout << "0 - 1 = " << (zero - one) << " (expected: -1)" << std::endl;
	std::cout << "1 * -1 = " << (one * neg_one) << " (expected: -1)" << std::endl;
	std::cout << "-1 * -1 = " << (neg_one * neg_one) << " (expected: 1)" << std::endl;
}

int main(void) {
	std::cout << "======================================" << std::endl;
	std::cout << "  EX02 COMPREHENSIVE TEST SUITE" << std::endl;
	std::cout << "======================================" << std::endl;

	testComparisonOperators();
	testArithmeticOperators();
	testNegativeArithmetic();
	testIncrementDecrement();
	testSmallestIncrement();
	testMinMax();
	testMinMaxWithNegatives();
	testChainedOperations();
	testEdgeCases();

	std::cout << "\n======================================" << std::endl;
	std::cout << "  ALL TESTS COMPLETED" << std::endl;
	std::cout << "======================================" << std::endl;

	return 0;
}
