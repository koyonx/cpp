#include "Span.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <climits>
#include <cstdlib>
#include <ctime>

static int g_pass = 0;
static int g_fail = 0;

static void section(const std::string& t) {
	std::cout << "\n=== " << t << " ===" << std::endl;
}
static void expect(bool cond, const std::string& what) {
	if (cond) { ++g_pass; std::cout << "  [PASS] " << what << std::endl; }
	else      { ++g_fail; std::cout << "  [FAIL] " << what << std::endl; }
}

int main() {
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// === 1. PDF 例そのまま ===
	section("1. PDF example: Span(5) with {6,3,17,9,11} -> shortest=2, longest=14");
	{
		Span sp = Span(5);
		sp.addNumber(6);
		sp.addNumber(3);
		sp.addNumber(17);
		sp.addNumber(9);
		sp.addNumber(11);
		expect(sp.shortestSpan() == 2, "shortestSpan == 2");
		expect(sp.longestSpan() == 14, "longestSpan == 14");
	}

	// === 2. addNumber 満杯で throw ===
	section("2. addNumber throws when full");
	{
		Span sp(3);
		sp.addNumber(1);
		sp.addNumber(2);
		sp.addNumber(3);
		bool caught = false;
		try { sp.addNumber(4); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "4th addNumber on Span(3) throws");
	}

	// === 3. shortestSpan / longestSpan: 要素 0 → throw ===
	section("3. shortest/longest throw on empty Span");
	{
		Span sp(5);
		bool caught_s = false, caught_l = false;
		try { sp.shortestSpan(); } catch (const std::exception&) { caught_s = true; }
		try { sp.longestSpan(); }  catch (const std::exception&) { caught_l = true; }
		expect(caught_s && caught_l, "both throw on empty");
	}

	// === 4. shortestSpan / longestSpan: 要素 1 → throw ===
	section("4. shortest/longest throw on single-element Span");
	{
		Span sp(5);
		sp.addNumber(42);
		bool caught_s = false, caught_l = false;
		try { sp.shortestSpan(); } catch (const std::exception&) { caught_s = true; }
		try { sp.longestSpan(); }  catch (const std::exception&) { caught_l = true; }
		expect(caught_s && caught_l, "both throw with 1 element");
	}

	// === 5. 2 要素の最小値: shortest == longest ===
	section("5. exactly 2 elements: shortest == longest");
	{
		Span sp(2);
		sp.addNumber(5);
		sp.addNumber(10);
		expect(sp.shortestSpan() == 5, "5..10 shortest = 5");
		expect(sp.longestSpan() == 5, "5..10 longest = 5");
	}

	// === 6. 同じ値のみ: shortest = 0 ===
	section("6. duplicates: shortest = 0");
	{
		Span sp(5);
		sp.addNumber(7);
		sp.addNumber(7);
		sp.addNumber(7);
		expect(sp.shortestSpan() == 0, "same value -> 0");
		expect(sp.longestSpan() == 0, "same value -> 0");
	}

	// === 7. 負の値を含む ===
	section("7. negative values");
	{
		Span sp(3);
		sp.addNumber(-10);
		sp.addNumber(-5);
		sp.addNumber(0);
		expect(sp.shortestSpan() == 5, "shortest = 5");
		expect(sp.longestSpan() == 10, "longest = 10 (0 - -10)");
	}

	// === 8. INT_MIN / INT_MAX 混在 (overflow 対策の検証) ===
	section("8. INT_MIN and INT_MAX values (overflow-safe long arithmetic)");
	{
		Span sp(3);
		sp.addNumber(INT_MIN);
		sp.addNumber(0);
		sp.addNumber(INT_MAX);
		int L = sp.longestSpan();
		// INT_MAX - INT_MIN は long で 4294967295, int に clamp すると INT_MAX
		expect(L == INT_MAX, "long-arithmetic clamps to INT_MAX");
	}

	// === 9. addNumbers with iterator range (vector) ===
	section("9. addNumbers with vector iterator range");
	{
		Span sp(5);
		std::vector<int> src;
		src.push_back(6); src.push_back(3); src.push_back(17);
		src.push_back(9); src.push_back(11);
		sp.addNumbers(src.begin(), src.end());
		expect(sp.size() == 5, "size == 5 after batch add");
		expect(sp.shortestSpan() == 2 && sp.longestSpan() == 14, "same PDF result");
	}

	// === 10. addNumbers with list iterator range ===
	section("10. addNumbers with std::list");
	{
		Span sp(4);
		std::list<int> src;
		src.push_back(100); src.push_back(200); src.push_back(300); src.push_back(400);
		sp.addNumbers(src.begin(), src.end());
		expect(sp.size() == 4, "size == 4");
		expect(sp.longestSpan() == 300, "400 - 100 == 300");
	}

	// === 11. addNumbers 範囲が capacity 超過 → throw ===
	section("11. addNumbers throws when range exceeds capacity");
	{
		Span sp(3);
		std::vector<int> src;
		src.push_back(1); src.push_back(2); src.push_back(3); src.push_back(4);
		bool caught = false;
		try { sp.addNumbers(src.begin(), src.end()); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "4-elem into Span(3) throws");
		expect(sp.size() == 0, "size unchanged after throw");
	}

	// === 12. addNumbers 部分挿入後の残り超過 → throw ===
	section("12. addNumbers throws when combined with existing exceeds capacity");
	{
		Span sp(5);
		sp.addNumber(1);
		sp.addNumber(2);
		std::vector<int> src;
		src.push_back(3); src.push_back(4); src.push_back(5); src.push_back(6);
		bool caught = false;
		try { sp.addNumbers(src.begin(), src.end()); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "2 existing + 4 new > 5 capacity throws");
		expect(sp.size() == 2, "size still 2 (transaction rolled back)");
	}

	// === 13. Span(10,000) with random data ===
	section("13. Span with 10,000 random numbers");
	{
		Span sp(10000);
		std::vector<int> src;
		src.reserve(10000);
		for (int i = 0; i < 10000; ++i) src.push_back(std::rand());
		sp.addNumbers(src.begin(), src.end());
		expect(sp.size() == 10000, "10,000 numbers added");
		int s = sp.shortestSpan();
		int l = sp.longestSpan();
		expect(s >= 0, "shortestSpan >= 0");
		expect(l >= s, "longestSpan >= shortestSpan");
	}

	// === 14. Span(100,000) large ===
	section("14. Span(100,000) large-scale");
	{
		Span sp(100000);
		std::vector<int> src;
		src.reserve(100000);
		for (int i = 0; i < 100000; ++i) src.push_back(i);
		sp.addNumbers(src.begin(), src.end());
		expect(sp.size() == 100000, "100000 numbers");
		expect(sp.shortestSpan() == 1, "consecutive ints -> shortest = 1");
		expect(sp.longestSpan() == 99999, "longest = 99999");
	}

	// === 15. Copy constructor ===
	section("15. copy constructor produces independent copy");
	{
		Span a(5);
		a.addNumber(1); a.addNumber(2); a.addNumber(3);
		Span b(a);
		expect(b.size() == 3, "size copied");
		expect(b.longestSpan() == 2, "content copied");
		a.addNumber(100);
		expect(b.size() == 3, "b unchanged when a modified");
	}

	// === 16. operator= ===
	section("16. operator= copies");
	{
		Span a(5);
		a.addNumber(10); a.addNumber(20);
		Span b(3);
		b.addNumber(999);
		b = a;
		expect(b.size() == 2 && b.capacity() == 5, "b has a's state");
		expect(b.longestSpan() == 10, "b longest = 10");
	}

	// === 17. Self-assignment ===
	section("17. self-assignment safe");
	{
		Span sp(5);
		sp.addNumber(3); sp.addNumber(7);
		sp = sp;
		expect(sp.size() == 2 && sp.longestSpan() == 4, "state preserved");
	}

	// === 18. Span(0): 何も追加できない ===
	section("18. Span(0) is degenerate");
	{
		Span sp(0);
		expect(sp.capacity() == 0, "capacity 0");
		bool caught = false;
		try { sp.addNumber(1); } catch (const std::exception&) { caught = true; }
		expect(caught, "any addNumber throws");
	}

	// === 19. Span(1): 追加は 1 個だけ、span 計算不可 ===
	section("19. Span(1)");
	{
		Span sp(1);
		sp.addNumber(5);
		expect(sp.size() == 1, "1 element added");
		bool caught_s = false, caught_l = false;
		try { sp.shortestSpan(); } catch (const std::exception&) { caught_s = true; }
		try { sp.longestSpan(); }  catch (const std::exception&) { caught_l = true; }
		expect(caught_s && caught_l, "span calculations throw with size 1");
	}

	// === 20. addNumbers 空範囲 → 変化なし ===
	section("20. addNumbers with empty range is no-op");
	{
		Span sp(5);
		sp.addNumber(1);
		std::vector<int> empty_src;
		sp.addNumbers(empty_src.begin(), empty_src.end());
		expect(sp.size() == 1, "size unchanged for empty range");
	}

	// === 21. capacity() 保持 ===
	section("21. capacity() reflects constructor arg");
	{
		Span sp(42);
		expect(sp.capacity() == 42, "capacity == 42");
		sp.addNumber(1);
		expect(sp.capacity() == 42, "capacity unchanged after add");
	}

	// === 22. Full 状態からの回復 (copy 後) ===
	section("22. copy of full Span still full");
	{
		Span a(2);
		a.addNumber(1); a.addNumber(2);
		Span b(a);
		bool caught = false;
		try { b.addNumber(3); } catch (const std::exception&) { caught = true; }
		expect(caught, "copy is also full");
	}

	// === 23. Leak safety: 1000 Span lifecycles ===
	section("23. 1000 Span lifecycles (leak safety)");
	{
		for (int i = 0; i < 1000; ++i) {
			Span sp(100);
			for (int j = 0; j < 100; ++j) sp.addNumber(std::rand());
			(void)sp.shortestSpan();
			(void)sp.longestSpan();
		}
		expect(true, "1000 lifecycles no crash");
	}

	// === 24. addNumbers with deque ===
	section("24. addNumbers with std::deque");
	{
		Span sp(3);
		std::deque<int> d;
		d.push_back(50); d.push_back(60); d.push_back(70);
		sp.addNumbers(d.begin(), d.end());
		expect(sp.shortestSpan() == 10 && sp.longestSpan() == 20, "deque range added");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
