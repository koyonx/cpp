#include "Contact.hpp"

Contact::Contact() {}

Contact::~Contact() {}

void Contact::setFirstName(const std::string& first_name) {
    this->_firstName = first_name;
}

void Contact::setLastName(const std::string& last_name) {
    this->_lastName = last_name;
}

void Contact::setNickname(const std::string& nickname) {
    this->_nickname = nickname;
}

void Contact::setPhoneNumber(const std::string& phone_number) {
    this->_phoneNumber = phone_number;
}

void Contact::setDarkestSecret(const std::string& darkest_secret) {
    this->_darkestSecret = darkest_secret;
}

std::string Contact::getFirstName() const {
    return this->_firstName;
}

std::string Contact::getLastName() const {
    return this->_lastName;
}

std::string Contact::getNickname() const {
    return this->_nickname;
}

std::string Contact::getPhoneNumber() const {
    return this->_phoneNumber;
}

std::string Contact::getDarkestSecret() const {
    return this->_darkestSecret;
}
