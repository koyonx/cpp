#include "AMateria.hpp"
#include "ICharacter.hpp"

AMateria::AMateria() : type("default") {}

AMateria::AMateria(std::string const & type) : type(type) {}

AMateria::AMateria(const AMateria& other) : type(other.type) {}

AMateria::~AMateria() {}

// Per subject: "While assigning a Materia to another, copying the type doesn't make sense."
// The concrete type (Ice / Cure / ...) is the identity of the materia, so we keep it.
AMateria& AMateria::operator=(const AMateria& other)
{
	(void)other;
	return *this;
}

std::string const & AMateria::getType() const
{
	return type;
}

void AMateria::use(ICharacter& target)
{
	(void)target;
}
