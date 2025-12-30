#include "ScavTrap.hpp"

ScavTrap::ScavTrap() : ClapTrap("Default_Scav") {
	hitPoints = 100;
	energyPoints = 50;
	attackDamage = 20;
	std::cout << "ScavTrap Default constructor called for " << name << std::endl;
}

ScavTrap::ScavTrap(const std::string& trapName) : ClapTrap(trapName) {
	hitPoints = 100;
	energyPoints = 50;
	attackDamage = 20;
	std::cout << "ScavTrap Parameterized constructor called for " << name << std::endl;
}

ScavTrap::ScavTrap(const ScavTrap& other) : ClapTrap(other) {
	std::cout << "ScavTrap Copy constructor called for " << name << std::endl;
}

ScavTrap& ScavTrap::operator=(const ScavTrap& other) {
	std::cout << "ScavTrap Copy assignment operator called" << std::endl;
	if (this != &other) {
		ClapTrap::operator=(other);
	}
	return *this;
}

ScavTrap::~ScavTrap() {
	std::cout << "ScavTrap Destructor called for " << name << std::endl;
}

void ScavTrap::attack(const std::string& target) {
	if (energyPoints == 0 || hitPoints == 0) {
		std::cout << "ScavTrap " << name << " has no energy or hit points left to attack." << std::endl;
		return;
	}
	--energyPoints;
	std::cout << "ScavTrap " << name << " attacks " << target
			  << ", causing " << attackDamage << " points of damage! "
			  << "(Energy left: " << energyPoints << ")" << std::endl;
}

void ScavTrap::guardGate() {
	std::cout << "ScavTrap " << name << " is now in Gate Keeper mode." << std::endl;
}
