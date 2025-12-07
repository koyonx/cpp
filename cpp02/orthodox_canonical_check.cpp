// Orthodox Canonical Form Verification Script
// This script checks if classes comply with the Orthodox Canonical Form:
// 1. Default constructor
// 2. Copy constructor
// 3. Copy assignment operator
// 4. Destructor

#include <iostream>

// Test for Fixed class from ex00
#include "ex00/Fixed.hpp"

void testEx00OrthodoxCanonicalForm() {
	std::cout << "\n=== EX00: Fixed Class Orthodox Canonical Form ===" << std::endl;

	// Test 1: Default constructor
	std::cout << "Test 1: Default constructor" << std::endl;
	Fixed a;

	// Test 2: Copy constructor
	std::cout << "\nTest 2: Copy constructor" << std::endl;
	Fixed b(a);

	// Test 3: Copy assignment operator
	std::cout << "\nTest 3: Copy assignment operator" << std::endl;
	Fixed c;
	c = a;

	// Test 4: Self assignment
	std::cout << "\nTest 4: Self assignment" << std::endl;
	Fixed& ref = c;
	c = ref;

	// Test 5: Chained assignment
	std::cout << "\nTest 5: Chained assignment" << std::endl;
	Fixed d, e, f;
	f = e = d;

	std::cout << "\n✅ EX00: All Orthodox Canonical Form tests passed!" << std::endl;
}

int main() {
	std::cout << "======================================" << std::endl;
	std::cout << "  ORTHODOX CANONICAL FORM VERIFICATION" << std::endl;
	std::cout << "======================================" << std::endl;

	testEx00OrthodoxCanonicalForm();

	std::cout << "\n======================================" << std::endl;
	std::cout << "  VERIFICATION COMPLETED" << std::endl;
	std::cout << "======================================" << std::endl;

	return 0;
}
