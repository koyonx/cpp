#ifndef DOG_HPP
#define DOG_HPP

#include "Animal.hpp"
#include "Brain.hpp"

class Dog : public Animal
{
private:
	Brain* brain;

public:
	Dog();
	Dog(const Dog& other);
	virtual ~Dog();
	Dog& operator=(const Dog& other);
	void makeSound() const;

	const Brain* getBrain() const;
	void setIdea(int index, const std::string& idea);
	const std::string& getIdea(int index) const;
};

#endif
