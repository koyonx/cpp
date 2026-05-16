#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"

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

		std::cout << std::endl << "--- Make sounds via Animal* ---" << std::endl;
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
	std::cout << "===== WrongAnimal / WrongCat Test (subject requirement) =====" << std::endl;
	{
		std::cout << std::endl << "--- 1) Via WrongAnimal* (no virtual => static binding) ---" << std::endl;
		const WrongAnimal* wMeta = new WrongAnimal();
		const WrongAnimal* wCatViaBase = new WrongCat();

		std::cout << "wMeta type: " << wMeta->getType() << std::endl;
		std::cout << "wCatViaBase type: " << wCatViaBase->getType() << std::endl;

		std::cout << "wMeta->makeSound():       ";
		wMeta->makeSound();
		std::cout << "wCatViaBase->makeSound(): ";
		wCatViaBase->makeSound();
		std::cout << "  ^ This should print WrongAnimal's sound, NOT WrongCat's" << std::endl;
		std::cout << "    because WrongAnimal::makeSound is NOT virtual" << std::endl;

		delete wMeta;
		delete wCatViaBase;

		std::cout << std::endl << "--- 2) Via WrongCat directly (real type) ---" << std::endl;
		const WrongCat realWrongCat;
		std::cout << "realWrongCat.makeSound(): ";
		realWrongCat.makeSound();
		std::cout << "  ^ This DOES print WrongCat's sound (real type)" << std::endl;

		std::cout << std::endl << "--- 3) Compare with proper Animal/Cat (virtual => dynamic binding) ---" << std::endl;
		const Animal* aCatViaBase = new Cat();
		std::cout << "aCatViaBase->makeSound(): ";
		aCatViaBase->makeSound();
		std::cout << "  ^ Cat's sound is printed because Animal::makeSound IS virtual" << std::endl;
		delete aCatViaBase;
	}

	std::cout << std::endl;
	std::cout << "===== All tests completed =====" << std::endl;

	return 0;
}
