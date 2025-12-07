#include "PhoneBook.hpp"
#include <cstdlib>

PhoneBook::PhoneBook() : _contactCount(0) {}

PhoneBook::~PhoneBook() {}

static bool is_valid_phone_number(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    for (size_t i = 0; i < str.length(); ++i) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

static bool is_whitespace_only(const std::string& str) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (!isspace(str[i])) {
            return false;
        }
    }
    return true;
}

static std::string get_input(const std::string& prompt) {
    std::string input;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);

        if (std::cin.eof()) {
            std::cin.clear();
            return "";
        }

        if (!input.empty() && !is_whitespace_only(input)) {
            return input;
        }
        std::cout << "Field cannot be empty. Please try again." << std::endl;
    }
}

void PhoneBook::addContact() {
    std::string input;
    Contact newContact;

    input = get_input("Enter first name: ");
    if (input.empty()) return;
    newContact.setFirstName(input);

    input = get_input("Enter last name: ");
    if (input.empty()) return;
    newContact.setLastName(input);

    input = get_input("Enter nickname: ");
    if (input.empty()) return;
    newContact.setNickname(input);

    while (true) {
        input = get_input("Enter phone number (digits only): ");
        if (input.empty()) return;
        if (is_valid_phone_number(input)) {
            newContact.setPhoneNumber(input);
            break;
        }
        std::cout << "Invalid phone number. It must contain only digits. Please try again." << std::endl;
    }

    input = get_input("Enter darkest secret: ");
    if (input.empty()) return;
    newContact.setDarkestSecret(input);

    if (_contactCount < 8) {
        this->_contacts[_contactCount] = newContact;
        _contactCount++;
    } else {
        // 最古(index 0)を押し出し、全要素を前にシフト
        for (int i = 0; i < 7; ++i) {
            this->_contacts[i] = this->_contacts[i + 1];
        }
        // 最新を末尾(index 7)に追加
        this->_contacts[7] = newContact;
    }
    std::cout << "Contact added successfully!" << std::endl;
}

void PhoneBook::searchContact() {
    if (_contactCount == 0) {
        std::cout << "Phonebook is empty." << std::endl;
        return;
    }

    displayContacts();

    std::string input;
    int index = -1;
    bool valid_input = false;

    while (!valid_input) {
        std::cout << "Enter the index of the contact to display: ";
        std::getline(std::cin, input);

        if (std::cin.eof()) {
            return;
        }

        bool is_numeric = !input.empty();
        for (size_t i = 0; i < input.length() && is_numeric; ++i) {
            if (!isdigit(input[i])) {
                is_numeric = false;
            }
        }
        if (!is_numeric) {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }
        index = std::atoi(input.c_str());
        if (index >= 0 && index < _contactCount) {
            valid_input = true;
        } else {
            std::cout << "Invalid index. Please enter a valid index from the list." << std::endl;
        }
    }

    std::cout << "First Name: " << _contacts[index].getFirstName() << std::endl;
    std::cout << "Last Name: " << _contacts[index].getLastName() << std::endl;
    std::cout << "Nickname: " << _contacts[index].getNickname() << std::endl;
    std::cout << "Phone Number: " << _contacts[index].getPhoneNumber() << std::endl;
    std::cout << "Darkest Secret: " << _contacts[index].getDarkestSecret() << std::endl;
}

void PhoneBook::displayContacts() {
    std::cout << std::setw(10) << std::right << "Index" << "|";
    std::cout << std::setw(10) << std::right << "First Name" << "|";
    std::cout << std::setw(10) << std::right << "Last Name" << "|";
    std::cout << std::setw(10) << std::right << "Nickname" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    for (int i = 0; i < _contactCount; ++i) {
        std::cout << std::setw(10) << std::right << i << "|";

        std::string first_name = _contacts[i].getFirstName();
        if (first_name.length() > 10) {
            first_name = first_name.substr(0, 9) + ".";
        }
        std::cout << std::setw(10) << std::right << first_name << "|";

        std::string last_name = _contacts[i].getLastName();
        if (last_name.length() > 10) {
            last_name = last_name.substr(0, 9) + ".";
        }
        std::cout << std::setw(10) << std::right << last_name << "|";

        std::string nickname = _contacts[i].getNickname();
        if (nickname.length() > 10) {
            nickname = nickname.substr(0, 9) + ".";
        }
        std::cout << std::setw(10) << std::right << nickname << std::endl;
    }
}
