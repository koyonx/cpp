#include "ScalarConverter.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cctype>
#include <limits>
#include <cerrno>
#include <cmath>

ScalarConverter::ScalarConverter() {}
ScalarConverter::ScalarConverter(const ScalarConverter &other) { (void)other; }
ScalarConverter &ScalarConverter::operator=(const ScalarConverter &other) { (void)other; return (*this); }
ScalarConverter::~ScalarConverter() {}

enum e_type { CHAR, INT, FLOAT, DOUBLE, PSEUDO, INVALID };

// === 型検出 ===
// namespace {} の代わりに static を付与して内部リンケージとする

static bool isPseudoLiteral(const std::string& s) {
	return s == "nan" || s == "nanf"
		|| s == "+inf" || s == "+inff" || s == "inf" || s == "inff"
		|| s == "-inf" || s == "-inff";
}

static bool isCharLiteral(const std::string& s) {
	return s.length() == 1 && !std::isdigit(static_cast<unsigned char>(s[0]));
}

static bool isIntLiteral(const std::string& s) {
	if (s.empty()) return false;
	size_t i = 0;
	if (s[i] == '+' || s[i] == '-') i++;
	if (i == s.length()) return false;
	while (i < s.length()) {
		if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
		i++;
	}
	return true;
}

static bool isFloatLiteral(const std::string& s) {
	if (s.empty() || s[s.length() - 1] != 'f') return false;
	std::string body = s.substr(0, s.length() - 1);
	if (body.empty()) return false;
	const char* p = body.c_str();
	char* endp;
	errno = 0;
	std::strtod(p, &endp);
	return (endp != p && *endp == '\0');
}

static bool isDoubleLiteral(const std::string& s) {
	if (s.empty()) return false;
	const char* p = s.c_str();
	char* endp;
	errno = 0;
	std::strtod(p, &endp);
	if (endp == p || *endp != '\0') return false;
	return s.find('.') != std::string::npos
		|| s.find('e') != std::string::npos
		|| s.find('E') != std::string::npos;
}

static e_type detectType(const std::string& s) {
	if (isPseudoLiteral(s)) return PSEUDO;
	if (isCharLiteral(s))   return CHAR;
	if (isIntLiteral(s))    return INT;
	if (isFloatLiteral(s))  return FLOAT;
	if (isDoubleLiteral(s)) return DOUBLE;
	return INVALID;
}

// === 表示ヘルパー ===

static void printCharValue(char c) {
	if (std::isprint(static_cast<unsigned char>(c)))
		std::cout << "char: '" << c << "'" << std::endl;
	else
		std::cout << "char: Non displayable" << std::endl;
}

static void printCharImpossible(void) {
	std::cout << "char: impossible" << std::endl;
}

static void printIntImpossible(void) {
	std::cout << "int: impossible" << std::endl;
}

static void printFloatValue(float f) {
	std::cout << "float: ";
	if (f != f) { std::cout << "nanf" << std::endl; return; }
	if (f == std::numeric_limits<float>::infinity()) { std::cout << "+inff" << std::endl; return; }
	if (f == -std::numeric_limits<float>::infinity()) { std::cout << "-inff" << std::endl; return; }
	if (f == std::floor(f))
		std::cout << std::fixed << std::setprecision(1) << f << "f" << std::endl;
	else
		std::cout << f << "f" << std::endl;
	std::cout.unsetf(std::ios::fixed);
	std::cout << std::setprecision(6);
}

static void printDoubleValue(double d) {
	std::cout << "double: ";
	if (d != d) { std::cout << "nan" << std::endl; return; }
	if (d == std::numeric_limits<double>::infinity()) { std::cout << "+inf" << std::endl; return; }
	if (d == -std::numeric_limits<double>::infinity()) { std::cout << "-inf" << std::endl; return; }
	if (d == std::floor(d))
		std::cout << std::fixed << std::setprecision(1) << d << std::endl;
	else
		std::cout << d << std::endl;
	std::cout.unsetf(std::ios::fixed);
	std::cout << std::setprecision(6);
}

// === 型別変換 ===

static void convertPseudo(const std::string& s) {
	printCharImpossible();
	printIntImpossible();
	if (s == "nan" || s == "nanf") {
		std::cout << "float: nanf" << std::endl;
		std::cout << "double: nan" << std::endl;
	} else if (s[0] == '-') {
		std::cout << "float: -inff" << std::endl;
		std::cout << "double: -inf" << std::endl;
	} else {
		std::cout << "float: +inff" << std::endl;
		std::cout << "double: +inf" << std::endl;
	}
}

static void convertChar(char c) {
	printCharValue(c);
	std::cout << "int: " << static_cast<int>(c) << std::endl;
	printFloatValue(static_cast<float>(c));
	printDoubleValue(static_cast<double>(c));
}

static void convertInt(const std::string& s) {
	errno = 0;
	long l = std::strtol(s.c_str(), NULL, 10);
	if (errno == ERANGE
		|| l > std::numeric_limits<int>::max()
		|| l < std::numeric_limits<int>::min()) {
		printCharImpossible();
		printIntImpossible();
		errno = 0;
		double d = std::strtod(s.c_str(), NULL);
		printFloatValue(static_cast<float>(d));
		printDoubleValue(d);
		return;
	}
	int n = static_cast<int>(l);
	if (n < 0 || n > 127)
		printCharImpossible();
	else
		printCharValue(static_cast<char>(n));
	std::cout << "int: " << n << std::endl;
	printFloatValue(static_cast<float>(n));
	printDoubleValue(static_cast<double>(n));
}

static void convertFloat(const std::string& s) {
	std::string body = s.substr(0, s.length() - 1);
	errno = 0;
	float f = static_cast<float>(std::strtod(body.c_str(), NULL));
	if (f != f
		|| f == std::numeric_limits<float>::infinity()
		|| f == -std::numeric_limits<float>::infinity()) {
		printCharImpossible();
		printIntImpossible();
	} else {
		if (f < 0 || f > 127)
			printCharImpossible();
		else
			printCharValue(static_cast<char>(f));
		if (static_cast<double>(f) > static_cast<double>(std::numeric_limits<int>::max())
			|| static_cast<double>(f) < static_cast<double>(std::numeric_limits<int>::min()))
			printIntImpossible();
		else
			std::cout << "int: " << static_cast<int>(f) << std::endl;
	}
	printFloatValue(f);
	printDoubleValue(static_cast<double>(f));
}

static void convertDouble(const std::string& s) {
	errno = 0;
	double d = std::strtod(s.c_str(), NULL);
	if (d != d
		|| d == std::numeric_limits<double>::infinity()
		|| d == -std::numeric_limits<double>::infinity()) {
		printCharImpossible();
		printIntImpossible();
	} else {
		if (d < 0 || d > 127)
			printCharImpossible();
		else
			printCharValue(static_cast<char>(d));
		if (d > static_cast<double>(std::numeric_limits<int>::max())
			|| d < static_cast<double>(std::numeric_limits<int>::min()))
			printIntImpossible();
		else
			std::cout << "int: " << static_cast<int>(d) << std::endl;
	}
	printFloatValue(static_cast<float>(d));
	printDoubleValue(d);
}

void ScalarConverter::convert(const std::string& s) {
	e_type type = detectType(s);
	switch (type) {
		case PSEUDO:  convertPseudo(s);  break;
		case CHAR:    convertChar(s[0]); break;
		case INT:     convertInt(s);     break;
		case FLOAT:   convertFloat(s);   break;
		case DOUBLE:  convertDouble(s);  break;
		case INVALID:
			printCharImpossible();
			printIntImpossible();
			std::cout << "float: impossible" << std::endl;
			std::cout << "double: impossible" << std::endl;
			break;
	}
}
