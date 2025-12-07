#include "Fixed.hpp"

Fixed::Fixed() : fixedPointValue(0) {
	std::cout << "Default constructor called" << std::endl;
}

Fixed::Fixed(const int intValue) {
	fixedPointValue = intValue << fractionalBits;
	std::cout << "Int constructor called" << std::endl;
}

Fixed::Fixed(const float floatValue) {
	fixedPointValue = static_cast<int>(roundf(floatValue * (1 << fractionalBits)));
	std::cout << "Float constructor called" << std::endl;
}

Fixed::Fixed(const Fixed& other) : fixedPointValue(other.fixedPointValue) {
	std::cout << "Copy constructor called" << std::endl;
}

Fixed::~Fixed() {
	std::cout << "Destructor called" << std::endl;
}

float Fixed::toFloat() const {
	return static_cast<float>(fixedPointValue) / (1 << fractionalBits);
}

int Fixed::toInt() const {
	return fixedPointValue >> fractionalBits;
}

int Fixed::getRawBits() const {
	return fixedPointValue;
}

void Fixed::setRawBits(int const raw) {
	fixedPointValue = raw;
}

Fixed& Fixed::operator=(const Fixed& other) {
	std::cout << "Copy assignment operator called" << std::endl;
	if (this != &other) {
		fixedPointValue = other.fixedPointValue;
	}
	return *this;
}

std::ostream& operator<<(std::ostream& os, const Fixed& fixed) {
	os << fixed.toFloat();
	return os;
}

bool Fixed::operator>(const Fixed& other) const {
	return fixedPointValue > other.fixedPointValue;
}

bool Fixed::operator<(const Fixed& other) const {
	return fixedPointValue < other.fixedPointValue;
}

bool Fixed::operator>=(const Fixed& other) const {
	return fixedPointValue >= other.fixedPointValue;
}

bool Fixed::operator<=(const Fixed& other) const {
	return fixedPointValue <= other.fixedPointValue;
}

bool Fixed::operator==(const Fixed& other) const {
	return fixedPointValue == other.fixedPointValue;
}

bool Fixed::operator!=(const Fixed& other) const {
	return fixedPointValue != other.fixedPointValue;
}

Fixed Fixed::operator+(const Fixed& other) const {
	return Fixed(this->toFloat() + other.toFloat());
}

Fixed Fixed::operator-(const Fixed& other) const {
	return Fixed(this->toFloat() - other.toFloat());
}

Fixed Fixed::operator*(const Fixed& other) const {
	return Fixed(this->toFloat() * other.toFloat());
}

Fixed Fixed::operator/(const Fixed& other) const {
	return Fixed(this->toFloat() / other.toFloat());
}

Fixed& Fixed::operator++() {
	++fixedPointValue;
	return *this;
}

Fixed Fixed::operator++(int) {
	Fixed temp(*this);
	++fixedPointValue;
	return temp;
}

Fixed& Fixed::operator--() {
	--fixedPointValue;
	return *this;
}

Fixed Fixed::operator--(int) {
	Fixed temp(*this);
	--fixedPointValue;
	return temp;
}

Fixed& Fixed::min(Fixed& a, Fixed& b) {
	return (a < b) ? a : b;
}

const Fixed& Fixed::min(const Fixed& a, const Fixed& b) {
	return (a < b) ? a : b;
}

Fixed& Fixed::max(Fixed& a, Fixed& b) {
	return (a > b) ? a : b;
}

const Fixed& Fixed::max(const Fixed& a, const Fixed& b) {
	return (a > b) ? a : b;
}

