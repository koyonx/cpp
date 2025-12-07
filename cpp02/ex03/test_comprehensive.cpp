#include "Point.hpp"
#include <iostream>

void testPointInside() {
	std::cout << "\n=== TEST: Points Inside Triangle ===" << std::endl;
	const Point A(0.0f, 0.0f);
	const Point B(10.0f, 0.0f);
	const Point C(5.0f, 10.0f);

	const Point P1(5.0f, 5.0f);   // Center
	const Point P2(3.0f, 2.0f);   // Inside near bottom
	const Point P3(7.0f, 3.0f);   // Inside near bottom right
	const Point P4(5.0f, 8.0f);   // Inside near top

	std::cout << "Triangle: A(0,0), B(10,0), C(5,10)" << std::endl;
	std::cout << "P1(5,5) inside? " << (bsp(A, B, C, P1) ? "TRUE" : "FALSE") << " (expected: TRUE)" << std::endl;
	std::cout << "P2(3,2) inside? " << (bsp(A, B, C, P2) ? "TRUE" : "FALSE") << " (expected: TRUE)" << std::endl;
	std::cout << "P3(7,3) inside? " << (bsp(A, B, C, P3) ? "TRUE" : "FALSE") << " (expected: TRUE)" << std::endl;
	std::cout << "P4(5,8) inside? " << (bsp(A, B, C, P4) ? "TRUE" : "FALSE") << " (expected: TRUE)" << std::endl;
}

void testPointOutside() {
	std::cout << "\n=== TEST: Points Outside Triangle ===" << std::endl;
	const Point A(0.0f, 0.0f);
	const Point B(10.0f, 0.0f);
	const Point C(5.0f, 10.0f);

	const Point P1(-1.0f, 0.0f);   // Left of triangle
	const Point P2(11.0f, 0.0f);   // Right of triangle
	const Point P3(5.0f, -1.0f);   // Below triangle
	const Point P4(5.0f, 11.0f);   // Above triangle
	const Point P5(15.0f, 15.0f);  // Far outside

	std::cout << "Triangle: A(0,0), B(10,0), C(5,10)" << std::endl;
	std::cout << "P1(-1,0) inside? " << (bsp(A, B, C, P1) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "P2(11,0) inside? " << (bsp(A, B, C, P2) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "P3(5,-1) inside? " << (bsp(A, B, C, P3) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "P4(5,11) inside? " << (bsp(A, B, C, P4) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "P5(15,15) inside? " << (bsp(A, B, C, P5) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
}

void testPointOnEdge() {
	std::cout << "\n=== TEST: Points On Edge (Should Return FALSE) ===" << std::endl;
	const Point A(0.0f, 0.0f);
	const Point B(10.0f, 0.0f);
	const Point C(5.0f, 10.0f);

	const Point P1(5.0f, 0.0f);    // On edge AB (midpoint)
	const Point P2(2.5f, 5.0f);    // On edge AC (midpoint)
	const Point P3(7.5f, 5.0f);    // On edge BC (midpoint)
	const Point P4(2.0f, 0.0f);    // On edge AB
	const Point P5(8.0f, 0.0f);    // On edge AB

	std::cout << "Triangle: A(0,0), B(10,0), C(5,10)" << std::endl;
	std::cout << "P1(5,0) on AB? " << (bsp(A, B, C, P1) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "P2(2.5,5) on AC? " << (bsp(A, B, C, P2) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "P3(7.5,5) on BC? " << (bsp(A, B, C, P3) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "P4(2,0) on AB? " << (bsp(A, B, C, P4) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "P5(8,0) on AB? " << (bsp(A, B, C, P5) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
}

void testPointOnVertex() {
	std::cout << "\n=== TEST: Points On Vertex (Should Return FALSE) ===" << std::endl;
	const Point A(0.0f, 0.0f);
	const Point B(10.0f, 0.0f);
	const Point C(5.0f, 10.0f);

	std::cout << "Triangle: A(0,0), B(10,0), C(5,10)" << std::endl;
	std::cout << "Point at A(0,0)? " << (bsp(A, B, C, A) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "Point at B(10,0)? " << (bsp(A, B, C, B) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "Point at C(5,10)? " << (bsp(A, B, C, C) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
}

void testNegativeCoordinates() {
	std::cout << "\n=== TEST: Triangle with Negative Coordinates ===" << std::endl;
	const Point A(-5.0f, -5.0f);
	const Point B(5.0f, -5.0f);
	const Point C(0.0f, 5.0f);

	const Point P1(0.0f, 0.0f);    // Inside
	const Point P2(-2.0f, -2.0f);  // Inside
	const Point P3(10.0f, 0.0f);   // Outside

	std::cout << "Triangle: A(-5,-5), B(5,-5), C(0,5)" << std::endl;
	std::cout << "P1(0,0) inside? " << (bsp(A, B, C, P1) ? "TRUE" : "FALSE") << " (expected: TRUE)" << std::endl;
	std::cout << "P2(-2,-2) inside? " << (bsp(A, B, C, P2) ? "TRUE" : "FALSE") << " (expected: TRUE)" << std::endl;
	std::cout << "P3(10,0) inside? " << (bsp(A, B, C, P3) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
}

void testVerySmallTriangle() {
	std::cout << "\n=== TEST: Very Small Triangle ===" << std::endl;
	const Point A(0.0f, 0.0f);
	const Point B(0.1f, 0.0f);
	const Point C(0.05f, 0.1f);

	const Point P1(0.05f, 0.05f);  // Inside
	const Point P2(1.0f, 1.0f);    // Outside

	std::cout << "Small Triangle: A(0,0), B(0.1,0), C(0.05,0.1)" << std::endl;
	std::cout << "P1(0.05,0.05) inside? " << (bsp(A, B, C, P1) ? "TRUE" : "FALSE") << " (expected: TRUE)" << std::endl;
	std::cout << "P2(1,1) inside? " << (bsp(A, B, C, P2) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
}

void testRightTriangle() {
	std::cout << "\n=== TEST: Right Triangle ===" << std::endl;
	const Point A(0.0f, 0.0f);
	const Point B(10.0f, 0.0f);
	const Point C(0.0f, 10.0f);

	const Point P1(2.0f, 2.0f);    // Inside
	const Point P2(6.0f, 6.0f);    // Outside (beyond hypotenuse)
	const Point P3(5.0f, 0.0f);    // On edge AB

	std::cout << "Right Triangle: A(0,0), B(10,0), C(0,10)" << std::endl;
	std::cout << "P1(2,2) inside? " << (bsp(A, B, C, P1) ? "TRUE" : "FALSE") << " (expected: TRUE)" << std::endl;
	std::cout << "P2(6,6) inside? " << (bsp(A, B, C, P2) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
	std::cout << "P3(5,0) on edge? " << (bsp(A, B, C, P3) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
}

void testObtuseTriangle() {
	std::cout << "\n=== TEST: Obtuse Triangle ===" << std::endl;
	const Point A(0.0f, 0.0f);
	const Point B(10.0f, 0.0f);
	const Point C(1.0f, 2.0f);

	const Point P1(3.0f, 0.5f);    // Inside
	const Point P2(5.0f, 5.0f);    // Outside

	std::cout << "Obtuse Triangle: A(0,0), B(10,0), C(1,2)" << std::endl;
	std::cout << "P1(3,0.5) inside? " << (bsp(A, B, C, P1) ? "TRUE" : "FALSE") << " (expected: TRUE)" << std::endl;
	std::cout << "P2(5,5) inside? " << (bsp(A, B, C, P2) ? "TRUE" : "FALSE") << " (expected: FALSE)" << std::endl;
}

int main(void) {
	std::cout << "======================================" << std::endl;
	std::cout << "  EX03 COMPREHENSIVE BSP TEST SUITE" << std::endl;
	std::cout << "======================================" << std::endl;

	testPointInside();
	testPointOutside();
	testPointOnEdge();
	testPointOnVertex();
	testNegativeCoordinates();
	testVerySmallTriangle();
	testRightTriangle();
	testObtuseTriangle();

	std::cout << "\n======================================" << std::endl;
	std::cout << "  ALL TESTS COMPLETED" << std::endl;
	std::cout << "======================================" << std::endl;

	return 0;
}
