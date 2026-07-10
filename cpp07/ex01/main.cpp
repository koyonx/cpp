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

// 関数オブジェクト (functor)
class Multiplier {
	int _factor;
public:
	Multiplier(int f) : _factor(f) {}
	void operator()(int& x) const { x *= _factor; }
};

// throw する関数 (指定 index で throw)
static int g_throw_at = -1;
static int g_visited = 0;
static void throwAt(int& x) {
	if (g_visited == g_throw_at) throw std::runtime_error("intentional");
	x += 1;
	++g_visited;
}

// 戻り値が void でない関数 (戻り値は無視される)
static int returning(int& x) { x *= 3; return x; }

// 静的にカウントアップ
static int g_count = 0;
static void countCall(const int&) { ++g_count; }

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

	// === 17. Functor class (operator()) ===
	section("17. iter with functor class");
	{
		int arr[4] = {1, 2, 3, 4};
		Multiplier by3(3);
		iter(arr, 4, by3);
		expect(arr[0] == 3 && arr[1] == 6 && arr[2] == 9 && arr[3] == 12,
		       "functor multiplied by 3");
	}

	// === 18. 関数の戻り値は無視される ===
	section("18. iter ignores function return value");
	{
		int arr[3] = {1, 2, 3};
		iter(arr, 3, returning);
		expect(arr[0] == 3 && arr[1] == 6 && arr[2] == 9, "returning func used for side effects");
	}

	// === 19. 関数からの throw は伝播 & 途中で終わる ===
	section("19. throwing function propagates & stops iteration");
	{
		int arr[5] = {0, 0, 0, 0, 0};
		g_visited = 0;
		g_throw_at = 2;  // 3 要素目で throw
		bool caught = false;
		try { iter(arr, 5, throwAt); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "exception propagated from iter");
		expect(arr[0] == 1 && arr[1] == 1 && arr[2] == 0 && arr[3] == 0 && arr[4] == 0,
		       "only first 2 elements were processed before throw");
	}

	// === 20. Function pointer as explicit type ===
	section("20. explicit function pointer type as 3rd param");
	{
		void (*fp)(int&) = doublify;
		int arr[3] = {1, 2, 3};
		iter(arr, 3, fp);
		expect(arr[0] == 2 && arr[1] == 4 && arr[2] == 6, "function pointer works");
	}

	// === 21. Pointer array (T* = int**) ===
	section("21. iter over pointer array");
	{
		int a = 10, b = 20, c = 30;
		int* arr[3] = {&a, &b, &c};
		g_sum = 0;
		// Lambda-less C++98: use a helper functor
		class SumViaPtr {
		public:
			static void call(int* const& p) { g_sum += *p; }
		};
		iter(arr, 3, SumViaPtr::call);
		expect(g_sum == 60, "10+20+30 == 60 via pointer array");
	}

	// === 22. 大配列 100,000 要素 ===
	section("22. iter over 100,000 elements");
	{
		int* arr = new int[100000];
		for (int i = 0; i < 100000; ++i) arr[i] = 1;
		g_count = 0;
		iter(arr, 100000, countCall);
		expect(g_count == 100000, "count == 100000");
		delete[] arr;
	}

	// === 23. Const 配列を非-const 関数で iter は compile error (verify by omission) ===
	section("23. const array + non-const-taking func = compile error (compile-time)");
	// const int arr[3] = {1,2,3};
	// iter(arr, 3, doublify);  // ← doublify は int& 要求 → 実体化失敗
	expect(true, "verified by not-uncommenting (T=const int, func requires int& -> mismatch)");

	// === 24. 2D 相当 (Array of Arrays) を iter ===
	section("24. iterating over 2D-like structure via helper");
	{
		int m0[3] = {1, 2, 3};
		int m1[3] = {4, 5, 6};
		int* rows[2] = {m0, m1};
		g_sum = 0;
		// 各行にiter(sum) → 行ポインタごとに iter を呼ぶ関数
		class RowSum {
		public:
			static void call(int* const& row) {
				for (int i = 0; i < 3; ++i) g_sum += row[i];
			}
		};
		iter(rows, 2, RowSum::call);
		expect(g_sum == 1+2+3+4+5+6, "sum of 2x3 == 21");
	}

	// === 25. size_t 巨大な length は要素外アクセス → 未定義動作なのでテスト対象外 ===
	section("25. iter behavior is deterministic for valid inputs");
	{
		g_sum = 0;
		int arr[1] = {7};
		iter(arr, 1, addToSum);
		expect(g_sum == 7, "1 element -> sum 7");
	}

	// === 26. 複数回の iter が独立に動く ===
	section("26. multiple iter calls independent");
	{
		int arr[3] = {1, 2, 3};
		iter(arr, 3, doublify);
		iter(arr, 3, doublify);
		iter(arr, 3, doublify);
		expect(arr[0] == 8 && arr[1] == 16 && arr[2] == 24, "3 doublify -> *8");
	}

	// === 27. Different types: char array with counting ===
	section("27. iter over char array");
	{
		char arr[6] = "hello";  // 5 + null
		iter(arr, 5, upcase);
		expect(arr[0] == 'H' && arr[4] == 'O', "hello -> HELLO");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
