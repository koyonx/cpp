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

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
