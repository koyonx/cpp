#include "ClapTrap.hpp"

ClapTrap::ClapTrap() : name("Default"), hitPoints(10), energyPoints(10), attackDamage(0) {
	std::cout << "ClapTrap Default constructor called for " << name << std::endl;
}

ClapTrap::ClapTrap(const std::string& trapName) : name(trapName), hitPoints(10), energyPoints(10), attackDamage(0) {
	std::cout << "ClapTrap Parameterized constructor called for " << name << std::endl;
}

ClapTrap::ClapTrap(const ClapTrap& other)
	: name(other.name), hitPoints(other.hitPoints), energyPoints(other.energyPoints), attackDamage(other.attackDamage) {
	std::cout << "ClapTrap Copy constructor called for " << name << std::endl;
}

ClapTrap& ClapTrap::operator=(const ClapTrap& other) {
	std::cout << "ClapTrap Copy assignment operator called" << std::endl;
	if (this != &other) {
		name = other.name;
		hitPoints = other.hitPoints;
		energyPoints = other.energyPoints;
		attackDamage = other.attackDamage;
	}
	return *this;
}

ClapTrap::~ClapTrap() {
	std::cout << "ClapTrap Destructor called for " << name << std::endl;
}

void ClapTrap::attack(const std::string& target) {
	if (energyPoints == 0 || hitPoints == 0) {
		std::cout << "ClapTrap " << name << " has no energy or hit points left to attack." << std::endl;
		return;
	}
	--energyPoints;
	std::cout << "ClapTrap " << name << " attacks " << target
			  << ", causing " << attackDamage << " points of damage! "
			  << "(Energy left: " << energyPoints << ")" << std::endl;
}

void ClapTrap::takeDamage(unsigned int amount) {
	if (hitPoints == 0) {
		std::cout << "ClapTrap " << name << " is already at 0 hit points." << std::endl;
		return;
	}
	hitPoints = (amount >= hitPoints) ? 0 : hitPoints - amount;
	std::cout << "ClapTrap " << name << " takes " << amount
			  << " points of damage! (Hit points left: " << hitPoints << ")" << std::endl;
}

void ClapTrap::beRepaired(unsigned int amount) {
	if (energyPoints == 0 || hitPoints == 0) {
		std::cout << "ClapTrap " << name << " has no energy or hit points left to repair." << std::endl;
		return;
	}
	--energyPoints;
	hitPoints += amount;
	std::cout << "ClapTrap " << name << " repairs itself for " << amount
			  << " hit points! (Hit points: " << hitPoints
			  << ", Energy left: " << energyPoints << ")" << std::endl;
}
