#include "Zombie.hpp"

int main( void ){
    std::cout << "--- Creating a zombie on the stack (randomChump) ---" << std::endl;
    randomChump("Chump");
    std::cout << "--- The stack zombie is now out of scope and destroyed ---" << std::endl;
    std::cout << std::endl;
    std::cout << "--- Creating a zombie on the heap (newZombie) ---" << std::endl;
    Zombie* heapZombie = newZombie("Heap");
    std::cout << "--- Announcing the heap zombie ---" << std::endl;
    heapZombie->announce();
    std::cout << "--- Deleting the heap zombie ---" << std::endl;
    delete heapZombie;

    return 0;
}
