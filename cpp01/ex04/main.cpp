#include "sed.hpp"

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cout << "Error: Invalid number of arguments." << std::endl;
		return 1;
	}
	Sed sed(argv[1], argv[2], argv[3]);
	if (!sed.isFileOpen())
	{
		std::cout << "Error: Could not open file." << std::endl;
		return 1;
	}
	sed.replace();
	sed.writeToFile();
	return 0;
}
