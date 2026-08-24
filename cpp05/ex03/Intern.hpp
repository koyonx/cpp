#ifndef INTERN_HPP
#define INTERN_HPP

#include <string>
#include <cstddef>

class AForm;

class Intern {
private:
	AForm*	createShrubberyCreationForm(const std::string& target) const;
	AForm*	createRobotomyRequestForm(const std::string& target) const;
	AForm*	createPresidentialPardonForm(const std::string& target) const;

	typedef AForm* (Intern::*FormFactory)(const std::string& target) const;

	struct FormEntry {
		const char*	name;
		FormFactory	factory;
	};

	static const FormEntry		_forms[];
	static const std::size_t	_formCount;

public:
	Intern();
	Intern(const Intern& other);
	Intern& operator=(const Intern& other);
	~Intern();

	AForm*	makeForm(const std::string& name, const std::string& target) const;
};

#endif
