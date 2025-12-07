#include "Fixed.hpp"
#include <iostream>
#include <cmath>

void testIntConstructor() {
	std::cout << "\n=== TEST: Int Constructor ===" << std::endl;
	Fixed a(0);
	Fixed b(42);
	Fixed c(-42);
	Fixed d(2147483647);
	std::cout << "a(0) = " << a << " (expected: 0)" << std::endl;
	std::cout << "b(42) = " << b << " (expected: 42)" << std::endl;
	std::cout << "c(-42) = " << c << " (expected: -42)" << std::endl;
	std::cout << "d(2147483647) = " << d << " (expected: 2147483647)" << std::endl;
}

void testFloatConstructor() {
	std::cout << "\n=== TEST: Float Constructor ===" << std::endl;
	Fixed a(0.0f);
	Fixed b(42.42f);
	Fixed c(-42.42f);
	Fixed d(0.00390625f);  // 1/256 - smallest representable value
	Fixed e(0.5f);
	std::cout << "a(0.0f) = " << a << " (expected: 0)" << std::endl;
	std::cout << "b(42.42f) = " << b << " (expected: ~42.4219)" << std::endl;
	std::cout << "c(-42.42f) = " << c << " (expected: ~-42.4219)" << std::endl;
	std::cout << "d(0.00390625f) = " << d << " (expected: 0.00390625)" << std::endl;
	std::cout << "e(0.5f) = " << e << " (expected: 0.5)" << std::endl;
}

void testToInt() {
	std::cout << "\n=== TEST: toInt() Conversion ===" << std::endl;
	Fixed a(42.9f);
	Fixed b(-42.9f);
	Fixed c(0.5f);
	std::cout << "Fixed(42.9f).toInt() = " << a.toInt() << " (expected: 42)" << std::endl;
	std::cout << "Fixed(-42.9f).toInt() = " << b.toInt() << " (expected: -42)" << std::endl;
	std::cout << "Fixed(0.5f).toInt() = " << c.toInt() << " (expected: 0)" << std::endl;
}

void testToFloat() {
	std::cout << "\n=== TEST: toFloat() Conversion ===" << std::endl;
	Fixed a(42);
	Fixed b(-42);
	Fixed c(42.42f);
	std::cout << "Fixed(42).toFloat() = " << a.toFloat() << " (expected: 42)" << std::endl;
	std::cout << "Fixed(-42).toFloat() = " << b.toFloat() << " (expected: -42)" << std::endl;
	std::cout << "Fixed(42.42f).toFloat() = " << c.toFloat() << " (expected: ~42.4219)" << std::endl;
}

void testPrecision() {
	std::cout << "\n=== TEST: Precision Limits ===" << std::endl;
	Fixed a(0.001f);
	Fixed b(0.005f);
	Fixed c(0.003f);
	std::cout << "Fixed(0.001f) = " << a << std::endl;
	std::cout << "Fixed(0.005f) = " << b << std::endl;
	std::cout << "Fixed(0.003f) = " << c << std::endl;
}

void testBoundaryValues() {
	std::cout << "\n=== TEST: Boundary Values ===" << std::endl;
	Fixed max_int(8388607);  // 2^23 - 1 (max representable int with 8 fractional bits)
	Fixed min_int(-8388608); // -2^23 (min representable int with 8 fractional bits)
	std::cout << "Max safe int: " << max_int << std::endl;
	std::cout << "Min safe int: " << min_int << std::endl;
}

void testOutputOperator() {
	std::cout << "\n=== TEST: Output Operator (<<) ===" << std::endl;
	Fixed a(123.456f);
	Fixed b(-123.456f);
	std::cout << "a = " << a << std::endl;
	std::cout << "b = " << b << std::endl;
}

void testCopyAndAssignment() {
	std::cout << "\n=== TEST: Copy Constructor and Assignment ===" << std::endl;
	Fixed a(42.42f);
	Fixed b(a);
	Fixed c;
	c = a;
	std::cout << "Original: " << a << std::endl;
	std::cout << "Copy constructed: " << b << std::endl;
	std::cout << "Assignment copied: " << c << std::endl;
}

int main(void) {
	std::cout << "======================================" << std::endl;
	std::cout << "  EX01 COMPREHENSIVE TEST SUITE" << std::endl;
	std::cout << "======================================" << std::endl;

	testIntConstructor();
	testFloatConstructor();
	testToInt();
	testToFloat();
	testPrecision();
	testBoundaryValues();
	testOutputOperator();
	testCopyAndAssignment();

	std::cout << "\n======================================" << std::endl;
	std::cout << "  ALL TESTS COMPLETED" << std::endl;
	std::cout << "======================================" << std::endl;

	return 0;
}
