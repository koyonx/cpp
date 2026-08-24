#include "Intern.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include <iostream>

const Intern::FormEntry Intern::_forms[] = {
	{"shrubbery creation",  &Intern::createShrubberyCreationForm},
	{"robotomy request",    &Intern::createRobotomyRequestForm},
	{"presidential pardon", &Intern::createPresidentialPardonForm}
};

const std::size_t Intern::_formCount = sizeof(_forms) / sizeof(_forms[0]);

Intern::Intern() {}
Intern::Intern(const Intern&) {}
Intern& Intern::operator=(const Intern&) { return *this; }
Intern::~Intern() {}

AForm* Intern::createShrubberyCreationForm(const std::string& target) const {
	return new ShrubberyCreationForm(target);
}

AForm* Intern::createRobotomyRequestForm(const std::string& target) const {
	return new RobotomyRequestForm(target);
}

AForm* Intern::createPresidentialPardonForm(const std::string& target) const {
	return new PresidentialPardonForm(target);
}

AForm* Intern::makeForm(const std::string& name, const std::string& target) const {
	for (std::size_t i = 0; i < _formCount; ++i) {
		if (name == _forms[i].name) {
			std::cout << "Intern creates " << name << std::endl;
			return (this->*_forms[i].factory)(target);
		}
	}
	std::cerr << "Intern: unknown form name \"" << name << "\"" << std::endl;
	return NULL;
}
