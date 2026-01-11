#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"

int main()
{
	std::cout << "===== Basic Test =====" << std::endl;
	const Animal* j = new Dog();
	const Animal* i = new Cat();

	delete j;
	delete i;

	std::cout << std::endl;
	std::cout << "===== Array Test =====" << std::endl;
	{
		const int size = 4;
		Animal* animals[size];

		for (int k = 0; k < size / 2; k++)
			animals[k] = new Dog();
		for (int k = size / 2; k < size; k++)
			animals[k] = new Cat();

		std::cout << std::endl << "--- Delete all ---" << std::endl;
		for (int k = 0; k < size; k++)
			delete animals[k];
	}

	std::cout << std::endl;
	std::cout << "===== Deep Copy Test (Dog) =====" << std::endl;
	{
		Dog original;
		std::cout << std::endl << "--- Copy constructor ---" << std::endl;
		Dog copy(original);
		std::cout << std::endl << "--- End of scope ---" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "===== Deep Copy Test (Cat) =====" << std::endl;
	{
		Cat original;
		std::cout << std::endl << "--- Assignment operator ---" << std::endl;
		Cat assigned;
		assigned = original;
		std::cout << std::endl << "--- End of scope ---" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "===== All tests completed =====" << std::endl;

	return 0;
}
