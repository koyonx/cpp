#include "MateriaSource.hpp"

MateriaSource::MateriaSource() : count(0)
{
	for (int i = 0; i < 4; i++)
		storage[i] = nullptr;
}
