#include "ClapTrap.hpp"

int main(void) {
    std::cout << "--- 1. ClapTrap Creation Test ---" << std::endl;
    ClapTrap bot("Clappy");
    ClapTrap target("TargetBot");

    std::cout << "\n--- Initial States ---" << std::endl;
    std::cout << "\n--- 2. Attack Test (Costs 1 EP) ---" << std::endl;
    bot.attack("TargetBot");

    target.takeDamage(0);

    std::cout << "\n--- 3. Take Damage Test ---" << std::endl;
    unsigned int test_damage = 5;
    target.takeDamage(test_damage);

    std::cout << "\n--- 4. Repair Test (Costs 1 EP) ---" << std::endl;
    unsigned int repair_amount = 3;
    target.beRepaired(repair_amount);

    std::cout << "\n--- 5. Out of Energy Test ---" << std::endl;
    ClapTrap lowEnergyBot("TiredBot");

    for (int i = 0; i < 9; ++i) {
        lowEnergyBot.attack("EnergyDummy");
    }

    std::cout << "Attempt 10: Final attack to use last EP." << std::endl;
    lowEnergyBot.attack("FinalDummy");

    std::cout << "Attempt 11: Attack with 0 EP (should fail)." << std::endl;
    lowEnergyBot.attack("FailedTarget");

    std::cout << "Attempt 12: Repair with 0 EP (should fail)." << std::endl;
    lowEnergyBot.beRepaired(5);

    std::cout << "\n--- 6. Out of Hit Points Test ---" << std::endl;
    target.takeDamage(100);

    std::cout << "Attempt to attack with 0 HP (should fail)." << std::endl;
    target.attack("DeadTarget");

    std::cout << "Attempt to repair with 0 HP (should fail)." << std::endl;
    target.beRepaired(1);

    std::cout << "\n--- 7. Destructor Test ---" << std::endl;

    return 0;
}
