#include "Point.hpp"
#include <iostream>

int main(void) {
    std::cout << "--- Exercise 03: BSP (Binary Space Partitioning) Test ---" << std::endl;

    std::cout << "\nDefining Triangle: A(1.0, 1.0), B(5.0, 1.0), C(3.0, 5.0)" << std::endl;
    const Point A(1.0f, 1.0f);
    const Point B(5.0f, 1.0f);
    const Point C(3.0f, 5.0f);

    std::cout << "\n-----------------------------------------" << std::endl;
    std::cout << "          TEST CASE 1: INSIDE            " << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    Point P1(3.0f, 2.0f);
    std::cout << "Point P1(3.0, 2.0) is inside? ";
    if (bsp(A, B, C, P1)) {
        std::cout << "✅ True (Correct)" << std::endl;
    } else {
        std::cout << "❌ False (Error)" << std::endl;
    }

    std::cout << "\n-----------------------------------------" << std::endl;
    std::cout << "          TEST CASE 2: OUTSIDE           " << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    Point P2(6.0f, 6.0f);
    std::cout << "Point P2(6.0, 6.0) is inside? ";
    if (bsp(A, B, C, P2)) {
        std::cout << "❌ True (Error)" << std::endl;
    } else {
        std::cout << "✅ False (Correct)" << std::endl;
    }

    std::cout << "\n-----------------------------------------" << std::endl;
    std::cout << "          TEST CASE 3: ON EDGE           " << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    Point P3(3.0f, 1.0f);
    std::cout << "Point P3(3.0, 1.0) on edge AB is inside? ";
    if (bsp(A, B, C, P3)) {
        std::cout << "❌ True (Error - Should be False)" << std::endl;
    } else {
        std::cout << "✅ False (Correct)" << std::endl;
    }

    std::cout << "\n-----------------------------------------" << std::endl;
    std::cout << "         TEST CASE 4: ON VERTEX          " << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    Point P4(1.0f, 1.0f);
    std::cout << "Point P4(1.0, 1.0) on vertex A is inside? ";
    if (bsp(A, B, C, P4)) {
        std::cout << "❌ True (Error - Should be False)" << std::endl;
    } else {
        std::cout << "✅ False (Correct)" << std::endl;
    }

    std::cout << "\n--- All tests completed ---" << std::endl;
    return 0;
}
