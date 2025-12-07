#include "Point.hpp"

Point::Point() : x(0), y(0) {
	std::cout << "Point Default constructor called" << std::endl;
}

Point::Point(const float xValue, const float yValue) : x(xValue), y(yValue) {
	std::cout << "Point Parameterized constructor called" << std::endl;
}

Point::Point(const Point& other) : x(other.x), y(other.y) {
	std::cout << "Point Copy constructor called" << std::endl;
}

Point::~Point() {
	std::cout << "Point Destructor called" << std::endl;
}

Fixed Point::getX() const {
	return x;
}

Fixed Point::getY() const {
	return y;
}

std::ostream& operator<<(std::ostream& os, const Point& point) {
	os << "(" << point.getX() << ", " << point.getY() << ")";
	return os;
}
