#ifndef POINT_HPP
#define POINT_HPP

#include "Fixed.hpp"

class Point {
private:
	Fixed const x;
	Fixed const y;
	Point& operator=(const Point& other);
public:
	Point();
	Point(const float xValue, const float yValue);
	Point(const Point& other);
	~Point();
	Fixed getX() const;
	Fixed getY() const;
};

std::ostream& operator<<(std::ostream& os, const Point& point);
bool bsp(const Point a, const Point b, const Point c, const Point point);

#endif
