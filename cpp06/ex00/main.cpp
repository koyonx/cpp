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

	// === 17. 科学記法 (double) ===
	section("17. scientific notation - double");
	{
		std::string out = captureConvert("1e2");
		expect(contains(out, "int: 100"), "1e2 int");
		expect(contains(out, "double: 100.0"), "1e2 double");
	}
	{
		std::string out = captureConvert("1.5e3");
		expect(contains(out, "int: 1500"), "1.5e3 int");
	}
	{
		std::string out = captureConvert("1E5");
		expect(contains(out, "int: 100000"), "1E5 (uppercase) int");
	}
	{
		std::string out = captureConvert("-1e3");
		expect(contains(out, "int: -1000"), "-1e3 int");
	}

	// === 18. 科学記法 (float 'f' suffix) ===
	section("18. scientific notation - float");
	{
		std::string out = captureConvert("1e2f");
		expect(contains(out, "float: 100.0f"), "1e2f float");
	}
	{
		std::string out = captureConvert("2.5e2f");
		expect(contains(out, "float: 250.0f"), "2.5e2f float");
	}

	// === 19. 正符号リテラル ===
	section("19. leading + sign literals");
	{
		std::string out = captureConvert("+42");
		expect(contains(out, "int: 42"), "+42 int");
		expect(contains(out, "char: '*'"), "+42 char");
	}
	{
		std::string out = captureConvert("+4.2");
		expect(contains(out, "double: 4.2"), "+4.2 double");
	}
	{
		std::string out = captureConvert("+4.2f");
		expect(contains(out, "float: 4.2f"), "+4.2f float");
	}

	// === 20. 全 printable ASCII 文字が正しく識別される (32..126) ===
	section("20. all printable ASCII chars roundtrip");
	{
		bool ok = true;
		for (int i = 32; i <= 126; ++i) {
			// 数字は int 扱いなのでスキップ
			if (i >= '0' && i <= '9') continue;
			std::string s(1, static_cast<char>(i));
			std::string out = captureConvert(s);
			std::ostringstream expect_char;
			expect_char << "char: '" << static_cast<char>(i) << "'";
			std::ostringstream expect_int;
			expect_int << "int: " << i;
			if (!contains(out, expect_char.str()) || !contains(out, expect_int.str())) {
				ok = false;
				break;
			}
		}
		expect(ok, "all 85 non-digit printable chars round-trip correctly");
	}

	// === 21. Truncation semantics: 42.9 → int 42, -4.9 → -4 ===
	section("21. int conversion uses truncation (toward zero)");
	{
		std::string out = captureConvert("42.9");
		expect(contains(out, "int: 42"), "42.9 -> int 42 (truncated)");
	}
	{
		std::string out = captureConvert("42.9f");
		expect(contains(out, "int: 42"), "42.9f -> int 42");
	}
	{
		std::string out = captureConvert("-4.9");
		expect(contains(out, "int: -4"), "-4.9 -> int -4");
	}
	{
		std::string out = captureConvert("0.9");
		expect(contains(out, "int: 0"), "0.9 -> int 0");
	}

	// === 22. 冪等性: 同じ入力で同じ出力 ===
	section("22. determinism: same input -> same output");
	{
		std::string a = captureConvert("42.5");
		std::string b = captureConvert("42.5");
		std::string c = captureConvert("42.5");
		expect(a == b && b == c, "3 consecutive calls produce identical output");
	}

	// === 23. 極端に小さい/大きい double ===
	section("23. extreme double values");
	{
		std::string out = captureConvert("1e300");
		expect(contains(out, "int: impossible"), "1e300 int impossible");
		expect(contains(out, "float: +inff"), "1e300 float overflow to +inff");
		expect(contains(out, "char: impossible"), "1e300 char impossible");
	}
	{
		std::string out = captureConvert("-1e300");
		expect(contains(out, "float: -inff"), "-1e300 float overflow");
	}
	{
		std::string out = captureConvert("1e-40");
		// 1e-40 は double では表現可能、float では subnormal or 0
		expect(contains(out, "int: 0"), "1e-40 int is 0 (truncation)");
	}

	// === 24. 追加の無効入力パターン ===
	section("24. additional invalid input patterns");
	// 単一の非digit文字は char literal と解釈される (PDF: 'c', 'a', ... の仕様)
	{
		std::string out = captureConvert("+");
		expect(contains(out, "char: '+'"), "'+' -> char '+' (single non-digit)");
		expect(contains(out, "int: 43"), "'+' -> int 43");
	}
	{
		std::string out = captureConvert("-");
		expect(contains(out, "char: '-'"), "'-' -> char '-'");
		expect(contains(out, "int: 45"), "'-' -> int 45");
	}
	// 複数文字の連続 sign はパース不能 -> impossible
	{
		std::string out = captureConvert("--42");
		expect(contains(out, "impossible"), "'--42' invalid");
	}
	{
		std::string out = captureConvert("++42");
		expect(contains(out, "impossible"), "'++42' invalid");
	}
	{
		std::string out = captureConvert("4..2");
		expect(contains(out, "impossible"), "'4..2' invalid");
	}
	{
		std::string out = captureConvert("42f");
		expect(contains(out, "float: 42.0f"), "'42f' -> float (integer form + f)");
	}

	// === 25. Boundary int: INT_MAX と INT_MAX+1 の差 ===
	section("25. int boundary: INT_MAX vs INT_MAX+1");
	{
		std::string out = captureConvert("2147483646");
		expect(contains(out, "int: 2147483646"), "INT_MAX-1 valid");
	}
	{
		std::string out = captureConvert("2147483647");
		expect(contains(out, "int: 2147483647"), "INT_MAX valid");
	}
	{
		std::string out = captureConvert("2147483648");
		expect(contains(out, "int: impossible"), "INT_MAX+1 -> impossible");
	}

	// === 26. PDF 例の 4 行厳密一致 (exact multi-line) ===
	section("26. PDF example exact 4-line output");
	{
		std::string expected =
			"char: Non displayable\n"
			"int: 0\n"
			"float: 0.0f\n"
			"double: 0.0\n";
		expect(captureConvert("0") == expected, "'0' exact 4-line output");
	}
	{
		std::string expected =
			"char: '*'\n"
			"int: 42\n"
			"float: 42.0f\n"
			"double: 42.0\n";
		expect(captureConvert("42.0f") == expected, "'42.0f' exact 4-line output");
	}
	{
		std::string expected =
			"char: impossible\n"
			"int: impossible\n"
			"float: nanf\n"
			"double: nan\n";
		expect(captureConvert("nan") == expected, "'nan' exact 4-line output");
	}

	// === 27. char 特殊: '0' vs 0 (digit は int 扱い、非digit-single は char) ===
	section("27. digit vs non-digit single-char handling");
	{
		std::string out_digit = captureConvert("0");
		expect(contains(out_digit, "int: 0"), "'0' parsed as int 0");
		expect(contains(out_digit, "char: Non displayable"), "'0' -> ASCII 0 char");
	}
	{
		std::string out_char = captureConvert("A");
		expect(contains(out_char, "char: 'A'"), "'A' parsed as char 'A'");
		expect(contains(out_char, "int: 65"), "'A' int 65");
	}

	// === 28. 入力に空白を含む: これらは無効 (strtod は先頭空白を skip するが suffix は不許可) ===
	section("28. inputs with spaces: rejected as invalid");
	{
		std::string out = captureConvert("42 ");
		expect(contains(out, "impossible"), "'42 ' (trailing space) invalid");
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
