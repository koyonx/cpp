#include "sed.hpp"

Sed::Sed(const std::string &filename, const std::string &s1, const std::string &s2)
	: filename(filename), s1(s1), s2(s2), fileOpen(false)
{
	inputFile.open(filename);
	if (inputFile.is_open())
	{
		fileOpen = true;
		char c;
		while (inputFile.get(c))
		{
			fileContent += c;
		}
		inputFile.close();
	}
}

Sed::~Sed()
{
	if (inputFile.is_open())
	{
		inputFile.close();
	}
}
bool Sed::isFileOpen() const
{
	return fileOpen;
}

void Sed::replace()
{
	if (s1.empty())
		return;
	size_t pos = 0;
	while ((pos = fileContent.find(s1, pos)) != std::string::npos)
	{
		fileContent.erase(pos, s1.length());
		fileContent.insert(pos, s2);
		pos += s2.length();
	}
}

void Sed::writeToFile() const
{
	std::ofstream outputFile(filename + ".replace");
	if (outputFile.is_open())
	{
		outputFile << fileContent;
		outputFile.close();
	}
}
