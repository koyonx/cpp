#include "MateriaSource.hpp"

MateriaSource::MateriaSource()
{
	for (int i = 0; i < 4; i++)
		storage[i] = NULL;
}

MateriaSource::MateriaSource(const MateriaSource& other)
{
	for (int i = 0; i < 4; i++)
	{
		if (other.storage[i])
			storage[i] = other.storage[i]->clone();
		else
			storage[i] = NULL;
	}
}

MateriaSource::~MateriaSource()
{
	for (int i = 0; i < 4; i++)
	{
		if (storage[i])
			delete storage[i];
	}
}

MateriaSource& MateriaSource::operator=(const MateriaSource& other)
{
	if (this != &other)
	{
		for (int i = 0; i < 4; i++)
		{
			if (storage[i])
				delete storage[i];
			if (other.storage[i])
				storage[i] = other.storage[i]->clone();
			else
				storage[i] = NULL;
		}
	}
	return *this;
}

// Subject: "Copies the Materia passed as a parameter and stores it in memory
// so it can be cloned later."  We clone the input and take ownership of it
// in both successful and over-capacity paths, so the caller never has to
// worry about leaking the pointer it just passed in.
void MateriaSource::learnMateria(AMateria* m)
{
	if (!m)
		return;
	for (int i = 0; i < 4; i++)
	{
		if (!storage[i])
		{
			storage[i] = m->clone();
			delete m;
			return;
		}
	}
	delete m;
}

AMateria* MateriaSource::createMateria(std::string const & type)
{
	for (int i = 0; i < 4; i++)
	{
		if (storage[i] && storage[i]->getType() == type)
			return storage[i]->clone();
	}
	return NULL;
}
