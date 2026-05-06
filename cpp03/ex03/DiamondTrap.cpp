#include "DiamondTrap.hpp"

DiamondTrap::DiamondTrap()
	: ClapTrap("default_clap_name"),
	  ScavTrap("default_scav"),
	  FragTrap("default_frag") {
	this->name = "default";
	this->ClapTrap::name = this->name + "_clap_name";
	this->hitPoints = FragTrap::hitPoints;
	this->energyPoints = ScavTrap::energyPoints;
	this->attackDamage = FragTrap::attackDamage;
	std::cout << "DiamondTrap Default constructor called for " << name << std::endl;
}

DiamondTrap::DiamondTrap(const std::string& diamondName)
	: ClapTrap(diamondName + "_clap_name"),
	  ScavTrap(diamondName + "_scav"),
	  FragTrap(diamondName + "_frag") {
	this->name = diamondName;
	this->hitPoints = FragTrap::hitPoints;
	this->energyPoints = ScavTrap::energyPoints;
	this->attackDamage = FragTrap::attackDamage;
	std::cout << "DiamondTrap Parameterized constructor called for " << name << std::endl;
}

DiamondTrap::DiamondTrap(const DiamondTrap& other)
	: ClapTrap(other), ScavTrap(other), FragTrap(other), name(other.name) {
	std::cout << "DiamondTrap Copy constructor called for " << name << std::endl;
}

DiamondTrap& DiamondTrap::operator=(const DiamondTrap& other) {
	std::cout << "DiamondTrap Copy assignment operator called" << std::endl;
	if (this != &other) {
		ClapTrap::operator=(other);
		name = other.name;
	}
	return *this;
}

DiamondTrap::~DiamondTrap() {
	std::cout << "DiamondTrap Destructor called for " << name << std::endl;
}

void DiamondTrap::whoAmI() {
	std::cout << "I am DiamondTrap " << name << " and my ClapTrap name is " << ClapTrap::name << std::endl;
}
