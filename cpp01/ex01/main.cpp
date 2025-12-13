#include "Zombie.hpp"

int main(void) {
    std::cout << "--- Creating a horde of 5 zombies ---" << std::endl;
    int numZombies = 5;
    Zombie* zombieArmy = zombieHorde(numZombies, "HordeMember");

    if (zombieArmy != NULL) {
        std::cout << "\n--- Zombies announce themselves ---" << std::endl;
        for (int i = 0; i < numZombies; ++i) {
            zombieArmy[i].announce();
        }

        std::cout << "\n--- Deallocating the zombie horde ---" << std::endl;
        delete[] zombieArmy;
    } else {
        std::cout << "Failed to create zombie horde." << std::endl;
    }

    std::cout << "\n--- Testing with a negative number ---" << std::endl;
    Zombie* emptyHorde = zombieHorde(-5, "BadGuy");
    if (emptyHorde == NULL) {
        std::cout << "Function correctly returned NULL for N <= 0." << std::endl;
    }

    // std::cout << "--- Creating a horde of INT_MAX zombies ---" << std::endl;
    // int int_max = 2147483647;
    // Zombie* zombieArmyForIntMax = zombieHorde(int_max, "HordeMember");

    // if (zombieArmyForIntMax != NULL) {
    //     std::cout << "\n--- Zombies announce themselves ---" << std::endl;
    //     for (int i = 0; i < int_max; ++i) {
    //         zombieArmyForIntMax[i].announce();
    //     }

    //     std::cout << "\n--- Deallocating the zombie horde ---" << std::endl;
    //     delete[] zombieArmyForIntMax;
    // } else {
    //     std::cout << "Failed to create zombie horde." << std::endl;
    // }

    // std::cout << "\n--- Testing with a overflow number ---" << std::endl;
    // Zombie* emptyHorde = zombieHorde(2147483648, "OverFlowGuy");
    // if (emptyHorde == NULL) {
    //     std::cout << "Function correctly returned NULL for N > INT_MAX" << std::endl;
    // }

    return 0;
}
