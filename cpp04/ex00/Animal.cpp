#include "Animal.hpp"

Animal::Animal() : type("Animal")
{
}

Animal::Animal(const Animal& other) : type(other.type)
{
}

Animal::~Animal()
{
}

Animal& Animal::operator=(const Animal& other)
{
	if (this != &other)
		type = other.type;
	return *this;
}

void Animal::makeSound() const
{
	std::cout << "Some generic animal sound" << std::endl;
}

std::string Animal::getType() const
{
	return type;
}
