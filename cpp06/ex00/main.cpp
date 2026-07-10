#include "ScalarConverter.hpp"
#include <iostream>
#include <sstream>
#include <string>

static int g_pass = 0;
static int g_fail = 0;

static void section(const std::string& t) {
	std::cout << "\n=== " << t << " ===" << std::endl;
}
static void expect(bool cond, const std::string& what) {
	if (cond) { ++g_pass; std::cout << "  [PASS] " << what << std::endl; }
	else      { ++g_fail; std::cout << "  [FAIL] " << what << std::endl; }
}

// convert() の stdout を捕捉して返す
static std::string captureConvert(const std::string& input) {
	std::ostringstream oss;
	std::streambuf* saved = std::cout.rdbuf(oss.rdbuf());
	ScalarConverter::convert(input);
	std::cout.rdbuf(saved);
	return oss.str();
}

static bool contains(const std::string& hay, const std::string& needle) {
	return hay.find(needle) != std::string::npos;
}

static void runTests() {
	// === 1. PDF 例: "0" ===
	section("1. PDF example: 0");
	{
		std::string out = captureConvert("0");
		expect(contains(out, "char: Non displayable"), "char: Non displayable");
		expect(contains(out, "int: 0"), "int: 0");
		expect(contains(out, "float: 0.0f"), "float: 0.0f");
		expect(contains(out, "double: 0.0"), "double: 0.0");
	}

	// === 2. PDF 例: "nan" ===
	section("2. PDF example: nan");
	{
		std::string out = captureConvert("nan");
		expect(contains(out, "char: impossible"), "char: impossible");
		expect(contains(out, "int: impossible"), "int: impossible");
		expect(contains(out, "float: nanf"), "float: nanf");
		expect(contains(out, "double: nan"), "double: nan");
	}

	// === 3. PDF 例: "42.0f" ===
	section("3. PDF example: 42.0f");
	{
		std::string out = captureConvert("42.0f");
		expect(contains(out, "char: '*'"), "char: '*'");
		expect(contains(out, "int: 42"), "int: 42");
		expect(contains(out, "float: 42.0f"), "float: 42.0f");
		expect(contains(out, "double: 42.0"), "double: 42.0");
	}

	// === 4. char リテラル: 'a', 'A', '~' 等 ===
	section("4. char literals");
	{
		std::string out = captureConvert("a");
		expect(contains(out, "char: 'a'"), "'a' char");
		expect(contains(out, "int: 97"), "'a' int");
		expect(contains(out, "float: 97.0f"), "'a' float");
	}
	{
		std::string out = captureConvert("A");
		expect(contains(out, "char: 'A'"), "'A' char");
		expect(contains(out, "int: 65"), "'A' int");
	}
	{
		std::string out = captureConvert("~");
		expect(contains(out, "char: '~'"), "'~' char");
		expect(contains(out, "int: 126"), "'~' int");
	}
	{
		std::string out = captureConvert(" ");
		expect(contains(out, "char: ' '"), "space char");
		expect(contains(out, "int: 32"), "space int");
	}
	{
		std::string out = captureConvert("!");
		expect(contains(out, "char: '!'"), "'!' char");
	}
	{
		std::string out = captureConvert("*");
		expect(contains(out, "char: '*'"), "'*' char");
		expect(contains(out, "int: 42"), "'*' int");
	}

	// === 5. int リテラル: 正/負/境界 ===
	section("5. int literals");
	{
		std::string out = captureConvert("42");
		expect(contains(out, "int: 42"), "42 int");
		expect(contains(out, "char: '*'"), "42 char");
		expect(contains(out, "float: 42.0f"), "42 float");
		expect(contains(out, "double: 42.0"), "42 double");
	}
	{
		std::string out = captureConvert("-42");
		expect(contains(out, "int: -42"), "-42 int");
		expect(contains(out, "char: impossible"), "-42 char impossible");
		expect(contains(out, "float: -42.0f"), "-42 float");
	}
	{
		std::string out = captureConvert("2147483647");
		expect(contains(out, "int: 2147483647"), "INT_MAX int");
	}
	{
		std::string out = captureConvert("-2147483648");
		expect(contains(out, "int: -2147483648"), "INT_MIN int");
	}

	// === 6. int オーバーフロー ===
	section("6. int overflow -> impossible");
	{
		std::string out = captureConvert("2147483648");
		expect(contains(out, "int: impossible"), "INT_MAX+1 int impossible");
		expect(contains(out, "double: 2147483648.0"), "INT_MAX+1 still double");
	}
	{
		std::string out = captureConvert("-2147483649");
		expect(contains(out, "int: impossible"), "INT_MIN-1 int impossible");
	}
	{
		std::string out = captureConvert("9999999999");
		expect(contains(out, "int: impossible"), "large int impossible");
	}

	// === 7. float リテラル ===
	section("7. float literals");
	{
		std::string out = captureConvert("4.2f");
		expect(contains(out, "float: 4.2f"), "4.2f float");
		expect(contains(out, "double: 4.2"), "4.2f double");
		expect(contains(out, "int: 4"), "4.2f int (truncated)");
	}
	{
		std::string out = captureConvert("-4.2f");
		expect(contains(out, "float: -4.2f"), "-4.2f float");
		expect(contains(out, "int: -4"), "-4.2f int");
		expect(contains(out, "char: impossible"), "-4.2f char impossible");
	}
	{
		std::string out = captureConvert("0.0f");
		expect(contains(out, "float: 0.0f"), "0.0f float");
	}

	// === 8. double リテラル ===
	section("8. double literals");
	{
		std::string out = captureConvert("4.2");
		expect(contains(out, "double: 4.2"), "4.2 double");
		expect(contains(out, "float: 4.2f"), "4.2 float");
	}
	{
		std::string out = captureConvert("-4.2");
		expect(contains(out, "double: -4.2"), "-4.2 double");
	}
	{
		std::string out = captureConvert("3.14");
		expect(contains(out, "double: 3.1"), "3.14 double (setprecision 1)");
	}

	// === 9. pseudo-literals ===
	section("9. pseudo-literals");
	{
		std::string out = captureConvert("nanf");
		expect(contains(out, "float: nanf"), "nanf float");
		expect(contains(out, "double: nan"), "nanf double");
		expect(contains(out, "char: impossible"), "nanf char impossible");
		expect(contains(out, "int: impossible"), "nanf int impossible");
	}
	{
		std::string out = captureConvert("+inf");
		expect(contains(out, "float: +inff"), "+inf float");
		expect(contains(out, "double: +inf"), "+inf double");
	}
	{
		std::string out = captureConvert("-inf");
		expect(contains(out, "float: -inff"), "-inf float");
		expect(contains(out, "double: -inf"), "-inf double");
	}
	{
		std::string out = captureConvert("+inff");
		expect(contains(out, "float: +inff"), "+inff float");
		expect(contains(out, "double: +inf"), "+inff double");
	}
	{
		std::string out = captureConvert("-inff");
		expect(contains(out, "float: -inff"), "-inff float");
		expect(contains(out, "double: -inf"), "-inff double");
	}

	// === 10. char 境界 ===
	section("10. char boundaries");
	{
		std::string out = captureConvert("32");   // space (displayable)
		expect(contains(out, "char: ' '"), "32 -> space");
	}
	{
		std::string out = captureConvert("31");   // control char (non-displayable)
		expect(contains(out, "char: Non displayable"), "31 -> Non displayable");
	}
	{
		std::string out = captureConvert("127");  // DEL (non-displayable)
		expect(contains(out, "char: Non displayable"), "127 -> Non displayable");
	}
	{
		std::string out = captureConvert("128");  // out of char range
		expect(contains(out, "char: impossible"), "128 -> impossible");
	}
	{
		std::string out = captureConvert("126");  // ~
		expect(contains(out, "char: '~'"), "126 -> ~");
	}

	// === 11. float overflow → +inff ===
	section("11. float overflow");
	{
		std::string out = captureConvert("1e40");
		expect(contains(out, "float: +inff") || contains(out, "float: impossible"),
		       "1e40 float overflow to +inff");
	}
	{
		std::string out = captureConvert("-1e40");
		expect(contains(out, "float: -inff") || contains(out, "float: impossible"),
		       "-1e40 float overflow to -inff");
	}

	// === 12. 無効なリテラル ===
	section("12. invalid literals");
	{
		std::string out = captureConvert("abc");
		expect(contains(out, "impossible"), "abc all impossible");
	}
	{
		std::string out = captureConvert("42x");
		expect(contains(out, "impossible"), "42x invalid");
	}
	{
		std::string out = captureConvert("3.14g");
		expect(contains(out, "impossible"), "3.14g invalid");
	}
	{
		std::string out = captureConvert("");
		expect(contains(out, "impossible"), "empty string invalid");
	}
	{
		std::string out = captureConvert("42.0ff");
		expect(contains(out, "impossible"), "42.0ff invalid");
	}

	// === 13. 全書式一貫性: convert 実行後もstd::coutが正常 ===
	section("13. stream state restored after convert");
	{
		captureConvert("42.5");
		std::ostringstream oss;
		std::streambuf* saved = std::cout.rdbuf(oss.rdbuf());
		std::cout << 3.14;
		std::cout.rdbuf(saved);
		// 3.14 は setprecision(6) デフォルトで "3.14" と印字される
		expect(oss.str() == "3.14", "stream precision restored");
	}

	// === 14. 数字1文字 "0", "5", "9" は int と解釈される ===
	section("14. single-digit strings are int, not char");
	{
		std::string out = captureConvert("5");
		expect(contains(out, "int: 5"), "5 int");
		expect(contains(out, "char: Non displayable"), "5 char non-displayable");
	}
	{
		std::string out = captureConvert("9");
		expect(contains(out, "int: 9"), "9 int");
	}

	// === 15. 負の値のchar は impossible ===
	section("15. negative -> char impossible");
	{
		std::string out = captureConvert("-1");
		expect(contains(out, "char: impossible"), "-1 char impossible");
		expect(contains(out, "int: -1"), "-1 int");
	}

	// === 16. leak safety: 10000回 convert ===
	section("16. 10,000 convert calls (leak safety)");
	{
		for (int i = 0; i < 10000; ++i) {
			std::ostringstream oss;
			std::streambuf* saved = std::cout.rdbuf(oss.rdbuf());
			ScalarConverter::convert("42.0f");
			std::cout.rdbuf(saved);
		}
		expect(true, "no crash after 10000 iterations");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
}

int main(int argc, char** argv) {
	if (argc == 2) {
		ScalarConverter::convert(argv[1]);
		return 0;
	}
	if (argc == 1) {
		runTests();
		return g_fail == 0 ? 0 : 1;
	}
	std::cerr << "Usage: " << argv[0] << " <literal>" << std::endl;
	return 1;
}
