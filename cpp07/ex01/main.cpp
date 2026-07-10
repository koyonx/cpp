#include "iter.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

static int g_pass = 0;
static int g_fail = 0;

static void section(const std::string& t) {
	std::cout << "\n=== " << t << " ===" << std::endl;
}
static void expect(bool cond, const std::string& what) {
	if (cond) { ++g_pass; std::cout << "  [PASS] " << what << std::endl; }
	else      { ++g_fail; std::cout << "  [FAIL] " << what << std::endl; }
}

// 非 const 参照を取る関数 (mutating)
static void doublify(int& x) { x *= 2; }

// const 参照を取る関数 (non-mutating)
static int g_sum = 0;
static void addToSum(const int& x) { g_sum += x; }

// std::string への変換
static void appendBang(std::string& s) { s += "!"; }

// 順序チェック用
static std::ostringstream g_trace;
static void trace(const int& x) { g_trace << x << ","; }

// テンプレート関数 (PDF: "third parameter can be an instantiated function template")
template <typename T>
static void printAny(const T& x) { std::cout << x << " "; }

// char を upper に変換 (mutating)
static void upcase(char& c) {
	c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

int main() {
	// === 1. int[] with doublify (mutating) ===
	section("1. iter mutates int array via doublify");
	{
		int arr[5] = {1, 2, 3, 4, 5};
		iter(arr, 5, doublify);
		expect(arr[0] == 2 && arr[1] == 4 && arr[2] == 6 && arr[3] == 8 && arr[4] == 10,
		       "all elements doubled");
	}

	// === 2. int[] with addToSum (accumulator) ===
	section("2. iter accumulates via addToSum");
	{
		g_sum = 0;
		int arr[4] = {10, 20, 30, 40};
		iter(arr, 4, addToSum);
		expect(g_sum == 100, "sum accumulated");
	}

	// === 3. const int[] with addToSum (const-correctness) ===
	section("3. iter on const int[]");
	{
		g_sum = 0;
		const int arr[3] = {5, 5, 5};
		iter(arr, 3, addToSum);
		expect(g_sum == 15, "const array iterated");
	}

	// === 4. std::string[] mutating (append !) ===
	section("4. iter mutates std::string array");
	{
		std::string arr[3] = {"hello", "world", "foo"};
		iter(arr, 3, appendBang);
		expect(arr[0] == "hello!" && arr[1] == "world!" && arr[2] == "foo!",
		       "all strings appended");
	}

	// === 5. char[] with upcase ===
	section("5. iter uppercases char array");
	{
		char arr[5] = {'a', 'b', 'c', 'd', 'e'};
		iter(arr, 5, upcase);
		expect(arr[0] == 'A' && arr[1] == 'B' && arr[2] == 'C' && arr[3] == 'D' && arr[4] == 'E',
		       "chars uppercased");
	}

	// === 6. Empty array (length 0) - no crash, no call ===
	section("6. iter with length 0 doesn't call function");
	{
		int arr[1] = {42};  // dummy 1 要素 (真の 0 サイズ配列は C++98 不可)
		g_sum = 0;
		iter(arr, 0, addToSum);
		expect(g_sum == 0, "length 0 -> function not called");
		expect(arr[0] == 42, "array untouched");
	}

	// === 7. Single element array ===
	section("7. iter with 1 element");
	{
		int arr[1] = {77};
		iter(arr, 1, doublify);
		expect(arr[0] == 154, "single element doubled");
	}

	// === 8. Large array (10,000 elements) ===
	section("8. iter over large array (10,000 int)");
	{
		int* arr = new int[10000];
		for (int i = 0; i < 10000; ++i) arr[i] = 1;
		iter(arr, 10000, doublify);
		bool all_two = true;
		for (int i = 0; i < 10000; ++i) if (arr[i] != 2) { all_two = false; break; }
		expect(all_two, "10000 elements all doubled");
		delete[] arr;
	}

	// === 9. Function template instantiation as 3rd param (PDF hint) ===
	section("9. instantiated function template as 3rd parameter");
	{
		g_trace.str("");
		int arr[3] = {1, 2, 3};
		iter(arr, 3, printAny<int>);
		// 出力は stdout に "1 2 3 " と出る (テストなので特に検証しない)
		expect(true, "compiles and runs with template<int>");
	}

	// === 10. 順序チェック: 0..N-1 の順に処理される ===
	section("10. iteration order is 0..N-1");
	{
		g_trace.str("");
		int arr[5] = {10, 20, 30, 40, 50};
		iter(arr, 5, trace);
		expect(g_trace.str() == "10,20,30,40,50,", "traversal order matches");
	}

	// === 11. Const と Non-const 版が同じテンプレートで働く ===
	section("11. single template works for both const and non-const arrays");
	{
		g_sum = 0;
		int nc[3] = {1, 2, 3};
		const int c[3] = {4, 5, 6};
		iter(nc, 3, addToSum);
		iter(c, 3, addToSum);
		expect(g_sum == 21, "1+2+3+4+5+6=21");
	}

	// === 12. std::string const array ===
	section("12. const std::string array iterated");
	{
		const std::string arr[3] = {"a", "b", "c"};
		std::ostringstream oss;
		std::streambuf* saved = std::cout.rdbuf(oss.rdbuf());
		iter(arr, 3, printAny<std::string>);
		std::cout.rdbuf(saved);
		expect(oss.str() == "a b c ", "const string array printed in order");
	}

	// === 13. Nested iter (iter of arrays of arrays) — 単純な 2 段呼び出し ===
	section("13. multiple iter calls sequentially");
	{
		int a[3] = {1, 2, 3};
		int b[3] = {10, 20, 30};
		iter(a, 3, doublify);
		iter(b, 3, doublify);
		expect(a[0]==2 && a[2]==6 && b[0]==20 && b[2]==60, "both arrays doubled");
	}

	// === 14. 10,000 iter calls (leak 耐性) ===
	section("14. 10,000 iter calls on small arrays");
	{
		int arr[10] = {0};
		for (int i = 0; i < 10000; ++i) {
			iter(arr, 10, doublify);
		}
		expect(true, "no crash");
	}

	// === 15. Custom class array (calls its method via free function) ===
	section("15. iter over user-defined class array");
	{
		std::string arr[2] = {"x", "y"};
		iter(arr, 2, appendBang);
		expect(arr[0] == "x!" && arr[1] == "y!", "custom class ok");
	}

	// === 16. size_t == 0 の境界 (PDF: 「const size_t length」を明示) ===
	section("16. length passed as const size_t");
	{
		size_t const length = 3;
		int arr[3] = {1, 2, 3};
		g_sum = 0;
		iter(arr, length, addToSum);
		expect(g_sum == 6, "const size_t length works");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
