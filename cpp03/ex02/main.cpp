#include "ClapTrap.hpp"
#include "FragTrap.hpp"

int main() {
	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 1: FragTrap Creation" << std::endl;
	std::cout << "========================================" << std::endl;
	std::cout << "Stats: HP=100, EP=100, AD=30" << std::endl;
	std::cout << "Construction order: ClapTrap -> FragTrap" << std::endl;
	FragTrap frag("Fraggy");
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 2: attack() - inherited from ClapTrap" << std::endl;
	std::cout << "========================================" << std::endl;
	frag.attack("Enemy1");
	frag.attack("Enemy2");
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 3: takeDamage() - inherited" << std::endl;
	std::cout << "========================================" << std::endl;
	frag.takeDamage(30);
	frag.takeDamage(20);
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 4: beRepaired() - inherited" << std::endl;
	std::cout << "========================================" << std::endl;
	frag.beRepaired(25);
	frag.beRepaired(10);
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 5: highFivesGuys() - FragTrap special" << std::endl;
	std::cout << "========================================" << std::endl;
	frag.highFivesGuys();
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 6: Copy Constructor (OCF)" << std::endl;
	std::cout << "========================================" << std::endl;
	FragTrap fragCopy(frag);
	std::cout << "Original attacks:" << std::endl;
	frag.attack("Target1");
	std::cout << "Copy attacks:" << std::endl;
	fragCopy.attack("Target2");
	fragCopy.highFivesGuys();
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 7: Assignment Operator (OCF)" << std::endl;
	std::cout << "========================================" << std::endl;
	FragTrap fragAssign("AssignTest");
	std::cout << "Before assignment:" << std::endl;
	fragAssign.attack("Target");
	fragAssign = frag;
	std::cout << "After assignment:" << std::endl;
	fragAssign.attack("Target");
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 8: Energy Depletion (100 EP)" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		FragTrap tired("Tired");
		std::cout << std::endl;
		std::cout << "Using all 100 energy points..." << std::endl;
		for (int i = 0; i < 100; i++) {
			tired.attack("Target");
		}
		std::cout << std::endl;
		std::cout << "Trying to attack with 0 EP:" << std::endl;
		tired.attack("ShouldFail");
		std::cout << "Trying to repair with 0 EP:" << std::endl;
		tired.beRepaired(10);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 9: HP Depletion (Lethal Damage)" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		FragTrap victim("Victim");
		std::cout << std::endl;
		victim.takeDamage(50);
		victim.takeDamage(50);
		victim.takeDamage(10);
		std::cout << "Trying to attack with 0 HP:" << std::endl;
		victim.attack("ShouldFail");
		std::cout << "Trying to repair with 0 HP:" << std::endl;
		victim.beRepaired(100);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 10: ClapTrap vs FragTrap" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		ClapTrap clap("ClapTest");
		FragTrap fragTest("FragTest");
		std::cout << std::endl;
		std::cout << "ClapTrap attacks (AD=0):" << std::endl;
		clap.attack("Target");
		std::cout << "FragTrap attacks (AD=30):" << std::endl;
		fragTest.attack("Target");
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 11: Multiple FragTraps" << std::endl;
	std::cout << "========================================" << std::endl;
	{
		FragTrap f1("Alpha");
		FragTrap f2("Beta");
		FragTrap f3("Gamma");
		std::cout << std::endl;
		f1.attack("Beta");
		f2.takeDamage(30);
		f2.attack("Gamma");
		f3.takeDamage(30);
		f3.highFivesGuys();
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 12: Destruction Chain" << std::endl;
	std::cout << "========================================" << std::endl;
	std::cout << "Destruction order: FragTrap -> ClapTrap" << std::endl;
	{
		FragTrap temp("TempFrag");
		std::cout << "--- Scope ending ---" << std::endl;
	}
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   TEST 13: Default Constructor" << std::endl;
	std::cout << "========================================" << std::endl;
	FragTrap defaultFrag;
	defaultFrag.attack("Target");
	defaultFrag.highFivesGuys();
	std::cout << std::endl;

	std::cout << "========================================" << std::endl;
	std::cout << "   Final Destruction Order" << std::endl;
	std::cout << "========================================" << std::endl;
	return 0;
}
