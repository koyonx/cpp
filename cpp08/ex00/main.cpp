#include "easyfind.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <deque>
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

int main() {
	// === 1. std::vector<int>: 存在する値 ===
	section("1. vector: found returns iterator to first occurrence");
	{
		std::vector<int> v;
		v.push_back(1); v.push_back(2); v.push_back(3); v.push_back(4); v.push_back(5);
		std::vector<int>::iterator it = easyfind(v, 3);
		expect(it != v.end(), "iterator not end");
		expect(*it == 3, "dereferenced value == 3");
		expect(it - v.begin() == 2, "position index == 2");
	}

	// === 2. std::vector<int>: 存在しない値 → throw ===
	section("2. vector: not found throws");
	{
		std::vector<int> v;
		v.push_back(1); v.push_back(2);
		bool caught = false;
		try { easyfind(v, 999); }
		catch (const std::exception& e) {
			caught = true;
			expect(std::string(e.what()).find("not found") != std::string::npos, "what() contains 'not found'");
		}
		expect(caught, "exception thrown for missing value");
	}

	// === 3. Empty vector → throw ===
	section("3. empty vector throws");
	{
		std::vector<int> v;
		bool caught = false;
		try { easyfind(v, 0); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "empty vector throws");
	}

	// === 4. std::list<int> ===
	section("4. list: found returns iterator");
	{
		std::list<int> l;
		l.push_back(10); l.push_back(20); l.push_back(30);
		std::list<int>::iterator it = easyfind(l, 20);
		expect(*it == 20, "list find works");
	}

	// === 5. std::deque<int> ===
	section("5. deque: found returns iterator");
	{
		std::deque<int> d;
		d.push_back(-1); d.push_back(0); d.push_back(1);
		std::deque<int>::iterator it = easyfind(d, -1);
		expect(*it == -1, "deque find works");
		expect(it == d.begin(), "iterator at begin");
	}

	// === 6. 重複値: 最初の occurrence を返す ===
	section("6. duplicate values: returns first occurrence");
	{
		std::vector<int> v;
		v.push_back(5); v.push_back(3); v.push_back(5); v.push_back(3);
		std::vector<int>::iterator it = easyfind(v, 3);
		expect(it - v.begin() == 1, "first '3' at index 1");
	}

	// === 7. 先頭要素 ===
	section("7. find at position 0 (first element)");
	{
		std::vector<int> v;
		v.push_back(42); v.push_back(100);
		std::vector<int>::iterator it = easyfind(v, 42);
		expect(it == v.begin(), "iterator at begin()");
	}

	// === 8. 末尾要素 ===
	section("8. find at last position");
	{
		std::vector<int> v;
		v.push_back(1); v.push_back(2); v.push_back(3);
		std::vector<int>::iterator it = easyfind(v, 3);
		expect(it + 1 == v.end(), "iterator at last valid");
	}

	// === 9. 負の値 ===
	section("9. find negative value");
	{
		std::vector<int> v;
		v.push_back(-5); v.push_back(-3); v.push_back(0);
		std::vector<int>::iterator it = easyfind(v, -3);
		expect(*it == -3, "negative value found");
	}

	// === 10. INT_MAX / INT_MIN ===
	section("10. find INT_MAX / INT_MIN");
	{
		std::vector<int> v;
		v.push_back(0);
		v.push_back(INT_MIN);
		v.push_back(INT_MAX);
		std::vector<int>::iterator it_max = easyfind(v, INT_MAX);
		std::vector<int>::iterator it_min = easyfind(v, INT_MIN);
		expect(*it_max == INT_MAX, "INT_MAX found");
		expect(*it_min == INT_MIN, "INT_MIN found");
	}

	// === 11. const container ===
	section("11. const container -> const_iterator");
	{
		std::vector<int> v;
		v.push_back(7); v.push_back(8); v.push_back(9);
		const std::vector<int>& cref = v;
		std::vector<int>::const_iterator it = easyfind(cref, 8);
		expect(*it == 8, "const overload works");
	}

	// === 12. Iterator を経由して修正できる (非-const 版) ===
	section("12. non-const iterator allows modification");
	{
		std::vector<int> v;
		v.push_back(1); v.push_back(2); v.push_back(3);
		std::vector<int>::iterator it = easyfind(v, 2);
		*it = 999;
		expect(v[1] == 999, "modified through returned iterator");
	}

	// === 13. Large container ===
	section("13. 10,000-element vector find");
	{
		std::vector<int> v;
		for (int i = 0; i < 10000; ++i) v.push_back(i);
		std::vector<int>::iterator it = easyfind(v, 9999);
		expect(*it == 9999, "found last element in 10000-vec");
	}

	// === 14. std::list is std::vector と同じ挙動 (linear time) ===
	section("14. list vs vector: same semantic");
	{
		std::list<int> l;
		std::vector<int> v;
		for (int i = 0; i < 100; ++i) {
			l.push_back(i);
			v.push_back(i);
		}
		std::list<int>::iterator lit = easyfind(l, 50);
		std::vector<int>::iterator vit = easyfind(v, 50);
		expect(*lit == 50 && *vit == 50, "both find 50");
	}

	// === 15. std::runtime_error として catch できる ===
	section("15. exception is std::runtime_error type");
	{
		std::vector<int> v;
		v.push_back(1);
		bool caught = false;
		try { easyfind(v, 999); }
		catch (const std::runtime_error&) { caught = true; }
		expect(caught, "catchable as std::runtime_error&");
	}

	// === 16. Leak safety: 10,000 iterations ===
	section("16. 10,000 easyfind iterations (leak safety)");
	{
		std::vector<int> v;
		for (int i = 0; i < 100; ++i) v.push_back(i);
		int found = 0, not_found = 0;
		for (int i = 0; i < 10000; ++i) {
			try {
				easyfind(v, i % 200);
				++found;
			} catch (const std::exception&) {
				++not_found;
			}
		}
		expect(found + not_found == 10000, "all 10000 handled");
		expect(found == 5000, "5000 successful");
	}

	// === 17. 1 要素の container ===
	section("17. single-element container");
	{
		std::vector<int> v;
		v.push_back(42);
		expect(*easyfind(v, 42) == 42, "found");
		bool caught = false;
		try { easyfind(v, 43); } catch (const std::exception&) { caught = true; }
		expect(caught, "not found throws");
	}

	// === 18. exception message 内容の厳密検証 ===
	section("18. exception message exact content");
	{
		std::vector<int> v;
		v.push_back(1);
		try {
			easyfind(v, 0);
			expect(false, "should have thrown");
		} catch (const std::exception& e) {
			std::string msg = e.what();
			expect(msg == "easyfind: value not found", "exact message match");
		}
	}

	// === 19. iterator 算術: found 後の it+n, distance ===
	section("19. iterator arithmetic after found (vector: random access)");
	{
		std::vector<int> v;
		for (int i = 0; i < 10; ++i) v.push_back(i * 10);
		std::vector<int>::iterator it = easyfind(v, 30);
		expect(*(it + 2) == 50, "it+2 -> 50");
		expect(it - v.begin() == 3, "distance from begin == 3");
		expect(v.end() - it == 7, "distance to end == 7");
	}

	// === 20. std::advance / std::distance on list iterator (bidirectional) ===
	section("20. list iterator + std::advance/std::distance");
	{
		std::list<int> l;
		for (int i = 0; i < 10; ++i) l.push_back(i);
		std::list<int>::iterator it = easyfind(l, 5);
		expect(std::distance(l.begin(), it) == 5, "distance == 5");
		std::advance(it, 2);
		expect(*it == 7, "advance +2 -> 7");
	}

	// === 21. Container 不変性 (const-correctness): easyfind は container を変えない ===
	section("21. easyfind does not modify container");
	{
		std::vector<int> v;
		v.push_back(1); v.push_back(2); v.push_back(3);
		std::vector<int> original(v);
		easyfind(v, 2);
		expect(v == original, "vector unchanged after find");
	}

	// === 22. All-duplicates container ===
	section("22. container of all duplicates");
	{
		std::vector<int> v(100, 7);  // 100 copies of 7
		std::vector<int>::iterator it = easyfind(v, 7);
		expect(it == v.begin(), "first occurrence at begin");
		expect(*(it + 50) == 7, "still 7 at +50");
	}

	// === 23. only-one-match: 存在するが 1 個だけ ===
	section("23. only one occurrence in large container");
	{
		std::vector<int> v(1000, 0);
		v[500] = 42;
		std::vector<int>::iterator it = easyfind(v, 42);
		expect(it - v.begin() == 500, "found at position 500");
	}

	// === 24. Reserved but not-yet-filled vector ===
	section("24. vector with reserved capacity");
	{
		std::vector<int> v;
		v.reserve(1000);
		for (int i = 0; i < 10; ++i) v.push_back(i);
		expect(*easyfind(v, 5) == 5, "found despite over-reserved capacity");
	}

	// === 25. Insert/erase 後の container ===
	section("25. easyfind on modified container");
	{
		std::vector<int> v;
		for (int i = 0; i < 10; ++i) v.push_back(i);
		v.insert(v.begin() + 5, 999);
		expect(*easyfind(v, 999) == 999, "inserted element found");
		v.erase(v.begin() + 5);
		bool caught = false;
		try { easyfind(v, 999); } catch (const std::exception&) { caught = true; }
		expect(caught, "erased element not found");
	}

	// === 26. Value == 0 (境界的) ===
	section("26. value 0 handling");
	{
		std::vector<int> v;
		v.push_back(-1); v.push_back(0); v.push_back(1);
		expect(*easyfind(v, 0) == 0, "0 found");
	}

	// === 27. std::deque with wraparound (many push_back/pop_front) ===
	section("27. std::deque after many push_back/pop_front cycles");
	{
		std::deque<int> d;
		for (int i = 0; i < 1000; ++i) d.push_back(i);
		for (int i = 0; i < 500; ++i) d.pop_front();
		// Now d contains 500..999
		std::deque<int>::iterator it = easyfind(d, 750);
		expect(*it == 750, "deque wraparound find works");
	}

	// === 28. 100,000-element large container ===
	section("28. 100,000-element container");
	{
		std::vector<int> v;
		v.reserve(100000);
		for (int i = 0; i < 100000; ++i) v.push_back(i);
		expect(*easyfind(v, 99999) == 99999, "last element found");
		expect(*easyfind(v, 0) == 0, "first element found");
		expect(*easyfind(v, 50000) == 50000, "middle element found");
	}

	// === 29. Same easyfind 何度も呼んでも状態変わらず ===
	section("29. multiple easyfind calls are deterministic");
	{
		std::vector<int> v;
		v.push_back(10); v.push_back(20); v.push_back(30);
		std::vector<int>::iterator a = easyfind(v, 20);
		std::vector<int>::iterator b = easyfind(v, 20);
		std::vector<int>::iterator c = easyfind(v, 20);
		expect(a == b && b == c, "3 calls return same iterator");
	}

	// === 30. std::list with negative values ===
	section("30. list with negative values");
	{
		std::list<int> l;
		l.push_back(-100); l.push_back(-50); l.push_back(0); l.push_back(50);
		std::list<int>::iterator it = easyfind(l, -50);
		expect(*it == -50, "found -50 in list");
	}

	// === 31. Not-found 例外に依存する制御フローの正しさ ===
	section("31. control flow using not-found exception");
	{
		std::vector<int> v;
		v.push_back(1); v.push_back(3); v.push_back(5);
		int missing_count = 0;
		int found_count = 0;
		for (int target = 0; target < 10; ++target) {
			try {
				easyfind(v, target);
				++found_count;
			} catch (const std::exception&) {
				++missing_count;
			}
		}
		expect(found_count == 3, "3 found (1, 3, 5)");
		expect(missing_count == 7, "7 missing");
	}

	// === 32. found iterator を経由した書き込みが container に反映 ===
	section("32. write via found iterator persists");
	{
		std::vector<int> v;
		for (int i = 0; i < 5; ++i) v.push_back(i * 10);
		*easyfind(v, 20) = -1;
		expect(v[2] == -1, "index 2 == -1 after write");
	}

	// === 33. easyfind 経由の iterator は container に紐づく ===
	section("33. found iterator distance from begin");
	{
		std::vector<int> v;
		for (int i = 0; i < 100; ++i) v.push_back(i);
		std::vector<int>::iterator it = easyfind(v, 42);
		expect(std::distance(v.begin(), it) == 42, "distance == 42");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
