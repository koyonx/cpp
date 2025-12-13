#include "Zombie.hpp"

Zombie* zombieHorde( int N, std::string name ) {
	if (N <= 0) {
		return NULL;
	}
	try{
		Zombie* horde = new Zombie[N];
		for (int i = 0; i < N; ++i) {
			horde[i].setName(name);
		}
		return horde;
	}	catch (std::bad_alloc& ba) {
		std::cerr << "Failed to allocate memory for zombie horde: " << ba.what() << std::endl;
		return NULL;
	}
}
