#include "PhoneBook.hpp"
#include <iostream>
#include <string>

int main() {
    PhoneBook myPhoneBook;
    std::string command;

    while (true) {
        std::cout << "Enter command (ADD, SEARCH, EXIT): ";
        if (!std::getline(std::cin, command)) {
            std::cout << "\nExiting phonebook." << std::endl;
            break;
        }

        if (command == "ADD") {
            myPhoneBook.addContact();
        } else if (command == "SEARCH") {
            myPhoneBook.searchContact();
        } else if (command == "EXIT") {
            break;
        } else {
            std::cout << "Invalid command." << std::endl;
        }
    }

    return 0;
}
