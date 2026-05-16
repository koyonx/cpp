#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"
#include "Brain.hpp"
#include "WrongAnimal.hpp"
#include "WrongCat.hpp"

int main()
{
	std::cout << "===== Basic Test (subject sample) =====" << std::endl;
	{
		const Animal* j = new Dog();
		const Animal* i = new Cat();

		delete j;
		delete i;
	}

	std::cout << std::endl;
	std::cout << "===== Array of half Dogs / half Cats (subject requirement) =====" << std::endl;
	{
		const int size = 6;
		Animal* animals[size];

		std::cout << "--- Construction ---" << std::endl;
		for (int k = 0; k < size / 2; k++)
			animals[k] = new Dog();
		for (int k = size / 2; k < size; k++)
			animals[k] = new Cat();

		std::cout << std::endl << "--- Make sounds ---" << std::endl;
		for (int k = 0; k < size; k++)
		{
			std::cout << "animals[" << k << "] (" << animals[k]->getType() << "): ";
			animals[k]->makeSound();
		}

		std::cout << std::endl << "--- Delete all as Animal* ---" << std::endl;
		for (int k = 0; k < size; k++)
			delete animals[k];
	}

	std::cout << std::endl;
	std::cout << "===== Deep Copy: Dog basic; { Dog tmp = basic; } pattern =====" << std::endl;
	{
		Dog basic;
		basic.setIdea(0, "I love bones");

		std::cout << std::endl << "--- Enter inner scope, copy via copy constructor ---" << std::endl;
		{
			Dog tmp = basic;
			std::cout << "basic.brain addr = " << basic.getBrain() << std::endl;
			std::cout << "tmp.brain   addr = " << tmp.getBrain() << std::endl;
			std::cout << "  ^ Addresses must differ (deep copy)" << std::endl;
			std::cout << "basic.idea[0] = \"" << basic.getIdea(0) << "\"" << std::endl;
			std::cout << "tmp.idea[0]   = \"" << tmp.getIdea(0) << "\"" << std::endl;

			tmp.setIdea(0, "I love cats actually");
			std::cout << "After tmp.setIdea(0, \"I love cats actually\"):" << std::endl;
			std::cout << "basic.idea[0] = \"" << basic.getIdea(0) << "\"  (unchanged)" << std::endl;
			std::cout << "tmp.idea[0]   = \"" << tmp.getIdea(0) << "\"" << std::endl;
			std::cout << "--- tmp goes out of scope ---" << std::endl;
		}
		std::cout << "After inner scope, basic still works:" << std::endl;
		basic.makeSound();
		std::cout << "basic.idea[0] = \"" << basic.getIdea(0) << "\"" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "===== Deep Copy: assignment operator =====" << std::endl;
	{
		Cat a;
		Cat b;

		a.setIdea(0, "fish");
		b.setIdea(0, "milk");
		std::cout << "Before: a.idea[0]=\"" << a.getIdea(0)
			<< "\"  b.idea[0]=\"" << b.getIdea(0) << "\"" << std::endl;
		std::cout << "Brain addresses: a=" << a.getBrain() << "  b=" << b.getBrain() << std::endl;

		b = a;

		std::cout << "After b = a: a.idea[0]=\"" << a.getIdea(0)
			<< "\"  b.idea[0]=\"" << b.getIdea(0) << "\"" << std::endl;
		std::cout << "Brain addresses: a=" << a.getBrain() << "  b=" << b.getBrain() << std::endl;
		std::cout << "  ^ b.brain pointer is the same as before (reused via *brain = *other.brain)" << std::endl;

		a.setIdea(0, "tuna");
		std::cout << "After a.setIdea(0, \"tuna\"): b.idea[0]=\"" << b.getIdea(0)
			<< "\" (unchanged: deep copy)" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "===== WrongAnimal / WrongCat carried over from ex00 =====" << std::endl;
	{
		const WrongAnimal* wMeta = new WrongAnimal();
		const WrongAnimal* wCatViaBase = new WrongCat();
		std::cout << "wCatViaBase->makeSound(): ";
		wCatViaBase->makeSound();
		std::cout << "  ^ WrongAnimal's sound (no virtual)" << std::endl;
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
		d.setIdea(0, "self");
		d = d;
		std::cout << "After d = d: d.idea[0] = \"" << d.getIdea(0)
			<< "\" (should remain \"self\")" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "===== All tests completed =====" << std::endl;
	return 0;
}
