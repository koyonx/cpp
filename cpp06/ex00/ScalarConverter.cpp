#include "ScalarConverter.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <climits>
#include <limits>
#include <cerrno>
#include <cmath>

namespace {

const double POS_INF = std::numeric_limits<double>::infinity();
const double NEG_INF = -std::numeric_limits<double>::infinity();

static bool isNan(double d) { return d != d; }
static bool isPosInf(double d) { return d == POS_INF; }
static bool isNegInf(double d) { return d == NEG_INF; }

// pseudo-literal (nan/inf) の判定
static bool isPseudoLiteral(const std::string& s, double& value) {
	if (s == "nan" || s == "nanf") {
		value = std::numeric_limits<double>::quiet_NaN();
		return true;
	}
	if (s == "+inf" || s == "+inff" || s == "inf" || s == "inff") {
		value = POS_INF;
		return true;
	}
	if (s == "-inf" || s == "-inff") {
		value = NEG_INF;
		return true;
	}
	return false;
}

// 単一 char (数字を除く) を char リテラルと判定
static bool isCharLiteral(const std::string& s) {
	return s.length() == 1 && !std::isdigit(static_cast<unsigned char>(s[0]));
}

// int/float/double 数値リテラルとしてパース、成功なら value に入れて true
static bool parseNumeric(const std::string& s, double& value) {
	if (s.empty()) return false;
	const char* p = s.c_str();
	char* endp;
	errno = 0;
	double d = std::strtod(p, &endp);
	if (endp == p) return false;
	if (errno == ERANGE) return false;
	std::string rest(endp);
	// 空: int または double。 "f" のみ: float。それ以外: 無効
	if (rest.empty() || rest == "f") {
		value = d;
		return true;
	}
	return false;
}

// 表示
static void printChar(double d) {
	std::cout << "char: ";
	if (isNan(d) || isPosInf(d) || isNegInf(d)) {
		std::cout << "impossible" << std::endl;
		return;
	}
	if (d < 0.0 || d > 127.0) {
		std::cout << "impossible" << std::endl;
		return;
	}
	char c = static_cast<char>(d);
	if (!std::isprint(static_cast<unsigned char>(c))) {
		std::cout << "Non displayable" << std::endl;
	} else {
		std::cout << "'" << c << "'" << std::endl;
	}
}

static void printInt(double d) {
	std::cout << "int: ";
	if (isNan(d) || isPosInf(d) || isNegInf(d)) {
		std::cout << "impossible" << std::endl;
		return;
	}
	if (d < static_cast<double>(std::numeric_limits<int>::min()) ||
	    d > static_cast<double>(std::numeric_limits<int>::max())) {
		std::cout << "impossible" << std::endl;
		return;
	}
	std::cout << static_cast<int>(d) << std::endl;
}

static void printFloat(double d) {
	std::cout << "float: ";
	if (isNan(d))    { std::cout << "nanf" << std::endl; return; }
	if (isPosInf(d)) { std::cout << "+inff" << std::endl; return; }
	if (isNegInf(d)) { std::cout << "-inff" << std::endl; return; }
	float f = static_cast<float>(d);
	if (f == std::numeric_limits<float>::infinity())  { std::cout << "+inff" << std::endl; return; }
	if (f == -std::numeric_limits<float>::infinity()) { std::cout << "-inff" << std::endl; return; }
	std::cout << std::fixed << std::setprecision(1) << f << "f" << std::endl;
	std::cout.unsetf(std::ios::fixed);
	std::cout << std::setprecision(6);
}

static void printDouble(double d) {
	std::cout << "double: ";
	if (isNan(d))    { std::cout << "nan" << std::endl; return; }
	if (isPosInf(d)) { std::cout << "+inf" << std::endl; return; }
	if (isNegInf(d)) { std::cout << "-inf" << std::endl; return; }
	std::cout << std::fixed << std::setprecision(1) << d << std::endl;
	std::cout.unsetf(std::ios::fixed);
	std::cout << std::setprecision(6);
}

}  // anonymous namespace

void ScalarConverter::convert(const std::string& s) {
	double value = 0.0;

	if (isPseudoLiteral(s, value)) {
		printChar(value);
		printInt(value);
		printFloat(value);
		printDouble(value);
		return;
	}

	if (isCharLiteral(s)) {
		value = static_cast<double>(static_cast<unsigned char>(s[0]));
		printChar(value);
		printInt(value);
		printFloat(value);
		printDouble(value);
		return;
	}

	if (parseNumeric(s, value)) {
		printChar(value);
		printInt(value);
		printFloat(value);
		printDouble(value);
		return;
	}

	std::cout << "char: impossible" << std::endl;
	std::cout << "int: impossible" << std::endl;
	std::cout << "float: impossible" << std::endl;
	std::cout << "double: impossible" << std::endl;
}
