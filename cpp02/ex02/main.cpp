#include "Fixed.hpp"
#include <iostream>

int main( void ) {

	std::cout << "----- subject test -----" << std::endl;

	Fixed a;
	Fixed const b( Fixed( 5.05f ) * Fixed( 2 ) );

	std::cout << a << std::endl;
	std::cout << ++a << std::endl;
	std::cout << a << std::endl;
	std::cout << a++ << std::endl;
	std::cout << a << std::endl;

	std::cout << b << std::endl;

	std::cout << Fixed::max( a, b ) << std::endl;

	std::cout << "----- add test -----" << std::endl;
    Fixed x;
    Fixed const y( Fixed( 5.05f ) * Fixed( 2 ) );

    std::cout << "--- Example Test ---" << std::endl;
    std::cout << x << std::endl;
    std::cout << ++x << std::endl;
    std::cout << x << std::endl;
    std::cout << x++ << std::endl;
    std::cout << x << std::endl;
    std::cout << y << std::endl;
    std::cout << Fixed::max( x, y ) << std::endl;

    std::cout << "\n--- Comparison Test ---" << std::endl;
    Fixed c(10.5f);
    Fixed d(5.5f);
    std::cout << "c: " << c << ", d: " << d << std::endl;
    std::cout << "c > d: " << (c > d) << std::endl; // 1 (True)
    std::cout << "c <= d: " << (c <= d) << std::endl; // 0 (False)
    std::cout << "c != d: " << (c != d) << std::endl; // 1 (True)

    std::cout << "\n--- Arithmetic Test ---" << std::endl;
    Fixed e(2.5f);
    Fixed f(2.0f);
    std::cout << "e + f: " << e + f << std::endl; // 4.5
    std::cout << "e * f: " << e * f << std::endl; // 5.0
    std::cout << "e / f: " << e / f << std::endl; // 1.25

    std::cout << "\n--- Min/Max Test ---" << std::endl;
    Fixed g(50);
    Fixed h(25);
    const Fixed const_g(g);
    const Fixed const_h(h);

    std::cout << "min(g, h): " << Fixed::min(g, h) << std::endl; // 25
    std::cout << "max(const_g, const_h): " << Fixed::max(const_g, const_h) << std::endl; // 50

	return 0;
}
