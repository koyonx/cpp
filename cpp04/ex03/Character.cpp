#include "Character.hpp"

Character::Character() : name("default"), inventory{nullptr, nullptr, nullptr, nullptr} {}

Character::Character(std::string const & name) : name(name), inventory{nullptr, nullptr, nullptr, nullptr} {}

Character::Character(const Character& other) : name(other.name), inventory{nullptr, nullptr, nullptr, nullptr}
{
	for (int i = 0; i < 4; i++)
	{
		if (other.inventory[i])
			inventory[i] = other.inventory[i]->clone();
	}
}

Character::~Character()
{
	for (int i = 0; i < 4; i++)
	{
		if (inventory[i])
			delete inventory[i];
	}
}

Character& Character::operator=(const Character& other)
{
	if (this != &other)
	{
		name = other.name;
		for (int i = 0; i < 4; i++)
		{
			if (inventory[i])
				delete inventory[i];
			inventory[i] = other.inventory[i] ? other.inventory[i]->clone() : nullptr;
		}
	}
	return *this;
}

std::string const & Character::getName() const
{
	return name;
}

void Character::equip(AMateria* m)
{
	for (int i = 0; i < 4; i++)
	{
		if (!inventory[i])
		{
			inventory[i] = m;
			return;
		}
	}
}

void Character::unequip(int idx)
{
	if (idx >= 0 && idx < 4 && inventory[idx])
	{
		inventory[idx] = nullptr;
	}
}

void Character::use(int idx, ICharacter& target)
{
	if (idx >= 0 && idx < 4 && inventory[idx])
		inventory[idx]->use(target);
}
