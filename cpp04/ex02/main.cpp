#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "Brain.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"

/*
 * Animal is now abstract:
 *   class Animal { ... virtual void makeSound() const = 0; ... };
 *
 * The following lines must NOT compile. Uncomment them to verify:
 *
 *   Animal a;                // error: cannot declare variable 'a' of abstract type
 *   Animal* p = new Animal;  // error: cannot allocate an object of abstract type
 */

int main()
{
	std::cout << "===== Abstract class: cannot instantiate Animal =====" << std::endl;
	std::cout << "(Uncomment the lines in main.cpp to confirm compile error)" << std::endl;

	std::cout << std::endl;
	std::cout << "===== Polymorphism via Animal* (subject behaviour) =====" << std::endl;
	{
		const Animal* j = new Dog();
		const Animal* i = new Cat();

		std::cout << std::endl;
		std::cout << "Dog type: " << j->getType() << std::endl;
		std::cout << "Cat type: " << i->getType() << std::endl;
		j->makeSound();
		i->makeSound();

		delete j;
		delete i;
	}

	std::cout << std::endl;
	std::cout << "===== Array of half Dogs / half Cats =====" << std::endl;
	{
		const int size = 4;
		Animal* zoo[size];

		for (int k = 0; k < size / 2; k++)
			zoo[k] = new Dog();
		for (int k = size / 2; k < size; k++)
			zoo[k] = new Cat();

		std::cout << std::endl << "--- Sounds ---" << std::endl;
		for (int k = 0; k < size; k++)
			zoo[k]->makeSound();

		std::cout << std::endl << "--- Delete all ---" << std::endl;
		for (int k = 0; k < size; k++)
			delete zoo[k];
	}

	std::cout << std::endl;
	std::cout << "===== Deep Copy: basic / tmp scope pattern =====" << std::endl;
	{
		Dog basic;
		basic.setIdea(0, "deep copy works");

		{
			Dog tmp = basic;
			std::cout << "basic.brain addr = " << basic.getBrain()
				<< "  tmp.brain addr = " << tmp.getBrain() << std::endl;
			tmp.setIdea(0, "tmp owns this");
			std::cout << "basic.idea[0] = " << basic.getIdea(0) << std::endl;
			std::cout << "tmp.idea[0]   = " << tmp.getIdea(0) << std::endl;
		}
		std::cout << "After scope, basic.idea[0] = " << basic.getIdea(0) << std::endl;
		basic.makeSound();
	}

	std::cout << std::endl;
	std::cout << "===== Deep Copy: Cat assignment =====" << std::endl;
	{
		Cat a;
		Cat b;
		a.setIdea(5, "fish");
		b = a;
		std::cout << "Before mutate: b.idea[5] = " << b.getIdea(5) << std::endl;
		a.setIdea(5, "tuna");
		std::cout << "After  mutate: a.idea[5] = " << a.getIdea(5)
			<< "   b.idea[5] = " << b.getIdea(5) << " (independent)" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "===== WrongAnimal / WrongCat: no virtual => static binding =====" << std::endl;
	{
		const WrongAnimal* wMeta = new WrongAnimal();
		const WrongAnimal* wCatViaBase = new WrongCat();
		std::cout << "wMeta->makeSound():       ";
		wMeta->makeSound();
		std::cout << "wCatViaBase->makeSound(): ";
		wCatViaBase->makeSound();
		std::cout << "  ^ Prints WrongAnimal sound: makeSound is NOT virtual" << std::endl;
		delete wMeta;
		delete wCatViaBase;

		WrongCat realWC;
		std::cout << "realWC.makeSound(): ";
		realWC.makeSound();
	}

	std::cout << std::endl;
	std::cout << "===== Self-assignment safety =====" << std::endl;
	{
		Dog d;
		d.setIdea(7, "self");
		d = d;
		std::cout << "After d = d: d.idea[7] = " << d.getIdea(7) << std::endl;
	}

	std::cout << std::endl;
	std::cout << "===== All tests completed =====" << std::endl;
	return 0;
}
