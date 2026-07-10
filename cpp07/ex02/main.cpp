#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <Array.hpp>

static int g_pass = 0;
static int g_fail = 0;

static void section(const std::string& t) {
	std::cout << "\n=== " << t << " ===" << std::endl;
}
static void expect(bool cond, const std::string& what) {
	if (cond) { ++g_pass; std::cout << "  [PASS] " << what << std::endl; }
	else      { ++g_fail; std::cout << "  [FAIL] " << what << std::endl; }
}

// ctor/dtor/copy 回数を追跡するクラス (Array の内部管理を検証)
static int g_ctor = 0;
static int g_dtor = 0;
static int g_copy = 0;
static int g_assign = 0;

class Counted {
public:
	int v;
	Counted() : v(0) { ++g_ctor; }
	Counted(int val) : v(val) { ++g_ctor; }
	Counted(const Counted& o) : v(o.v) { ++g_copy; }
	Counted& operator=(const Counted& o) { v = o.v; ++g_assign; return *this; }
	~Counted() { ++g_dtor; }
};

// テスト用: Array<int> を値渡し (コピー発生) & 戻り値
static Array<int> passByValue(Array<int> arg) {
	// arg 内容を変更しても呼び出し側に影響しないことを確認
	if (arg.size() > 0) arg[0] = -999;
	return arg;
}

static Array<int> makeArray(unsigned int n) {
	Array<int> a(n);
	for (unsigned int i = 0; i < n; ++i) a[i] = static_cast<int>(i + 100);
	return a;
}

#define MAX_VAL 750

// ==== PDF (提供された) テストコードを関数化 ====
static bool pdfProvidedTest() {
	Array<int> numbers(MAX_VAL);
	int* mirror = new int[MAX_VAL];
	std::srand(static_cast<unsigned int>(std::time(NULL)));
	for (int i = 0; i < MAX_VAL; i++) {
		const int value = std::rand();
		numbers[i] = value;
		mirror[i] = value;
	}
	//SCOPE
	{
		Array<int> tmp = numbers;
		Array<int> test(tmp);
	}
	for (int i = 0; i < MAX_VAL; i++) {
		if (mirror[i] != numbers[i]) {
			std::cerr << "didn't save the same value!!" << std::endl;
			delete[] mirror;
			return false;
		}
	}
	bool caught_neg = false, caught_over = false;
	try { numbers[-2] = 0; }
	catch(const std::exception& e) { caught_neg = true; }
	try { numbers[MAX_VAL] = 0; }
	catch(const std::exception& e) { caught_over = true; }
	for (int i = 0; i < MAX_VAL; i++)
		numbers[i] = std::rand();
	delete[] mirror;
	return caught_neg && caught_over;
}

int main(int, char**) {
	// === 1. PDF 提供テスト ===
	section("1. PDF-provided test (750 elements, scope copy, out-of-bounds)");
	{
		expect(pdfProvidedTest(), "PDF test passes: values preserved after scope, -2/MAX both throw");
	}

	// === 2. デフォルトコンストラクタ: empty array ===
	section("2. default constructor -> empty array");
	{
		Array<int> a;
		expect(a.size() == 0, "size() == 0");
	}

	// === 3. size 付きコンストラクタ: T をデフォルト初期化 ===
	section("3. Array(n): elements default-initialized (int -> 0)");
	{
		Array<int> a(5);
		expect(a.size() == 5, "size() == 5");
		bool all_zero = true;
		for (unsigned int i = 0; i < 5; ++i)
			if (a[i] != 0) { all_zero = false; break; }
		expect(all_zero, "5 int elements zero-initialized");
	}
	{
		Array<double> d(3);
		expect(d[0] == 0.0 && d[1] == 0.0 && d[2] == 0.0, "3 double zero-initialized");
	}
	{
		Array<std::string> s(4);
		expect(s[0].empty() && s[3].empty(), "std::string default constructed (empty)");
	}

	// === 4. operator[] 読み書き ===
	section("4. operator[] read/write");
	{
		Array<int> a(3);
		a[0] = 10; a[1] = 20; a[2] = 30;
		expect(a[0] == 10 && a[1] == 20 && a[2] == 30, "3 elements set & read");
	}

	// === 5. operator[] out-of-bounds throw (positive index) ===
	section("5. out-of-bounds throws std::exception (index == size)");
	{
		Array<int> a(5);
		bool caught = false;
		try { a[5] = 0; }
		catch (const std::exception&) { caught = true; }
		expect(caught, "a[5] on size-5 array throws");
	}

	// === 6. operator[] out-of-bounds throw (huge index) ===
	section("6. out-of-bounds throws (huge index)");
	{
		Array<int> a(5);
		bool caught = false;
		try { a[1000000] = 0; }
		catch (const std::exception&) { caught = true; }
		expect(caught, "a[1000000] throws");
	}

	// === 7. operator[] out-of-bounds throw (negative int -> unsigned overflow) ===
	section("7. out-of-bounds throws (negative -> unsigned huge)");
	{
		Array<int> a(5);
		bool caught = false;
		try { a[-1] = 0; }
		catch (const std::exception&) { caught = true; }
		expect(caught, "a[-1] wraps to UINT_MAX -> throws");
	}

	// === 8. Empty array: 任意の index が throw ===
	section("8. empty array: any index throws");
	{
		Array<int> a;
		bool caught = false;
		try { a[0] = 0; }
		catch (const std::exception&) { caught = true; }
		expect(caught, "a[0] on empty array throws");
	}

	// === 9. Const Array の operator[] (read-only) ===
	section("9. const Array operator[] const-correctness");
	{
		Array<int> a(3);
		a[0] = 100; a[1] = 200; a[2] = 300;
		const Array<int>& c = a;
		expect(c[0] == 100 && c[1] == 200 && c[2] == 300, "const [] reads");
	}

	// === 10. Copy constructor: 独立コピー ===
	section("10. copy constructor produces independent array");
	{
		Array<int> a(3);
		a[0] = 1; a[1] = 2; a[2] = 3;
		Array<int> b(a);
		expect(b.size() == 3 && b[0] == 1 && b[1] == 2 && b[2] == 3, "b has same content");
		b[0] = 999;
		expect(a[0] == 1, "modifying b does not affect a");
	}

	// === 11. Copy constructor: source modification doesn't affect copy ===
	section("11. modifying source after copy does not affect copy");
	{
		Array<int> a(3);
		a[0] = 10;
		Array<int> b(a);
		a[0] = 999;
		expect(b[0] == 10, "b[0] unchanged");
	}

	// === 12. operator= (assignment) ===
	section("12. operator= produces independent assignment");
	{
		Array<int> a(3);
		a[0] = 1; a[1] = 2; a[2] = 3;
		Array<int> b(5);
		b = a;
		expect(b.size() == 3, "size copied");
		expect(b[0] == 1 && b[1] == 2 && b[2] == 3, "elements copied");
		b[0] = 999;
		expect(a[0] == 1, "modifying b doesn't affect a");
	}

	// === 13. operator= self-assignment ===
	section("13. self-assignment safe");
	{
		Array<int> a(3);
		a[0] = 42; a[1] = 43; a[2] = 44;
		a = a;
		expect(a.size() == 3 && a[0] == 42 && a[1] == 43 && a[2] == 44, "state preserved");
	}

	// === 14. operator= chaining (a = b = c) ===
	section("14. operator= chained a = b = c");
	{
		Array<int> a(2), b(2), c(2);
		c[0] = 7; c[1] = 8;
		a = b = c;
		expect(a[0] == 7 && a[1] == 8, "a takes c's values through b");
	}

	// === 15. size() は const method ===
	section("15. size() callable on const Array");
	{
		const Array<int> a(10);
		expect(a.size() == 10, "size() on const object");
	}

	// === 16. Array<std::string> ===
	section("16. Array<std::string>");
	{
		Array<std::string> a(3);
		a[0] = "hello";
		a[1] = "world";
		a[2] = "!";
		expect(a[0] == "hello" && a[1] == "world" && a[2] == "!", "string storage");
		Array<std::string> b(a);
		b[0] = "goodbye";
		expect(a[0] == "hello", "copy independence for string");
	}

	// === 17. Array<double> ===
	section("17. Array<double>");
	{
		Array<double> a(3);
		a[0] = 3.14; a[1] = 2.71; a[2] = -1.5;
		expect(a[0] == 3.14 && a[1] == 2.71 && a[2] == -1.5, "double storage");
	}

	// === 18. Large array (10,000 elements) ===
	section("18. large Array<int> (10,000)");
	{
		Array<int> a(10000);
		for (unsigned int i = 0; i < 10000; ++i) a[i] = static_cast<int>(i);
		bool ok = true;
		for (unsigned int i = 0; i < 10000; ++i)
			if (a[i] != static_cast<int>(i)) { ok = false; break; }
		expect(ok, "10000 elements set/get correctly");
	}

	// === 19. Copy of large array ===
	section("19. copy of large array");
	{
		Array<int> a(1000);
		for (unsigned int i = 0; i < 1000; ++i) a[i] = static_cast<int>(i * 2);
		Array<int> b(a);
		bool ok = true;
		for (unsigned int i = 0; i < 1000; ++i)
			if (b[i] != static_cast<int>(i * 2)) { ok = false; break; }
		expect(ok, "1000-element copy identical");
	}

	// === 20. what() message content ===
	section("20. OutOfBoundsException::what() message present");
	{
		Array<int> a(3);
		try {
			a[10] = 0;
		} catch (const std::exception& e) {
			std::string msg = e.what();
			expect(!msg.empty(), "what() returns non-empty string");
		}
	}

	// === 21. leak safety: 10,000 Array life cycles ===
	section("21. 10,000 Array<int> create/destroy cycles");
	{
		for (int i = 0; i < 10000; ++i) {
			Array<int> a(100);
			for (unsigned int j = 0; j < 100; ++j) a[j] = j;
		}
		expect(true, "10000 cycles no crash");
	}

	// === 22. leak safety: 10,000 copy cycles ===
	section("22. 10,000 copy constructor cycles");
	{
		Array<int> src(50);
		for (unsigned int j = 0; j < 50; ++j) src[j] = j;
		for (int i = 0; i < 10000; ++i) {
			Array<int> copy(src);
			(void)copy;
		}
		expect(true, "10000 copy cycles OK");
	}

	// === 23. Array<Array<int>> (nested template) — 動作は簡易確認 ===
	section("23. nested template: Array<Array<int> >");
	{
		Array<Array<int> > outer(3);
		expect(outer.size() == 3, "outer size 3");
		expect(outer[0].size() == 0, "inner Array<int> default constructed (empty)");
	}

	// === 24. Array<int>(0): explicit zero size ===
	section("24. Array<int>(0) is valid empty array");
	{
		Array<int> a(0);
		expect(a.size() == 0, "size 0");
		bool caught = false;
		try { a[0] = 0; } catch (const std::exception&) { caught = true; }
		expect(caught, "any access throws");
	}

	// === 25. assignment: 大→小、小→大 でサイズも更新 ===
	section("25. assignment adjusts internal size correctly");
	{
		Array<int> big(100);
		Array<int> small(3);
		small[0] = 1; small[1] = 2; small[2] = 3;
		big = small;
		expect(big.size() == 3, "big now size 3");
		expect(big[0] == 1 && big[2] == 3, "content copied");
		bool caught = false;
		try { big[3] = 0; } catch (const std::exception&) { caught = true; }
		expect(caught, "big[3] now out of bounds");
	}
	{
		Array<int> src(5);
		for (unsigned int i = 0; i < 5; ++i) src[i] = static_cast<int>(i);
		Array<int> dst;
		dst = src;
		expect(dst.size() == 5, "empty -> resized");
	}

	// === 26. カスタム class T: Array<Counted>(n) は n 個 ctor + n 個 dtor ===
	section("26. Array<Counted>(n) constructs and destroys exactly n times");
	{
		g_ctor = g_dtor = g_copy = g_assign = 0;
		{
			Array<Counted> arr(5);
			expect(g_ctor == 5, "5 ctors on construction");
			expect(g_dtor == 0, "0 dtors yet");
		}
		expect(g_dtor == 5, "5 dtors on destruction");
	}

	// === 27. Copy ctor: default-init n 個 + assign n 個 ===
	// (実装: _data = new T[n]() → 4 default ctor, その後ループで operator= → 4 assign)
	section("27. Array<Counted> copy ctor: n default ctors + n assigns");
	{
		g_ctor = g_dtor = g_copy = g_assign = 0;
		{
			Array<Counted> orig(4);
			int ctor_before_copy = g_ctor;
			Array<Counted> cpy(orig);
			expect(g_ctor == ctor_before_copy + 4, "4 new default ctors for new[] allocation");
			expect(g_assign == 4, "4 copy assigns to fill new[] from source");
		}
		expect(g_dtor == 8, "8 dtors (both arrays destroyed)");
	}

	// === 28. 関数への値渡し: コピー発生・呼び出し元は変更されない ===
	section("28. pass Array by value -> caller not modified");
	{
		Array<int> orig(3);
		orig[0] = 10; orig[1] = 20; orig[2] = 30;
		Array<int> result = passByValue(orig);
		expect(orig[0] == 10, "orig[0] unchanged (was 10)");
		expect(result[0] == -999, "returned array has modified value");
		expect(orig[1] == 20 && orig[2] == 30, "orig[1,2] unchanged");
	}

	// === 29. Return by value from function ===
	section("29. return Array<int> by value from function");
	{
		Array<int> r = makeArray(5);
		expect(r.size() == 5, "returned array size 5");
		expect(r[0] == 100 && r[4] == 104, "returned array values");
	}

	// === 30. Array<char> ===
	section("30. Array<char>");
	{
		Array<char> a(5);
		a[0] = 'H'; a[1] = 'e'; a[2] = 'l'; a[3] = 'l'; a[4] = 'o';
		expect(a[0] == 'H' && a[4] == 'o', "Array<char> works");
	}

	// === 31. Array<bool> ===
	section("31. Array<bool>");
	{
		Array<bool> a(4);
		expect(a[0] == false && a[3] == false, "bool default false");
		a[0] = true; a[2] = true;
		expect(a[0] == true && a[1] == false && a[2] == true && a[3] == false, "bool set/get");
	}

	// === 32. Chained subscript operations ===
	section("32. chained subscript operations");
	{
		Array<int> a(3);
		a[0] = 10;
		a[0]++;
		++a[0];
		a[0] += 3;
		expect(a[0] == 15, "10 -> 11 -> 12 -> 15");
	}

	// === 33. Very large array (100,000 int) ===
	section("33. very large Array<int> (100,000 elements)");
	{
		Array<int> a(100000);
		for (unsigned int i = 0; i < 100000; ++i) a[i] = static_cast<int>(i);
		bool ok = true;
		for (unsigned int i = 0; i < 100000; ++i)
			if (a[i] != static_cast<int>(i)) { ok = false; break; }
		expect(ok, "100000 elements set/get correctly");
		expect(a.size() == 100000, "size == 100000");
	}

	// === 34. what() メッセージが期待通り ===
	section("34. OutOfBoundsException::what() returns expected message");
	{
		Array<int> a(3);
		try { a[5] = 0; }
		catch (const std::exception& e) {
			std::string msg = e.what();
			expect(msg.find("out of bounds") != std::string::npos,
			       "what() message contains 'out of bounds'");
		}
	}

	// === 35. 同じ Array で複数回 exception cycle → 状態保持 ===
	section("35. multiple exception cycles preserve Array state");
	{
		Array<int> a(3);
		a[0] = 1; a[1] = 2; a[2] = 3;
		for (int i = 0; i < 100; ++i) {
			try { a[100] = 0; }
			catch (const std::exception&) {}
		}
		expect(a[0] == 1 && a[1] == 2 && a[2] == 3, "state after 100 exceptions preserved");
	}

	// === 36. Const Array にも size() が呼べる ===
	section("36. size() and const [] on const Array");
	{
		Array<int> src(5);
		for (unsigned int i = 0; i < 5; ++i) src[i] = static_cast<int>(i * i);
		const Array<int>& cref = src;
		expect(cref.size() == 5, "size() on const ref");
		expect(cref[3] == 9, "const [3] == 9");
	}

	// === 37. コピー独立性: source の各要素を全て変更しても copy は不変 ===
	section("37. exhaustive copy independence");
	{
		Array<int> src(10);
		for (unsigned int i = 0; i < 10; ++i) src[i] = static_cast<int>(i);
		Array<int> cpy(src);
		for (unsigned int i = 0; i < 10; ++i) src[i] = -1;
		bool copy_intact = true;
		for (unsigned int i = 0; i < 10; ++i)
			if (cpy[i] != static_cast<int>(i)) { copy_intact = false; break; }
		expect(copy_intact, "copy still has original 0..9");
	}

	// === 38. Custom class T + operator= 経由 ===
	section("38. Array<Counted> operator= correctness");
	{
		g_ctor = g_dtor = g_copy = g_assign = 0;
		{
			Array<Counted> src(3);
			Array<Counted> dst(5);
			dst = src;
			expect(dst.size() == 3, "size updated to 3");
		}
		expect(g_ctor + g_copy == g_dtor,
		       "total constructions (ctor + copy) equals destructions");
	}

	// === 39. Consecutive assignment chain ===
	section("39. consecutive operator= chain");
	{
		Array<int> a(2), b(3), c(4);
		c[0] = 100;
		b = c;
		a = b;
		expect(a.size() == 4 && b.size() == 4 && a[0] == 100, "chain a=b=c");
	}

	// === 40. Fast throw: 10,000 out-of-bounds catches ===
	section("40. 10,000 out-of-bounds catch cycles (leak safety)");
	{
		Array<int> a(3);
		int caught = 0;
		for (int i = 0; i < 10000; ++i) {
			try { a[100] = 0; }
			catch (const std::exception&) { ++caught; }
		}
		expect(caught == 10000, "all 10000 exceptions caught");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
