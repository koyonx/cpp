#include <iostream>
#include <string>
#include <limits>
#include <new>
#include "ex01/Zombie.hpp"

void testZombieHordeOverflow() {
    std::cout << "=== Testing zombieHorde for overflow issues ===" << std::endl;

    // Test 1: Negative value
    std::cout << "\n[Test 1] Negative N value (-10):" << std::endl;
    Zombie* result1 = zombieHorde(-10, "NegativeTest");
    if (result1 == NULL) {
        std::cout << "✓ Correctly returned NULL for negative N" << std::endl;
    } else {
        std::cout << "✗ ERROR: Should return NULL for negative N" << std::endl;
        delete[] result1;
    }

    // Test 2: Zero value
    std::cout << "\n[Test 2] Zero N value (0):" << std::endl;
    Zombie* result2 = zombieHorde(0, "ZeroTest");
    if (result2 == NULL) {
        std::cout << "✓ Correctly returned NULL for N = 0" << std::endl;
    } else {
        std::cout << "✗ ERROR: Should return NULL for N = 0" << std::endl;
        delete[] result2;
    }

    // Test 3: Very large value (likely to cause allocation failure)
    std::cout << "\n[Test 3] Very large N value (INT_MAX):" << std::endl;
    std::cout << "Attempting to allocate " << std::numeric_limits<int>::max() << " Zombies..." << std::endl;
    try {
        Zombie* result3 = zombieHorde(std::numeric_limits<int>::max(), "OverflowTest");
        if (result3 != NULL) {
            std::cout << "✗ Unexpectedly succeeded (should fail or take very long)" << std::endl;
            delete[] result3;
        }
    } catch (const std::bad_alloc& e) {
        std::cout << "✓ Caught std::bad_alloc exception: " << e.what() << std::endl;
        std::cout << "✓ Function properly throws exception for excessive allocation" << std::endl;
    } catch (...) {
        std::cout << "✓ Caught unknown exception (allocation failed safely)" << std::endl;
    }

    // Test 4: Large but reasonable value
    std::cout << "\n[Test 4] Large but allocatable N value (1000000):" << std::endl;
    try {
        Zombie* result4 = zombieHorde(1000000, "LargeTest");
        if (result4 != NULL) {
            std::cout << "✓ Successfully allocated 1,000,000 Zombies" << std::endl;
            std::cout << "First zombie announces: ";
            result4[0].announce();
            std::cout << "Last zombie announces: ";
            result4[999999].announce();
            delete[] result4;
            std::cout << "✓ Successfully deallocated" << std::endl;
        }
    } catch (const std::bad_alloc& e) {
        std::cout << "⚠ Could not allocate 1,000,000 Zombies (insufficient memory): " << e.what() << std::endl;
    } catch (...) {
        std::cout << "⚠ Allocation failed with unknown exception" << std::endl;
    }

    // Test 5: Boundary value near overflow
    std::cout << "\n[Test 5] Boundary test (INT_MAX / 100):" << std::endl;
    int largeN = std::numeric_limits<int>::max() / 100;
    std::cout << "Attempting to allocate " << largeN << " Zombies..." << std::endl;
    try {
        Zombie* result5 = zombieHorde(largeN, "BoundaryTest");
        if (result5 != NULL) {
            std::cout << "✗ Unexpectedly succeeded (should likely fail)" << std::endl;
            delete[] result5;
        }
    } catch (const std::bad_alloc& e) {
        std::cout << "✓ Caught std::bad_alloc: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "✓ Caught exception (allocation failed safely)" << std::endl;
    }

    std::cout << "\n=== zombieHorde overflow tests completed ===" << std::endl;
}

int main() {
    std::cout << "╔════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Overflow Test Suite for cpp01 Project   ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    testZombieHordeOverflow();

    std::cout << "\n" << std::endl;
    std::cout << "All overflow tests completed!" << std::endl;

    return 0;
}
