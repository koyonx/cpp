#ifndef FIRE_HPP
#define FIRE_HPP

#include "AMateria.hpp"

class Fire : public AMateria
{
public:
	Fire();
	Fire(const Fire& other);
	virtual ~Fire();

	Fire& operator=(const Fire& other);

	AMateria* clone() const;
	void use(ICharacter& target);
};

#endif
