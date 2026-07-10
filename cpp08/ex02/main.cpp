#include "MutantStack.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <cstddef>

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

	// === 21. iterator 経由で書き込みが stack に反映 ===
	section("21. write via iterator persists in stack");
	{
		MutantStack<int> ms;
		ms.push(10); ms.push(20); ms.push(30);
		MutantStack<int>::iterator it = ms.begin();
		++it;  // it -> 20
		*it = 999;
		// top は 30 のまま (30 は最後 push)
		expect(ms.top() == 30, "top unchanged");
		// deque based: iteration order 10, 999, 30
		std::ostringstream oss;
		for (MutantStack<int>::iterator i = ms.begin(); i != ms.end(); ++i)
			oss << *i << ",";
		expect(oss.str() == "10,999,30,", "iteration reflects modification");
	}

	// === 22. 複数 iterator 同時使用 ===
	section("22. multiple concurrent iterators");
	{
		MutantStack<int> ms;
		for (int i = 0; i < 5; ++i) ms.push(i);
		MutantStack<int>::iterator it1 = ms.begin();
		MutantStack<int>::iterator it2 = ms.begin();
		it2 += 3;
		expect(*it1 == 0, "it1 at start == 0");
		expect(*it2 == 3, "it2 at +3 == 3");
		expect(it2 - it1 == 3, "it2 - it1 == 3");
	}

	// === 23. std::distance/std::advance ===
	section("23. std::distance / std::advance on MutantStack iterator");
	{
		MutantStack<int> ms;
		for (int i = 0; i < 20; ++i) ms.push(i);
		MutantStack<int>::iterator it = ms.begin();
		std::advance(it, 10);
		expect(*it == 10, "advance to 10");
		expect(std::distance(ms.begin(), ms.end()) == 20, "distance == 20");
	}

	// === 24. std::sort MutantStack の内部を並べ替える (実装依存) ===
	section("24. std::sort on iterator range (random access from deque)");
	{
		MutantStack<int> ms;
		ms.push(5); ms.push(3); ms.push(8); ms.push(1); ms.push(9);
		std::sort(ms.begin(), ms.end());
		std::ostringstream oss;
		for (MutantStack<int>::iterator it = ms.begin(); it != ms.end(); ++it)
			oss << *it << ",";
		expect(oss.str() == "1,3,5,8,9,", "sorted iteration");
		// top は sort 後の末尾 = 9
		expect(ms.top() == 9, "top after sort == 9");
	}

	// === 25. std::count ===
	section("25. std::count on MutantStack iterator range");
	{
		MutantStack<int> ms;
		ms.push(1); ms.push(2); ms.push(1); ms.push(3); ms.push(1);
		expect(std::count(ms.begin(), ms.end(), 1) == 3, "count of 1 == 3");
	}

	// === 26. operator= returns *this (chainable) ===
	section("26. operator= returns *this");
	{
		MutantStack<int> a, b, c;
		c.push(42);
		MutantStack<int>& ref = (a = b = c);
		expect(&ref == &a, "returns reference to lhs");
		expect(a.top() == 42, "a takes 42 via chain");
	}

	// === 27. 継承した std::stack の比較演算子 ===
	section("27. inherited comparison operators");
	{
		MutantStack<int> a, b;
		a.push(1); a.push(2);
		b.push(1); b.push(2);
		expect(a == b, "equal stacks are ==");
		b.push(3);
		expect(a != b, "different stacks are !=");
		expect(a < b, "shorter stack < longer (lexicographic)");
	}

	// === 28. std::stack と比較可能 ===
	section("28. compare with std::stack");
	{
		MutantStack<int> ms;
		ms.push(1); ms.push(2); ms.push(3);
		std::stack<int> ss;
		ss.push(1); ss.push(2); ss.push(3);
		std::stack<int> converted = ms;
		expect(ss == converted, "converted MutantStack equals same-content std::stack");
	}

	// === 29. Nested MutantStack of MutantStack ===
	section("29. MutantStack<MutantStack<int> > (nested)");
	{
		MutantStack<MutantStack<int> > outer;
		MutantStack<int> inner;
		inner.push(10); inner.push(20);
		outer.push(inner);
		expect(outer.size() == 1, "outer has 1 element");
		expect(outer.top().size() == 2, "inner has 2 elements");
		expect(outer.top().top() == 20, "inner top == 20");
	}

	// === 30. push after copy: 独立性の再確認 ===
	section("30. push after copy: originals independent");
	{
		MutantStack<int> a;
		a.push(1);
		MutantStack<int> b(a);
		a.push(2);
		b.push(99);
		expect(a.size() == 2 && a.top() == 2, "a has 2 elements ending with 2");
		expect(b.size() == 2 && b.top() == 99, "b has 2 elements ending with 99");
	}

	// === 31. Copy assignment: 完全上書き ===
	section("31. operator= fully overwrites");
	{
		MutantStack<int> a;
		for (int i = 0; i < 10; ++i) a.push(i);
		MutantStack<int> b;
		b.push(999);
		b = a;
		expect(b.size() == 10, "b size == a size");
		expect(b.top() == 9, "b top == a top");
	}

	// === 32. Const MutantStack: iterate & size ===
	section("32. const MutantStack: iterate + size + top");
	{
		MutantStack<int> ms;
		ms.push(100); ms.push(200); ms.push(300);
		const MutantStack<int>& cref = ms;
		expect(cref.size() == 3, "size on const");
		expect(cref.top() == 300, "top on const");
		int sum = 0;
		for (MutantStack<int>::const_iterator it = cref.begin(); it != cref.end(); ++it)
			sum += *it;
		expect(sum == 600, "sum via const iteration");
	}

	// === 33. Iterator arithmetic: begin + size == end ===
	section("33. iterator invariant: begin + size == end");
	{
		MutantStack<int> ms;
		for (int i = 0; i < 7; ++i) ms.push(i);
		expect(ms.begin() + static_cast<std::ptrdiff_t>(ms.size()) == ms.end(),
		       "begin + size == end");
	}

	// === 34. Empty stack: begin == end, size == 0 ===
	section("34. empty stack invariants");
	{
		MutantStack<int> ms;
		expect(ms.begin() == ms.end() && ms.rbegin() == ms.rend(), "empty iterators equal");
		expect(ms.size() == 0 && ms.empty(), "size 0 & empty()");
	}

	// === 35. push - pop - push - pop パターン ===
	section("35. push/pop interleaved pattern");
	{
		MutantStack<int> ms;
		ms.push(1);
		ms.push(2);
		ms.pop();
		ms.push(3);
		ms.pop();
		ms.pop();
		expect(ms.empty(), "empty after balanced push/pop");
	}

	// === 36. MutantStack of std::string ===
	section("36. MutantStack<std::string> operations");
	{
		MutantStack<std::string> ms;
		ms.push("first");
		ms.push("second");
		ms.push("third");
		expect(ms.top() == "third", "string top");
		ms.pop();
		expect(ms.top() == "second", "string top after pop");
		expect(ms.size() == 2, "size 2");
	}

	// === 37. iterate と push を交互 (deque はイテレータ無効化少ない) ===
	section("37. deque-backed: iterator survives some push operations");
	{
		MutantStack<int> ms;
		ms.push(1); ms.push(2); ms.push(3);
		int total = 0;
		for (MutantStack<int>::iterator it = ms.begin(); it != ms.end(); ++it)
			total += *it;
		expect(total == 6, "1+2+3 == 6");
	}

	// === 38. size() は const method ===
	section("38. size() const-callable");
	{
		MutantStack<int> ms;
		ms.push(1);
		const MutantStack<int>* cptr = &ms;
		expect(cptr->size() == 1, "size via const ptr");
	}

	// === 39. push で size が線形に増える ===
	section("39. size grows linearly with pushes");
	{
		MutantStack<int> ms;
		for (int i = 1; i <= 100; ++i) {
			ms.push(i);
			if (static_cast<int>(ms.size()) != i) {
				expect(false, "size mismatch");
				break;
			}
		}
		expect(ms.size() == 100, "size == 100 after 100 pushes");
	}

	// === 40. pop で size が線形に減る ===
	section("40. size shrinks linearly with pops");
	{
		MutantStack<int> ms;
		for (int i = 0; i < 50; ++i) ms.push(i);
		for (int i = 49; i >= 0; --i) {
			ms.pop();
			if (static_cast<int>(ms.size()) != i) {
				expect(false, "size mismatch");
				break;
			}
		}
		expect(ms.empty(), "empty after pop-all");
	}

	// === 41. Vector-backed with std::sort ===
	section("41. vector-backed MutantStack + std::sort");
	{
		MutantStack<int, std::vector<int> > ms;
		ms.push(5); ms.push(1); ms.push(3);
		std::sort(ms.begin(), ms.end());
		expect(*ms.begin() == 1, "sorted begin == 1");
	}

	// === 42. std::stack から MutantStack への copy 経由の interop ===
	// (MutantStack の copy ctor は MutantStack しか受け付けないので、
	// std::stack から作るには一旦 MutantStack を作って中身をコピーする必要がある)
	section("42. MutantStack from std::stack: pattern via manual push");
	{
		std::stack<int> src;
		src.push(1); src.push(2); src.push(3);
		MutantStack<int> ms;
		// std::stack を破壊的に空にしつつ順序を保つには temporary 経由
		std::stack<int> tmp;
		while (!src.empty()) { tmp.push(src.top()); src.pop(); }
		while (!tmp.empty()) { ms.push(tmp.top()); tmp.pop(); }
		expect(ms.size() == 3 && ms.top() == 3, "MutantStack rebuilt from std::stack");
	}

	// === 43. iterator による reverse iteration もう一度 ===
	section("43. rbegin/rend deep traversal");
	{
		MutantStack<int> ms;
		for (int i = 1; i <= 5; ++i) ms.push(i);
		std::ostringstream oss;
		for (MutantStack<int>::reverse_iterator it = ms.rbegin(); it != ms.rend(); ++it)
			oss << *it << ",";
		expect(oss.str() == "5,4,3,2,1,", "reverse order");
	}

	// === 44. Large stack: 100,000 push + iterate ===
	section("44. MutantStack with 100,000 elements");
	{
		MutantStack<int> ms;
		for (int i = 0; i < 100000; ++i) ms.push(i);
		int count = 0;
		int last = -1;
		for (MutantStack<int>::iterator it = ms.begin(); it != ms.end(); ++it) {
			last = *it;
			++count;
		}
		expect(count == 100000, "100k iterations");
		expect(last == 99999, "last == 99999");
	}

	// === 45. std::stack constructible from MutantStack via copy ctor ===
	section("45. std::stack<int> s = MutantStack (implicit copy)");
	{
		MutantStack<int> ms;
		ms.push(11); ms.push(22); ms.push(33);
		std::stack<int> s = ms;
		expect(s.size() == 3 && s.top() == 33, "implicit copy works");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
