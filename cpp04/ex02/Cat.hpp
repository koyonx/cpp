#ifndef CAT_HPP
#define CAT_HPP

#include "Animal.hpp"
#include "Brain.hpp"

class Cat : public Animal
{
private:
	Brain* brain;

public:
	Cat();
	Cat(const Cat& other);
	virtual ~Cat();
	Cat& operator=(const Cat& other);
	void makeSound() const;

	// Read-only access to the Brain (e.g. for address comparison in tests).
	const Brain* getBrain() const;

	// Convenience accessors that delegate to Brain without exposing it for write.
	void setIdea(int index, const std::string& idea);
	const std::string& getIdea(int index) const;
};

#endif
