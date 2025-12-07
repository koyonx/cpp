#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include "Contact.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>
#include <limits>

class PhoneBook {
public:
    PhoneBook();
    ~PhoneBook();

    void addContact();
    void searchContact();
    void displayContacts();

private:
    Contact _contacts[8];
    int _contactCount;
};

#endif
