#include "Animal.hpp"
#include "Dog.hpp"
#include "Cat.hpp"

int main()
{
	// Animal animal; // This would cause a compile error (abstract class)
	// Animal* animal = new Animal(); // This would also cause a compile error

	std::cout << "===== Abstract Class Test =====" << std::endl;
	const Animal* j = new Dog();
	const Animal* i = new Cat();

	std::cout << std::endl;
	std::cout << "Dog type: " << j->getType() << std::endl;
	std::cout << "Cat type: " << i->getType() << std::endl;

	std::cout << std::endl;
	j->makeSound();
	i->makeSound();

	std::cout << std::endl;
	delete j;
	delete i;

	std::cout << std::endl;
	std::cout << "===== Deep Copy Test =====" << std::endl;
	{
		Dog original;
		Dog copy(original);

		std::cout << std::endl;
		original.makeSound();
		copy.makeSound();
	}

	std::cout << std::endl;
	std::cout << "===== All tests completed =====" << std::endl;

	return 0;
}
