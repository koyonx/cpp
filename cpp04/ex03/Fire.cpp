#include "Fire.hpp"
#include "ICharacter.hpp"

Fire::Fire() : AMateria("fire") {}

Fire::Fire(const Fire& other) : AMateria(other) {}

Fire::~Fire() {}

Fire& Fire::operator=(const Fire& other)
{
	if (this != &other)
		AMateria::operator=(other);
	return *this;
}

AMateria* Fire::clone() const
{
	return new Fire(*this);
}

void Fire::use(ICharacter& target)
{
	std::cout << "* burns " << target.getName() << " with a fireball *" << std::endl;
}
