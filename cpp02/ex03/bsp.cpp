#include "Point.hpp"
static Fixed cross_product(Point const p1, Point const p2, Point const p3) {
    return (p1.getX() - p3.getX()) * (p2.getY() - p3.getY()) -
           (p2.getX() - p3.getX()) * (p1.getY() - p3.getY());
}

bool bsp(Point const a, Point const b, Point const c, Point const point) {
    Fixed cp1 = cross_product(a, b, point);
    Fixed cp2 = cross_product(b, c, point);
    Fixed cp3 = cross_product(c, a, point);

    if (cp1 == Fixed(0) || cp2 == Fixed(0) || cp3 == Fixed(0)) {
        return false;
    }

    bool all_positive = (cp1 > Fixed(0)) && (cp2 > Fixed(0)) && (cp3 > Fixed(0));
    bool all_negative = (cp1 < Fixed(0)) && (cp2 < Fixed(0)) && (cp3 < Fixed(0));

    return all_positive || all_negative;
}
