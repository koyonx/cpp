#include "ClapTrap.hpp"

int main() {
	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 1: Basic ClapTrap Creation" << std::endl;
	std::cout << "========================================" << std::endl;
	std::cout << "Stats: HP=10, EP=10, AD=0" << std::endl;
	ClapTrap clap("Clappy");
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 2: attack()" << std::endl;
	std::cout << "========================================" << std::endl;
	clap.attack("Enemy1");
	clap.attack("Enemy2");
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 3: takeDamage()" << std::endl;
	std::cout << "========================================" << std::endl;
	clap.takeDamage(3);
	clap.takeDamage(2);
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 4: beRepaired()" << std::endl;
	std::cout << "========================================" << std::endl;
	clap.beRepaired(4);
	clap.beRepaired(1);
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 5: Copy Constructor (OCF)" << std::endl;
	std::cout << "========================================" << std::endl;
	ClapTrap clapCopy(clap);
	std::cout << "Original attacks:" << std::endl;
	clap.attack("Target1");
	std::cout << "Copy attacks:" << std::endl;
	clapCopy.attack("Target2");
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 6: Assignment Operator (OCF)" << std::endl;
	std::cout << "========================================" << std::endl;
	ClapTrap clapAssign("AssignTest");
	std::cout << "Before assignment:" << std::endl;
	clapAssign.attack("Target");
	clapAssign = clap;
	std::cout << "After assignment:" << std::endl;
	clapAssign.attack("Target");
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 7: Energy Depletion (10 EP)" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		ClapTrap tired("Tired");
		std::cout << std::endl;
		std::cout << "Using all 10 energy points..." << std::endl;
		for (int i = 0; i < 10; i++) {
			tired.attack("Target");
		}
		std::cout << std::endl;
		std::cout << "Trying to attack with 0 EP:" << std::endl;
		tired.attack("ShouldFail");
		std::cout << "Trying to repair with 0 EP:" << std::endl;
		tired.beRepaired(5);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 8: HP Depletion (Lethal Damage)" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		ClapTrap victim("Victim");
		std::cout << std::endl;
		victim.takeDamage(5);
		victim.takeDamage(5);
		victim.takeDamage(1);
		std::cout << "Trying to attack with 0 HP:" << std::endl;
		victim.attack("ShouldFail");
		std::cout << "Trying to repair with 0 HP:" << std::endl;
		victim.beRepaired(10);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 9: Overkill Damage" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		ClapTrap overkill("Overkill");
		std::cout << std::endl;
		overkill.takeDamage(9999);
		overkill.takeDamage(1);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 10: Multiple ClapTraps" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		ClapTrap c1("Alpha");
		ClapTrap c2("Beta");
		ClapTrap c3("Gamma");
		std::cout << std::endl;
		c1.attack("Beta");
		c2.takeDamage(0);
		c2.attack("Gamma");
		c3.takeDamage(0);
		c3.attack("Alpha");
		c1.takeDamage(0);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 11: Default Constructor" << std::endl;
	std::cout << "========================================" << std::endl;
	ClapTrap defaultClap;
	defaultClap.attack("Target");
	defaultClap.takeDamage(2);
	defaultClap.beRepaired(1);
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   Final Destruction Order" << std::endl;
	std::cout << "========================================" << std::endl;
	return 0;
}
