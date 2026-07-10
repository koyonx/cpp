#include "Intern.hpp"
#include "AForm.hpp"
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"
#include <iostream>
#include <cstddef>

Intern::Intern() {}
Intern::Intern(const Intern&) {}
Intern& Intern::operator=(const Intern&) { return *this; }
Intern::~Intern() {}

namespace {
	template <typename T>
	AForm* create(const std::string& target) { return new T(target); }

	struct FormEntry {
		const char*	name;
		AForm*		(*factory)(const std::string&);
	};

	const FormEntry kFormTable[] = {
		{"shrubbery creation",  &create<ShrubberyCreationForm>},
		{"robotomy request",    &create<RobotomyRequestForm>},
		{"presidential pardon", &create<PresidentialPardonForm>}
	};

	const std::size_t kFormTableSize = sizeof(kFormTable) / sizeof(kFormTable[0]);
}

AForm* Intern::makeForm(const std::string& name, const std::string& target) const {
	for (std::size_t i = 0; i < kFormTableSize; ++i) {
		if (name == kFormTable[i].name) {
			std::cout << "Intern creates " << name << std::endl;
			return kFormTable[i].factory(target);
		}
	}
	std::cerr << "Intern: unknown form name \"" << name << "\"" << std::endl;
	return NULL;
}
