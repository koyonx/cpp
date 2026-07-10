#include "AForm.hpp"

AForm::AForm()
	: _name("default_aform"), _signed(false), _gradeToSign(150), _gradeToExecute(150) {}

AForm::AForm(const std::string& name, int gradeToSign, int gradeToExecute)
	: _name(name), _signed(false), _gradeToSign(gradeToSign), _gradeToExecute(gradeToExecute) {
	if (gradeToSign < 1 || gradeToExecute < 1) throw GradeTooHighException();
	if (gradeToSign > 150 || gradeToExecute > 150) throw GradeTooLowException();
}

AForm::AForm(const AForm& other)
	: _name(other._name), _signed(other._signed),
	  _gradeToSign(other._gradeToSign), _gradeToExecute(other._gradeToExecute) {}

AForm& AForm::operator=(const AForm& other) {
	if (this != &other) _signed = other._signed;
	return *this;
}

AForm::~AForm() {}

const std::string& AForm::getName() const { return _name; }
bool AForm::getSigned() const { return _signed; }
int AForm::getGradeToSign() const { return _gradeToSign; }
int AForm::getGradeToExecute() const { return _gradeToExecute; }

void AForm::beSigned(const Bureaucrat& b) {
	if (b.getGrade() > _gradeToSign) throw GradeTooLowException();
	_signed = true;
}

void AForm::execute(const Bureaucrat& executor) const {
	if (!_signed) throw FormNotSignedException();
	if (executor.getGrade() > _gradeToExecute) throw GradeTooLowException();
	action(executor);
}

const char* AForm::GradeTooHighException::what() const throw() {
	return "AForm: grade too high (must be >= 1)";
}
const char* AForm::GradeTooLowException::what() const throw() {
	return "AForm: grade too low";
}
const char* AForm::FormNotSignedException::what() const throw() {
	return "AForm: form is not signed";
}

std::ostream& operator<<(std::ostream& os, const AForm& f) {
	os << "AForm \"" << f.getName() << "\" [signed=" << (f.getSigned() ? "yes" : "no")
	   << ", gradeToSign=" << f.getGradeToSign()
	   << ", gradeToExecute=" << f.getGradeToExecute() << "]";
	return os;
}
