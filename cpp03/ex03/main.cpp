#include "DiamondTrap.hpp"

int main() {
	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 1: Basic DiamondTrap Creation" << std::endl;
	std::cout << "========================================" << std::endl;
	DiamondTrap diamond("Diamond");
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 2: whoAmI() function" << std::endl;
	std::cout << "========================================" << std::endl;
	diamond.whoAmI();
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 3: attack() from ScavTrap" << std::endl;
	std::cout << "========================================" << std::endl;
	diamond.attack("Enemy1");
	diamond.attack("Enemy2");
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 4: takeDamage()" << std::endl;
	std::cout << "========================================" << std::endl;
	diamond.takeDamage(30);
	diamond.takeDamage(25);
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 5: beRepaired()" << std::endl;
	std::cout << "========================================" << std::endl;
	diamond.beRepaired(20);
	diamond.beRepaired(10);
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 6: guardGate() from ScavTrap" << std::endl;
	std::cout << "========================================" << std::endl;
	diamond.guardGate();
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 7: highFivesGuys() from FragTrap" << std::endl;
	std::cout << "========================================" << std::endl;
	diamond.highFivesGuys();
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 8: Copy Constructor" << std::endl;
	std::cout << "========================================" << std::endl;
	DiamondTrap diamondCopy(diamond);
	std::cout << "Original: ";
	diamond.whoAmI();
	std::cout << "Copy: ";
	diamondCopy.whoAmI();
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 9: Assignment Operator" << std::endl;
	std::cout << "========================================" << std::endl;
	DiamondTrap diamondAssign("AssignTest");
	std::cout << "Before assignment: ";
	diamondAssign.whoAmI();
	diamondAssign = diamond;
	std::cout << "After assignment: ";
	diamondAssign.whoAmI();
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 10: Lethal Damage (HP to 0)" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		DiamondTrap victim("Victim");
		std::cout << std::endl;
		victim.takeDamage(50);
		victim.takeDamage(50);
		victim.takeDamage(10);
		std::cout << "Trying to attack with 0 HP:" << std::endl;
		victim.attack("Nobody");
		std::cout << "Trying to repair with 0 HP:" << std::endl;
		victim.beRepaired(100);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 11: Energy Depletion" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		DiamondTrap tired("Tired");
		std::cout << std::endl;
		std::cout << "Using all 50 energy points..." << std::endl;
		for (int i = 0; i < 50; i++) {
			tired.attack("Target");
		}
		std::cout << std::endl;
		std::cout << "Trying to attack with 0 energy:" << std::endl;
		tired.attack("Nobody");
		std::cout << "Trying to repair with 0 energy:" << std::endl;
		tired.beRepaired(10);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 12: Default Constructor" << std::endl;
	std::cout << "========================================" << std::endl;
	DiamondTrap defaultDiamond;
	defaultDiamond.whoAmI();
	defaultDiamond.attack("DefaultTarget");
	defaultDiamond.guardGate();
	defaultDiamond.highFivesGuys();
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 13: ScavTrap standalone test" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		ScavTrap scav("ScavTest");
		std::cout << std::endl;
		scav.attack("Enemy");
		scav.takeDamage(20);
		scav.beRepaired(10);
		scav.guardGate();
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 14: FragTrap standalone test" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		FragTrap frag("FragTest");
		std::cout << std::endl;
		frag.attack("Enemy");
		frag.takeDamage(40);
		frag.beRepaired(15);
		frag.highFivesGuys();
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 15: ClapTrap standalone test" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		ClapTrap clap("ClapTest");
		std::cout << std::endl;
		clap.attack("Enemy");
		clap.takeDamage(5);
		clap.beRepaired(3);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 16: Multiple DiamondTraps" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		DiamondTrap d1("Alpha");
		DiamondTrap d2("Beta");
		DiamondTrap d3("Gamma");
		std::cout << std::endl;
		d1.whoAmI();
		d2.whoAmI();
		d3.whoAmI();
		std::cout << std::endl;
		d1.attack("Beta");
		d2.takeDamage(30);
		d2.attack("Gamma");
		d3.takeDamage(30);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 17: Massive Overkill Damage" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		DiamondTrap overkill("Overkill");
		std::cout << std::endl;
		overkill.takeDamage(9999);
		overkill.takeDamage(1);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   Final Destruction Order" << std::endl;
	std::cout << "========================================" << std::endl;
	return 0;
}
