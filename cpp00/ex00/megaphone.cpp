#include <iostream>
#include <string>

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "* LOUD AND UNBEARABLE FEEDBACK NOISE *" << std::endl;
    } else {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            for (std::string::iterator it = arg.begin(); it != arg.end(); ++it) {
                *it = toupper(*it);
            }
            std::cout << arg;
        }
        std::cout << std::endl;
    }
    return 0;
}
