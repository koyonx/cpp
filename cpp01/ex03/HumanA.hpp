#ifndef HUMANA_HPP
#define HUMANA_HPP
#include <string>
#include <iostream>
#include "Weapon.hpp"
class HumanA {
public:
	HumanA(std::string name, Weapon& weapon);
	~HumanA(void);
	void attack(void) const;
private:
	std::string _name;
	Weapon& _weapon;
};
#endif
