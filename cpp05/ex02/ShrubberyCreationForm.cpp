#include "ShrubberyCreationForm.hpp"
#include <fstream>

ShrubberyCreationForm::ShrubberyCreationForm()
	: AForm("shrubbery", 145, 137), _target("default") {}

ShrubberyCreationForm::ShrubberyCreationForm(const std::string& target)
	: AForm("shrubbery", 145, 137), _target(target) {}

ShrubberyCreationForm::ShrubberyCreationForm(const ShrubberyCreationForm& other)
	: AForm(other), _target(other._target) {}

ShrubberyCreationForm& ShrubberyCreationForm::operator=(const ShrubberyCreationForm& other) {
	if (this != &other) {
		AForm::operator=(other);
		_target = other._target;
	}
	return *this;
}

ShrubberyCreationForm::~ShrubberyCreationForm() {}

const std::string& ShrubberyCreationForm::getTarget() const { return _target; }

void ShrubberyCreationForm::action(const Bureaucrat& executor) const {
	(void)executor;
	const std::string filename = _target + "_shrubbery";
	std::ofstream ofs(filename.c_str());
	if (!ofs) {
		std::cerr << "ShrubberyCreationForm: cannot open " << filename << std::endl;
		return;
	}
	ofs << "       #\n"
	       "      ###\n"
	       "     #####\n"
	       "    #######\n"
	       "   #########\n"
	       "  ###########\n"
	       " #############\n"
	       "###############\n"
	       "       |\n"
	       "       |\n"
	       "       |\n"
	       "\n"
	       "       *\n"
	       "      ***\n"
	       "     *****\n"
	       "    *******\n"
	       "   *********\n"
	       "  ***********\n"
	       "       |\n"
	       "       |\n";
}
