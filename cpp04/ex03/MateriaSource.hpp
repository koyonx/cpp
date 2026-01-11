#ifndef MATERIASOURCE_HPP
#define MATERIASOURCE_HPP

#include "IMateriaSource.hpp"
#include "AMateria.hpp"

class MateriaSource : public IMateriaSource
{
private:
	AMateria* storage[4];

public:
	MateriaSource();
	MateriaSource(const MateriaSource& other);
	virtual ~MateriaSource();

	MateriaSource& operator=(const MateriaSource& other);

	virtual void learnMateria(AMateria*);
	virtual AMateria* createMateria(std::string const & type);
};

#endif
