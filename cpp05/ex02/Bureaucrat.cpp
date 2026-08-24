#include "Bureaucrat.hpp"
#include "AForm.hpp"

Bureaucrat::Bureaucrat() : _name("default"), _grade(150) {}

Bureaucrat::Bureaucrat(const std::string& name, int grade)
	: _name(name), _grade(grade) {
	if (grade < 1) throw GradeTooHighException();
	if (grade > 150) throw GradeTooLowException();
}

Bureaucrat::Bureaucrat(const Bureaucrat& other) : _name(other._name), _grade(other._grade) {}

Bureaucrat& Bureaucrat::operator=(const Bureaucrat& other) {
	if (this != &other) _grade = other._grade;
	return *this;
}

Bureaucrat::~Bureaucrat() {}

const std::string& Bureaucrat::getName() const { return _name; }
int Bureaucrat::getGrade() const { return _grade; }

void Bureaucrat::incrementGrade() {
	if (_grade - 1 < 1) throw GradeTooHighException();
	_grade -= 1;
}

void Bureaucrat::decrementGrade() {
	if (_grade + 1 > 150) throw GradeTooLowException();
	_grade += 1;
}

void Bureaucrat::signForm(AForm& form) const {
	try {
		form.beSigned(*this);
		std::cout << _name << " signed " << form.getName() << std::endl;
	} catch (std::exception& e) {
		std::cout << _name << " couldn't sign " << form.getName()
			<< " because " << e.what() << "." << std::endl;
	}
}

void Bureaucrat::executeForm(const AForm& form) const {
	try {
		form.execute(*this);
		std::cout << _name << " executed " << form.getName() << std::endl;
	} catch (std::exception& e) {
		std::cout << _name << " couldn't execute " << form.getName()
			<< " because " << e.what() << "." << std::endl;
	}
}

const char* Bureaucrat::GradeTooHighException::what() const throw() {
	return "the grade is too high";
}

const char* Bureaucrat::GradeTooLowException::what() const throw() {
	return "the grade is too low";
}

std::ostream& operator<<(std::ostream& os, const Bureaucrat& b) {
	os << b.getName() << ", bureaucrat grade " << b.getGrade() << ".";
	return os;
}
