#include "MutantStack.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

static int g_pass = 0;
static int g_fail = 0;

static void section(const std::string& t) {
	std::cout << "\n=== " << t << " ===" << std::endl;
}
static void expect(bool cond, const std::string& what) {
	if (cond) { ++g_pass; std::cout << "  [PASS] " << what << std::endl; }
	else      { ++g_fail; std::cout << "  [FAIL] " << what << std::endl; }
}

// PDF 例の動作を MutantStack と std::list で比較して同じ出力になるか検証
static std::string collectMutant() {
	MutantStack<int> mstack;
	mstack.push(5);
	mstack.push(17);
	std::ostringstream oss;
	oss << mstack.top() << "\n";  // 17
	mstack.pop();
	oss << mstack.size() << "\n"; // 1
	mstack.push(3);
	mstack.push(5);
	mstack.push(737);
	mstack.push(0);
	MutantStack<int>::iterator it = mstack.begin();
	MutantStack<int>::iterator ite = mstack.end();
	++it; --it;
	while (it != ite) { oss << *it << "\n"; ++it; }
	std::stack<int> s(mstack);
	(void)s;
	return oss.str();
}

static std::string collectList() {
	std::list<int> mstack;
	mstack.push_back(5);
	mstack.push_back(17);
	std::ostringstream oss;
	oss << mstack.back() << "\n";  // top -> back
	mstack.pop_back();             // pop -> pop_back
	oss << mstack.size() << "\n";
	mstack.push_back(3);
	mstack.push_back(5);
	mstack.push_back(737);
	mstack.push_back(0);
	std::list<int>::iterator it = mstack.begin();
	std::list<int>::iterator ite = mstack.end();
	++it; --it;
	while (it != ite) { oss << *it << "\n"; ++it; }
	return oss.str();
}

int main() {
	// === 1. PDF 例そのまま実行 ===
	section("1. PDF example: MutantStack behaves like PDF");
	{
		MutantStack<int> mstack;
		mstack.push(5);
		mstack.push(17);
		expect(mstack.top() == 17, "top() == 17");
		mstack.pop();
		expect(mstack.size() == 1, "size() == 1 after pop");
		mstack.push(3);
		mstack.push(5);
		mstack.push(737);
		mstack.push(0);
		expect(mstack.size() == 5, "5 elements now: 5,3,5,737,0");
	}

	// === 2. PDF 例: iterate 経由の出力が std::list と一致 ===
	section("2. MutantStack iteration output == std::list output");
	{
		std::string m = collectMutant();
		std::string l = collectList();
		expect(m == l, "output identical");
	}

	// === 3. std::stack API 全部使える (継承) ===
	section("3. std::stack API inherited (push/pop/top/size/empty)");
	{
		MutantStack<int> ms;
		expect(ms.empty(), "empty initially");
		ms.push(1);
		ms.push(2);
		ms.push(3);
		expect(!ms.empty() && ms.size() == 3, "3 elements");
		expect(ms.top() == 3, "top is last pushed");
		ms.pop();
		expect(ms.top() == 2, "top after pop == 2");
	}

	// === 4. begin/end iterator: 全要素 traverse ===
	section("4. begin/end traversal");
	{
		MutantStack<int> ms;
		ms.push(10);
		ms.push(20);
		ms.push(30);
		std::ostringstream oss;
		for (MutantStack<int>::iterator it = ms.begin(); it != ms.end(); ++it)
			oss << *it << ",";
		expect(oss.str() == "10,20,30,", "iteration in push order (bottom-up)");
	}

	// === 5. rbegin/rend: 逆走査 ===
	section("5. rbegin/rend reverse iteration");
	{
		MutantStack<int> ms;
		ms.push(1);
		ms.push(2);
		ms.push(3);
		std::ostringstream oss;
		for (MutantStack<int>::reverse_iterator it = ms.rbegin(); it != ms.rend(); ++it)
			oss << *it << ",";
		expect(oss.str() == "3,2,1,", "reverse iteration");
	}

	// === 6. const iterator on const MutantStack ===
	section("6. const_iterator on const MutantStack");
	{
		MutantStack<int> ms;
		ms.push(100);
		ms.push(200);
		const MutantStack<int>& cref = ms;
		std::ostringstream oss;
		for (MutantStack<int>::const_iterator it = cref.begin(); it != cref.end(); ++it)
			oss << *it << ",";
		expect(oss.str() == "100,200,", "const iteration");
	}

	// === 7. iterator との +/- 演算 (random access iterator: deque 由来) ===
	section("7. random access iterator arithmetic");
	{
		MutantStack<int> ms;
		for (int i = 0; i < 10; ++i) ms.push(i);
		MutantStack<int>::iterator it = ms.begin();
		it += 5;
		expect(*it == 5, "begin + 5 -> value 5");
		expect(ms.end() - ms.begin() == 10, "distance == 10");
	}

	// === 8. iterator を STL algorithm で使う ===
	section("8. use MutantStack with std::algorithm");
	{
		MutantStack<int> ms;
		ms.push(5); ms.push(2); ms.push(8); ms.push(1); ms.push(9);
		MutantStack<int>::iterator it = std::find(ms.begin(), ms.end(), 8);
		expect(it != ms.end() && *it == 8, "std::find works on MutantStack");
		int sum = 0;
		for (MutantStack<int>::iterator i = ms.begin(); i != ms.end(); ++i) sum += *i;
		expect(sum == 25, "sum via iteration == 25");
	}

	// === 9. Copy constructor ===
	section("9. copy constructor");
	{
		MutantStack<int> a;
		a.push(1); a.push(2); a.push(3);
		MutantStack<int> b(a);
		expect(b.size() == 3 && b.top() == 3, "b copies a");
		a.push(100);
		expect(b.size() == 3, "a modification doesn't affect b");
	}

	// === 10. operator= ===
	section("10. operator=");
	{
		MutantStack<int> a;
		a.push(10); a.push(20);
		MutantStack<int> b;
		b.push(999);
		b = a;
		expect(b.size() == 2 && b.top() == 20, "b takes a's state");
	}

	// === 11. Self-assignment ===
	section("11. self-assignment safe");
	{
		MutantStack<int> ms;
		ms.push(42);
		ms = ms;
		expect(ms.size() == 1 && ms.top() == 42, "state preserved");
	}

	// === 12. std::stack<int> s(mstack): PDF 最後の一行 ===
	section("12. std::stack constructible from MutantStack");
	{
		MutantStack<int> ms;
		ms.push(11);
		ms.push(22);
		std::stack<int> s(ms);
		expect(s.size() == 2 && s.top() == 22, "std::stack takes MutantStack");
	}

	// === 13. MutantStack<std::string> ===
	section("13. MutantStack<std::string>");
	{
		MutantStack<std::string> ms;
		ms.push("hello");
		ms.push("world");
		expect(ms.top() == "world", "string top");
		std::ostringstream oss;
		for (MutantStack<std::string>::iterator it = ms.begin(); it != ms.end(); ++it)
			oss << *it << ",";
		expect(oss.str() == "hello,world,", "string iteration");
	}

	// === 14. MutantStack<char> ===
	section("14. MutantStack<char>");
	{
		MutantStack<char> ms;
		ms.push('a'); ms.push('b'); ms.push('c');
		expect(ms.top() == 'c', "char top");
	}

	// === 15. Empty stack iteration ===
	section("15. empty stack begin() == end()");
	{
		MutantStack<int> ms;
		expect(ms.begin() == ms.end(), "empty iterators equal");
	}

	// === 16. Large: 10,000 push + iterate ===
	section("16. 10,000 push + full iteration");
	{
		MutantStack<int> ms;
		for (int i = 0; i < 10000; ++i) ms.push(i);
		int count = 0;
		int last = -1;
		for (MutantStack<int>::iterator it = ms.begin(); it != ms.end(); ++it) {
			last = *it;
			++count;
		}
		expect(count == 10000, "10000 iterations counted");
		expect(last == 9999, "last iterated value == 9999");
	}

	// === 17. push/pop cycle (10,000 回) ===
	section("17. 10,000 push/pop cycles");
	{
		MutantStack<int> ms;
		for (int i = 0; i < 10000; ++i) {
			ms.push(i);
			ms.pop();
		}
		expect(ms.empty(), "stack empty after equal push/pop");
	}

	// === 18. underlying container: std::vector を選ぶ ===
	section("18. MutantStack with std::vector as underlying container");
	{
		MutantStack<int, std::vector<int> > ms;
		ms.push(1); ms.push(2); ms.push(3);
		expect(ms.top() == 3 && ms.size() == 3, "vector-backed works");
		std::ostringstream oss;
		for (MutantStack<int, std::vector<int> >::iterator it = ms.begin(); it != ms.end(); ++it)
			oss << *it << ",";
		expect(oss.str() == "1,2,3,", "iteration works");
	}

	// === 19. iterator の ++/-- ===
	section("19. iterator ++/-- roundtrip");
	{
		MutantStack<int> ms;
		ms.push(10); ms.push(20); ms.push(30);
		MutantStack<int>::iterator it = ms.begin();
		expect(*it == 10, "start at 10");
		++it;
		expect(*it == 20, "++ -> 20");
		--it;
		expect(*it == 10, "-- -> 10");
	}

	// === 20. Leak safety: 1000 MutantStack lifecycle ===
	section("20. 1000 MutantStack lifecycles");
	{
		for (int i = 0; i < 1000; ++i) {
			MutantStack<int> ms;
			for (int j = 0; j < 100; ++j) ms.push(j);
		}
		expect(true, "1000 lifecycles no crash");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
