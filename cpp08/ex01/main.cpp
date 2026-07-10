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

	// === 25. C-style array を addNumbers に渡す ===
	section("25. addNumbers with C-style array via pointer iterators");
	{
		Span sp(5);
		int arr[] = {100, 50, 200, 25, 300};
		sp.addNumbers(arr, arr + 5);
		expect(sp.size() == 5, "5 elements added");
		expect(sp.longestSpan() == 275, "300 - 25 == 275");
		expect(sp.shortestSpan() == 25, "50 - 25 == 25 shortest");
	}

	// === 26. addNumber を throw させても state 保持 ===
	section("26. failed addNumber preserves state");
	{
		Span sp(2);
		sp.addNumber(10);
		sp.addNumber(20);
		int shortest_before = sp.shortestSpan();
		try { sp.addNumber(30); }
		catch (const std::exception&) {}
		expect(sp.size() == 2, "size still 2 after failed add");
		expect(sp.shortestSpan() == shortest_before, "computation still same");
	}

	// === 27. addNumber と addNumbers の混在 ===
	section("27. mixed addNumber + addNumbers");
	{
		Span sp(5);
		sp.addNumber(1);
		std::vector<int> mid;
		mid.push_back(2); mid.push_back(3);
		sp.addNumbers(mid.begin(), mid.end());
		sp.addNumber(4);
		sp.addNumber(5);
		expect(sp.size() == 5, "5 elements");
		expect(sp.shortestSpan() == 1, "consecutive -> 1");
		expect(sp.longestSpan() == 4, "5 - 1 == 4");
	}

	// === 28. shortest/longest 呼び出し冪等 ===
	section("28. shortest/longest calls are idempotent");
	{
		Span sp(10);
		for (int i = 0; i < 10; ++i) sp.addNumber(i * 3);
		int s1 = sp.shortestSpan();
		int s2 = sp.shortestSpan();
		int l1 = sp.longestSpan();
		int l2 = sp.longestSpan();
		expect(s1 == s2 && l1 == l2, "results deterministic across calls");
	}

	// === 29. Reverse order の値: shortest はソート後に決定 ===
	section("29. reverse-ordered values: shortest computed on sorted");
	{
		Span sp(5);
		sp.addNumber(50);
		sp.addNumber(40);
		sp.addNumber(30);
		sp.addNumber(20);
		sp.addNumber(10);
		expect(sp.shortestSpan() == 10, "sorted min diff == 10");
		expect(sp.longestSpan() == 40, "50 - 10 == 40");
	}

	// === 30. Full 状態でも shortest/longest は動く ===
	section("30. full Span can still compute spans");
	{
		Span sp(3);
		sp.addNumber(1); sp.addNumber(2); sp.addNumber(3);
		expect(sp.shortestSpan() == 1, "full: shortest works");
		expect(sp.longestSpan() == 2, "full: longest works");
	}

	// === 31. 巨大な値の shortest (both positive edge) ===
	section("31. large positive values shortest");
	{
		Span sp(3);
		sp.addNumber(1000000);
		sp.addNumber(1000001);
		sp.addNumber(2000000);
		expect(sp.shortestSpan() == 1, "1M -> 1M+1 == diff 1");
		expect(sp.longestSpan() == 1000000, "1M diff");
	}

	// === 32. Only-negative values ===
	section("32. all negative values");
	{
		Span sp(4);
		sp.addNumber(-1000);
		sp.addNumber(-500);
		sp.addNumber(-100);
		sp.addNumber(-1);
		expect(sp.longestSpan() == 999, "-1000 to -1 == 999");
		expect(sp.shortestSpan() == 99, "smallest diff -100 to -1 == 99");
	}

	// === 33. addNumbers with mixed containers concatenated ===
	section("33. addNumbers called multiple times");
	{
		Span sp(6);
		std::vector<int> v1; v1.push_back(1); v1.push_back(2);
		std::vector<int> v2; v2.push_back(10); v2.push_back(20);
		std::list<int> l3; l3.push_back(100); l3.push_back(200);
		sp.addNumbers(v1.begin(), v1.end());
		sp.addNumbers(v2.begin(), v2.end());
		sp.addNumbers(l3.begin(), l3.end());
		expect(sp.size() == 6, "6 total elements");
		expect(sp.longestSpan() == 199, "200 - 1 == 199");
	}

	// === 34. Copy 経由の同一性: shortest/longest 一致 ===
	section("34. copy preserves shortest/longest values");
	{
		Span a(5);
		for (int i = 0; i < 5; ++i) a.addNumber(i * 7);
		Span b(a);
		expect(a.shortestSpan() == b.shortestSpan(), "shortest match");
		expect(a.longestSpan() == b.longestSpan(), "longest match");
	}

	// === 35. operator= chain ===
	section("35. operator= chain a = b = c");
	{
		Span a(5), b(5), c(5);
		for (int i = 0; i < 3; ++i) c.addNumber(i);
		a = b = c;
		expect(a.size() == 3 && b.size() == 3, "all size 3");
		expect(a.shortestSpan() == b.shortestSpan(), "shortest match after chain");
	}

	// === 36. shortestSpan/longestSpan は const method ===
	section("36. shortest/longest are const-callable");
	{
		Span sp(3);
		sp.addNumber(1); sp.addNumber(2); sp.addNumber(3);
		const Span& cref = sp;
		expect(cref.shortestSpan() == 1, "const shortest");
		expect(cref.longestSpan() == 2, "const longest");
		expect(cref.size() == 3, "const size");
		expect(cref.capacity() == 3, "const capacity");
	}

	// === 37. Very large: 1,000,000 elements ===
	section("37. Span with 1,000,000 elements (extreme)");
	{
		Span sp(1000000);
		std::vector<int> src;
		src.reserve(1000000);
		for (int i = 0; i < 1000000; ++i) src.push_back(i);
		sp.addNumbers(src.begin(), src.end());
		expect(sp.size() == 1000000, "1M elements added");
		expect(sp.shortestSpan() == 1, "consecutive -> 1");
		expect(sp.longestSpan() == 999999, "1M - 1");
	}

	// === 38. Same Span reused across capacity ===
	section("38. Span assignment to different capacity");
	{
		Span original(3);
		original.addNumber(10); original.addNumber(20); original.addNumber(30);
		Span target(100);
		for (int i = 0; i < 50; ++i) target.addNumber(i);
		target = original;
		expect(target.size() == 3 && target.capacity() == 3, "target now = original");
		expect(target.longestSpan() == 20, "target longest = 20");
	}

	// === 39. addNumbers 空 range 何度呼んでも no-op ===
	section("39. multiple empty addNumbers calls are no-op");
	{
		Span sp(3);
		sp.addNumber(1);
		std::vector<int> empty_src;
		for (int i = 0; i < 100; ++i) {
			sp.addNumbers(empty_src.begin(), empty_src.end());
		}
		expect(sp.size() == 1, "size still 1 after 100 empty adds");
	}

	// === 40. shortestSpan 2要素 ===
	section("40. shortestSpan with exactly 2 identical elements");
	{
		Span sp(2);
		sp.addNumber(5); sp.addNumber(5);
		expect(sp.shortestSpan() == 0, "identical -> 0");
		expect(sp.longestSpan() == 0, "identical -> 0");
	}

	// === 41. shortestSpan sorted vs random order 一致 ===
	section("41. shortestSpan is order-independent (sorted vs shuffled input)");
	{
		Span sorted(5);
		Span shuffled(5);
		sorted.addNumber(1); sorted.addNumber(2); sorted.addNumber(4);
		sorted.addNumber(7); sorted.addNumber(11);
		shuffled.addNumber(7); shuffled.addNumber(1); shuffled.addNumber(11);
		shuffled.addNumber(2); shuffled.addNumber(4);
		expect(sorted.shortestSpan() == shuffled.shortestSpan(), "shortest equal");
		expect(sorted.longestSpan() == shuffled.longestSpan(), "longest equal");
	}

	// === 42. Deep sequential lifecycle ===
	section("42. sequential lifecycle: add -> compute -> more -> compute");
	{
		Span sp(10);
		sp.addNumber(1); sp.addNumber(100);
		int l1 = sp.longestSpan();
		sp.addNumber(50);
		int l2 = sp.longestSpan();
		sp.addNumber(1000);
		int l3 = sp.longestSpan();
		expect(l1 == 99, "1 to 100 -> 99");
		expect(l2 == 99, "1 to 100 still -> 99");
		expect(l3 == 999, "1 to 1000 -> 999");
	}

	// === 43. Full Span への追加 → throw 後 state 完全維持 ===
	section("43. full Span throws + state fully preserved");
	{
		Span sp(3);
		sp.addNumber(10); sp.addNumber(20); sp.addNumber(30);
		int prev_short = sp.shortestSpan();
		int prev_long = sp.longestSpan();
		for (int i = 0; i < 100; ++i) {
			try { sp.addNumber(999); }
			catch (const std::exception&) {}
		}
		expect(sp.size() == 3, "size still 3 after 100 fail adds");
		expect(sp.shortestSpan() == prev_short, "shortest unchanged");
		expect(sp.longestSpan() == prev_long, "longest unchanged");
	}

	// === 44. addNumbers 自身への iterator は unsafe だがテストしない ===
	section("44. addNumbers with vector<int>::iterator external safe");
	{
		Span sp(5);
		std::vector<int> src;
		src.push_back(1); src.push_back(3); src.push_back(5); src.push_back(7); src.push_back(9);
		sp.addNumbers(src.begin(), src.end());
		expect(sp.size() == 5, "5 added");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
