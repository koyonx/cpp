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

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
