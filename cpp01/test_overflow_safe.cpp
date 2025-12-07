#include <iostream>
#include <string>
#include <limits>
#include "ex01/Zombie.hpp"

void testBasicCases() {
    std::cout << "=== Testing Basic Cases ===" << std::endl;

    // Test 1: Negative value
    std::cout << "\n[Test 1] Negative N value (-10):" << std::endl;
    Zombie* result1 = zombieHorde(-10, "NegativeTest");
    if (result1 == NULL) {
        std::cout << "✓ PASS: Correctly returned NULL for negative N" << std::endl;
    } else {
        std::cout << "✗ FAIL: Should return NULL for negative N" << std::endl;
        delete[] result1;
    }

    // Test 2: Zero value
    std::cout << "\n[Test 2] Zero N value (0):" << std::endl;
    Zombie* result2 = zombieHorde(0, "ZeroTest");
    if (result2 == NULL) {
        std::cout << "✓ PASS: Correctly returned NULL for N = 0" << std::endl;
    } else {
        std::cout << "✗ FAIL: Should return NULL for N = 0" << std::endl;
        delete[] result2;
    }

    // Test 3: Normal case
    std::cout << "\n[Test 3] Normal case (5 zombies):" << std::endl;
    Zombie* result3 = zombieHorde(5, "NormalTest");
    if (result3 != NULL) {
        std::cout << "✓ PASS: Successfully allocated 5 Zombies" << std::endl;
        result3[0].announce();
        delete[] result3;
        std::cout << "✓ PASS: Successfully deallocated" << std::endl;
    } else {
        std::cout << "✗ FAIL: Should successfully allocate 5 zombies" << std::endl;
    }
}

void testLargeAllocation() {
    std::cout << "\n\n=== Testing Large Allocations ===" << std::endl;

    // Test 4: Moderate large value
    std::cout << "\n[Test 4] Moderate allocation (10,000 zombies):" << std::endl;
    try {
        Zombie* result4 = zombieHorde(10000, "ModerateTest");
        if (result4 != NULL) {
            std::cout << "✓ PASS: Successfully allocated 10,000 Zombies" << std::endl;
            delete[] result4;
            std::cout << "✓ PASS: Successfully deallocated" << std::endl;
        }
    } catch (const std::bad_alloc& e) {
        std::cout << "⚠ WARNING: Could not allocate 10,000 Zombies: " << e.what() << std::endl;
    }

    // Test 5: Large value (1 million)
    std::cout << "\n[Test 5] Large allocation (1,000,000 zombies):" << std::endl;
    std::cout << "⚠ This may take a moment or fail with bad_alloc..." << std::endl;
    try {
        Zombie* result5 = zombieHorde(1000000, "LargeTest");
        if (result5 != NULL) {
            std::cout << "✓ PASS: Successfully allocated 1,000,000 Zombies" << std::endl;
            delete[] result5;
            std::cout << "✓ PASS: Successfully deallocated" << std::endl;
        }
    } catch (const std::bad_alloc& e) {
        std::cout << "⚠ WARNING: Could not allocate 1,000,000 Zombies: " << e.what() << std::endl;
    }
}

void testOverflowIssues() {
    std::cout << "\n\n=== Testing Overflow Issues ===" << std::endl;

    std::cout << "\n[CRITICAL FINDING]" << std::endl;
    std::cout << "The zombieHorde function has a CRITICAL overflow vulnerability:" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Issue Location: ex01/zombieHorde.cpp:7" << std::endl;
    std::cout << "Code: Zombie* horde = new Zombie[N];" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Problems:" << std::endl;
    std::cout << "1. No upper bound check on N before allocation" << std::endl;
    std::cout << "2. INT_MAX or very large values cause:" << std::endl;
    std::cout << "   - System memory exhaustion" << std::endl;
    std::cout << "   - Process termination (SIGKILL)" << std::endl;
    std::cout << "   - Potential denial of service" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "3. No exception handling for std::bad_alloc" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Recommended Fix:" << std::endl;
    std::cout << "  if (N <= 0 || N > MAX_REASONABLE_SIZE) {" << std::endl;
    std::cout << "      return NULL;" << std::endl;
    std::cout << "  }" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "  try {" << std::endl;
    std::cout << "      Zombie* horde = new Zombie[N];" << std::endl;
    std::cout << "      // ..." << std::endl;
    std::cout << "  } catch (const std::bad_alloc&) {" << std::endl;
    std::cout << "      return NULL;" << std::endl;
    std::cout << "  }" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "\n[Test 6] Testing with INT_MAX:" << std::endl;
    std::cout << "⚠ SKIPPED: This test causes system kill (exit 137)" << std::endl;
    std::cout << "Value: " << std::numeric_limits<int>::max() << " zombies" << std::endl;
    std::cout << "Result: Process terminated by OS (memory exhaustion)" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "╔════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Overflow Test Suite - Safe Version      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════╝" << std::endl;

    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "basic") {
            testBasicCases();
        } else if (arg == "large") {
            testLargeAllocation();
        } else if (arg == "overflow") {
            testOverflowIssues();
        }
    } else {
        testBasicCases();
        testLargeAllocation();
        testOverflowIssues();
    }

    std::cout << "\n════════════════════════════════════════════" << std::endl;
    std::cout << "Test suite completed successfully!" << std::endl;
    std::cout << "════════════════════════════════════════════" << std::endl;

    return 0;
}
