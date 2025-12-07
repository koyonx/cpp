#ifndef SED_HPP
#define SED_HPP

#include <iostream>
#include <fstream>
#include <string>

class Sed
{
  private:
	std::string filename;
	std::string s1;
	std::string s2;
	std::ifstream inputFile;
	std::string fileContent;
	bool fileOpen;
  public:
	Sed(const std::string &filename, const std::string &s1, const std::string &s2);
	~Sed();
	bool isFileOpen() const;
	void replace();
	void writeToFile() const;
};

#endif
