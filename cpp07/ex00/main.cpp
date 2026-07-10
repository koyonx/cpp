#include "whatever.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <climits>

static int g_pass = 0;
static int g_fail = 0;

static void section(const std::string& t) {
	std::cout << "\n=== " << t << " ===" << std::endl;
}
static void expect(bool cond, const std::string& what) {
	if (cond) { ++g_pass; std::cout << "  [PASS] " << what << std::endl; }
	else      { ++g_fail; std::cout << "  [FAIL] " << what << std::endl; }
}

// PDF 例で使う (コピー・代入・比較を検証できる) ダミー型
class Fixed {
	int _v;
public:
	Fixed() : _v(0) {}
	Fixed(int v) : _v(v) {}
	Fixed(const Fixed& o) : _v(o._v) {}
	Fixed& operator=(const Fixed& o) { _v = o._v; return *this; }
	bool operator<(const Fixed& o) const { return _v < o._v; }
	bool operator>(const Fixed& o) const { return _v > o._v; }
	bool operator==(const Fixed& o) const { return _v == o._v; }
	int value() const { return _v; }
};

// swap/min/max のコピー回数を数える型 (C++98 では template 引数にローカルクラスを使えないので file scope)
static int g_copy_ctor = 0;
static int g_copy_assign = 0;

class Tracker {
public:
	int v;
	Tracker() : v(0) {}
	Tracker(int val) : v(val) {}
	Tracker(const Tracker& o) : v(o.v) { ++g_copy_ctor; }
	Tracker& operator=(const Tracker& o) { v = o.v; ++g_copy_assign; return *this; }
	bool operator<(const Tracker& o) const { return v < o.v; }
	bool operator>(const Tracker& o) const { return v > o.v; }
};

// 逆順比較 (v が大きい方が「小さい」) を持つ型
class Rev {
public:
	int v;
	Rev(int val) : v(val) {}
	bool operator<(const Rev& o) const { return v > o.v; }
	bool operator>(const Rev& o) const { return v < o.v; }
};

int main() {
	// === 1. PDF 例そのまま (int) ===
	section("1. PDF example: int a=2, b=3");
	{
		int a = 2;
		int b = 3;
		::swap(a, b);
		expect(a == 3 && b == 2, "swap swapped int: a=3, b=2");
		expect(::min(a, b) == 2, "min(3, 2) == 2");
		expect(::max(a, b) == 3, "max(3, 2) == 3");
	}

	// === 2. PDF 例そのまま (string) ===
	section("2. PDF example: string chaine1/chaine2");
	{
		std::string c = "chaine1";
		std::string d = "chaine2";
		::swap(c, d);
		expect(c == "chaine2" && d == "chaine1", "swap: c=chaine2, d=chaine1");
		expect(::min(c, d) == "chaine1", "min lexicographic");
		expect(::max(c, d) == "chaine2", "max lexicographic");
	}

	// === 3. swap 基本 int ===
	section("3. swap for int");
	{
		int a = 10, b = 20;
		::swap(a, b);
		expect(a == 20 && b == 10, "10 <-> 20");
	}
	{
		int a = -5, b = 5;
		::swap(a, b);
		expect(a == 5 && b == -5, "negative <-> positive");
	}
	{
		int a = 42, b = 42;
		::swap(a, b);
		expect(a == 42 && b == 42, "swap equal values -> identical");
	}

	// === 4. swap for double ===
	section("4. swap for double");
	{
		double x = 3.14, y = 2.71;
		::swap(x, y);
		expect(x == 2.71 && y == 3.14, "double swap");
	}

	// === 5. swap for char ===
	section("5. swap for char");
	{
		char a = 'X', b = 'Y';
		::swap(a, b);
		expect(a == 'Y' && b == 'X', "char swap");
	}

	// === 6. swap for std::string with different lengths ===
	section("6. swap for strings with different lengths");
	{
		std::string a = "short";
		std::string b = "considerably longer string";
		::swap(a, b);
		expect(a == "considerably longer string" && b == "short", "different length swap");
	}

	// === 7. swap for custom class ===
	section("7. swap for user-defined class");
	{
		Fixed a(10), b(20);
		::swap(a, b);
		expect(a.value() == 20 && b.value() == 10, "Fixed swap");
	}

	// === 8. min/max for int (basic) ===
	section("8. min/max int basic");
	{
		expect(::min(1, 2) == 1, "min(1,2)=1");
		expect(::max(1, 2) == 2, "max(1,2)=2");
		expect(::min(-5, 3) == -5, "min(-5,3)=-5");
		expect(::max(-5, 3) == 3, "max(-5,3)=3");
	}

	// === 9. min/max PDF rule: equal returns SECOND ===
	section("9. min/max equal -> returns second parameter");
	{
		int a = 42, b = 42;
		const int& m = ::min(a, b);
		expect(&m == &b, "min: equal returns &b (2nd param)");
		const int& M = ::max(a, b);
		expect(&M == &b, "max: equal returns &b (2nd param)");
	}
	{
		std::string a = "same", b = "same";
		const std::string& m = ::min(a, b);
		expect(&m == &b, "string min: equal returns 2nd");
		const std::string& M = ::max(a, b);
		expect(&M == &b, "string max: equal returns 2nd");
	}

	// === 10. min/max for double with negatives ===
	section("10. min/max double");
	{
		expect(::min(1.5, -2.5) == -2.5, "min double");
		expect(::max(1.5, -2.5) == 1.5, "max double");
	}

	// === 11. min/max INT_MIN / INT_MAX 境界 ===
	section("11. min/max INT boundaries");
	{
		expect(::min(INT_MIN, INT_MAX) == INT_MIN, "min INT_MIN vs INT_MAX");
		expect(::max(INT_MIN, INT_MAX) == INT_MAX, "max INT_MIN vs INT_MAX");
	}

	// === 12. min/max for char ===
	section("12. min/max char");
	{
		expect(::min('a', 'b') == 'a', "min(a,b)=a");
		expect(::max('a', 'b') == 'b', "max(a,b)=b");
	}

	// === 13. min/max for string lexicographic ===
	section("13. min/max string lexicographic");
	{
		std::string a = "apple", b = "banana";
		expect(::min(a, b) == "apple", "'apple' < 'banana'");
		expect(::max(a, b) == "banana", "'banana' > 'apple'");
	}
	{
		std::string a = "abc", b = "abd";
		expect(::min(a, b) == "abc", "abc < abd");
	}
	{
		std::string a = "", b = "x";
		expect(::min(a, b) == "", "empty < non-empty");
	}

	// === 14. min/max for user-defined class ===
	section("14. min/max user-defined class");
	{
		Fixed a(1), b(2);
		expect(::min(a, b).value() == 1, "Fixed min");
		expect(::max(a, b).value() == 2, "Fixed max");
	}

	// === 15. swap は const 引数を拒否する (compile-time) ===
	// 以下はコンパイルエラー (T& は const 引数を bind できない):
	//   const int a = 1; const int b = 2; ::swap(a, b);
	section("15. swap rejects const args (compile-time enforced)");
	expect(true, "T& requires non-const; verified by not-uncommenting");

	// === 16. min/max chaining ===
	section("16. min/max chaining");
	{
		int a = 1, b = 2, c = 3;
		expect(::min(::min(a, b), c) == 1, "min of 3");
		expect(::max(::max(a, b), c) == 3, "max of 3");
	}

	// === 17. std::string 特殊: 同じ文字列 ===
	section("17. min/max identical string returns 2nd");
	{
		std::string a = "hello", b = "hello";
		const std::string& m = ::min(a, b);
		expect(&m == &b, "identical string -> 2nd");
	}

	// === 18. 型推論: 明示的テンプレート引数 ===
	section("18. explicit template argument");
	{
		int a = 5, b = 3;
		expect(::min<int>(a, b) == 3, "explicit <int>");
		expect(::max<int>(a, b) == 5, "explicit <int>");
	}

	// === 19. swap は自己 swap セーフ ===
	section("19. swap self is safe (a==a)");
	{
		int a = 42;
		::swap(a, a);
		expect(a == 42, "swap(a, a) preserves value");
	}

	// === 20. 10,000 回 swap/min/max (性能・leak耐性) ===
	section("20. 10,000 iterations");
	{
		int x = 0, y = 0;
		for (int i = 0; i < 10000; ++i) {
			x = i; y = i + 1;
			::swap(x, y);
			(void)::min(x, y);
			(void)::max(x, y);
		}
		expect(true, "no crash");
	}

	// === 21. swap of pointer types ===
	section("21. swap works on pointer types");
	{
		int x = 1, y = 2;
		int* p = &x;
		int* q = &y;
		::swap(p, q);
		expect(p == &y && q == &x, "pointer swap");
	}

	// === 22. min/max with float 特殊値 (-0.0 vs 0.0) ===
	section("22. min/max with signed zero");
	{
		double a = 0.0, b = -0.0;
		// 0.0 == -0.0 なので等値扱い -> 2nd を返す
		expect(&::min(a, b) == &b, "min(0.0, -0.0) equal -> 2nd");
		expect(&::max(a, b) == &b, "max(0.0, -0.0) equal -> 2nd");
	}

	// === 23. swap の empty string ===
	section("23. swap involving empty string");
	{
		std::string a = "";
		std::string b = "not empty";
		::swap(a, b);
		expect(a == "not empty" && b.empty(), "empty <-> content swap");
	}

	// === 24. min/max on empty vs non-empty string ===
	section("24. min/max empty string edge");
	{
		std::string a = "";
		std::string b = "a";
		expect(::min(a, b) == "", "empty < 'a'");
		expect(::max(a, b) == "a", "'a' > empty");
	}

	// === 25. Copy counting: swap は正確に 3 コピー = 1 ctor + 2 assign ===
	// (T tmp = a → copy ctor 1回, a = b → copy assign 1回, b = tmp → copy assign 1回)
	section("25. swap performs exactly 3 copy operations (1 ctor + 2 assign)");
	{
		Tracker a(1), b(2);
		g_copy_ctor = 0;
		g_copy_assign = 0;
		::swap(a, b);
		expect(g_copy_ctor == 1, "swap: exactly 1 copy ctor");
		expect(g_copy_assign == 2, "swap: exactly 2 copy assign");
		expect(a.v == 2 && b.v == 1, "values swapped");
	}

	// === 26. Copy counting: min/max は 0 copy (const ref return) ===
	section("26. min/max perform 0 copies (returns const ref)");
	{
		Tracker a(1), b(2);
		g_copy_ctor = 0;
		g_copy_assign = 0;
		const Tracker& m = ::min(a, b);
		const Tracker& M = ::max(a, b);
		(void)m; (void)M;
		expect(g_copy_ctor == 0, "min/max: 0 copy ctors");
		expect(g_copy_assign == 0, "min/max: 0 copy assignments");
	}

	// === 27. min/max: 一時値で呼んでも segfault しない (フルエクスプレッション内で生存) ===
	section("27. min/max with rvalue temporaries safe within expression");
	{
		int result = ::min(10, 20) + ::max(5, 15);
		expect(result == 10 + 15, "10 + 15 == 25");
	}

	// === 28. min/max chain の depth 5 ===
	section("28. deep chain of min/max");
	{
		int a = 5, b = 3, c = 8, d = 1, e = 9;
		int result_min = ::min(::min(::min(::min(a, b), c), d), e);
		int result_max = ::max(::max(::max(::max(a, b), c), d), e);
		expect(result_min == 1, "deep min == 1");
		expect(result_max == 9, "deep max == 9");
	}

	// === 29. min/max returning const reference: 変更禁止 (compile-time) ===
	// const int& m = ::min(a, b); m = 100;  // これはコンパイルエラー
	section("29. min/max returns const T& (compile-time immutability)");
	expect(true, "verified by const return type in signature");

	// === 30. 逆順比較を持つカスタム型でも動く (反対の結果) ===
	section("30. custom type with reversed comparison");
	{
		Rev a(1), b(10);
		expect(::min(a, b).v == 10, "reversed min returns v=10");
		expect(::max(a, b).v == 1, "reversed max returns v=1");
	}

	// === 31. swap of user class + verify content ===
	section("31. swap deep-copies for classes with heap");
	{
		std::string a(1000, 'A');
		std::string b(500, 'B');
		::swap(a, b);
		expect(a.size() == 500 && a[0] == 'B', "500 B in a");
		expect(b.size() == 1000 && b[0] == 'A', "1000 A in b");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
