#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"

int main()
{
	std::cout << "===== Basic Test (from subject) =====" << std::endl;
	const Animal* meta = new Animal();
	const Animal* j = new Dog();
	const Animal* i = new Cat();
	std::cout << j->getType() << " " << std::endl;
	std::cout << i->getType() << " " << std::endl;
	i->makeSound();
	j->makeSound();
	meta->makeSound();

	delete meta;
	delete j;
	delete i;

	std::cout << std::endl;
	std::cout << "===== Stack Objects Test =====" << std::endl;
	{
		Animal animal;
		Dog dog;
		Cat cat;

		std::cout << "Animal type: " << animal.getType() << std::endl;
		std::cout << "Dog type: " << dog.getType() << std::endl;
		std::cout << "Cat type: " << cat.getType() << std::endl;

		animal.makeSound();
		dog.makeSound();
		cat.makeSound();
	}

	std::cout << std::endl;
	std::cout << "===== Copy Constructor Test =====" << std::endl;
	{
		Dog originalDog;
		Dog copiedDog(originalDog);

		std::cout << "Original Dog type: " << originalDog.getType() << std::endl;
		std::cout << "Copied Dog type: " << copiedDog.getType() << std::endl;
		originalDog.makeSound();
		copiedDog.makeSound();
	}

	std::cout << std::endl;
	std::cout << "===== Assignment Operator Test =====" << std::endl;
	{
		Cat cat1;
		Cat cat2;

		std::cout << "Before assignment:" << std::endl;
		std::cout << "Cat1 type: " << cat1.getType() << std::endl;
		std::cout << "Cat2 type: " << cat2.getType() << std::endl;

		cat2 = cat1;

		std::cout << "After assignment:" << std::endl;
		std::cout << "Cat1 type: " << cat1.getType() << std::endl;
		std::cout << "Cat2 type: " << cat2.getType() << std::endl;
	}

	std::cout << std::endl;
	std::cout << "===== Polymorphism Array Test =====" << std::endl;
	{
		const int size = 4;
		Animal* animals[size];

		animals[0] = new Animal();
		animals[1] = new Dog();
		animals[2] = new Cat();
		animals[3] = new Dog();

		std::cout << std::endl << "--- Make sounds ---" << std::endl;
		for (int k = 0; k < size; k++)
		{
			std::cout << "animals[" << k << "] (" << animals[k]->getType() << "): ";
			animals[k]->makeSound();
		}

		std::cout << std::endl << "--- Delete all ---" << std::endl;
		for (int k = 0; k < size; k++)
			delete animals[k];
	}

	std::cout << std::endl;
	std::cout << "===== All tests completed =====" << std::endl;

	return 0;
}
