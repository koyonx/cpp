#include "Harl.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: ./harlFilter <DEBUG|INFO|WARNING|ERROR>" << std::endl;
        return 1;
    }

    Harl harl;
    harl.filterComplain(argv[1]);

    return 0;
}
