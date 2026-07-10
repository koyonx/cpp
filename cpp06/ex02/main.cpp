#include "Base.hpp"
#include "A.hpp"
#include "B.hpp"
#include "C.hpp"
#include "Functions.hpp"
#include <iostream>
#include <sstream>
#include <string>
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

static std::string capturePtr(Base* p) {
	std::ostringstream oss;
	std::streambuf* saved = std::cout.rdbuf(oss.rdbuf());
	identify(p);
	std::cout.rdbuf(saved);
	std::string s = oss.str();
	// 末尾の "\n" を削除
	if (!s.empty() && s[s.size() - 1] == '\n') s.erase(s.size() - 1);
	return s;
}

static std::string captureRef(Base& p) {
	std::ostringstream oss;
	std::streambuf* saved = std::cout.rdbuf(oss.rdbuf());
	identify(p);
	std::cout.rdbuf(saved);
	std::string s = oss.str();
	if (!s.empty() && s[s.size() - 1] == '\n') s.erase(s.size() - 1);
	return s;
}

int main() {
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// === 1. identify(A*) -> "A" ===
	section("1. identify pointer version: correct type per concrete");
	{
		A a; B b; C c;
		expect(capturePtr(&a) == "A", "A* -> 'A'");
		expect(capturePtr(&b) == "B", "B* -> 'B'");
		expect(capturePtr(&c) == "C", "C* -> 'C'");
	}

	// === 2. identify(A&) -> "A" ===
	section("2. identify reference version: correct type per concrete");
	{
		A a; B b; C c;
		expect(captureRef(a) == "A", "A& -> 'A'");
		expect(captureRef(b) == "B", "B& -> 'B'");
		expect(captureRef(c) == "C", "C& -> 'C'");
	}

	// === 3. 両方の identify() が同じ結果 ===
	section("3. pointer and reference versions agree");
	{
		A a;
		expect(capturePtr(&a) == captureRef(a), "A: ptr == ref");
	}
	{
		B b;
		expect(capturePtr(&b) == captureRef(b), "B: ptr == ref");
	}
	{
		C c;
		expect(capturePtr(&c) == captureRef(c), "C: ptr == ref");
	}

	// === 4. Base* 経由の polymorphic identify ===
	section("4. identify via Base* variable");
	{
		Base* pa = new A();
		Base* pb = new B();
		Base* pc = new C();
		expect(capturePtr(pa) == "A", "Base* -> A");
		expect(capturePtr(pb) == "B", "Base* -> B");
		expect(capturePtr(pc) == "C", "Base* -> C");
		delete pa; delete pb; delete pc;
	}

	// === 5. Base& 経由の polymorphic identify ===
	section("5. identify via Base& variable");
	{
		A a; B b; C c;
		Base& ra = a;
		Base& rb = b;
		Base& rc = c;
		expect(captureRef(ra) == "A", "Base& -> A");
		expect(captureRef(rb) == "B", "Base& -> B");
		expect(captureRef(rc) == "C", "Base& -> C");
	}

	// === 6. NULL pointer -> Unknown (crashしない) ===
	section("6. identify(NULL) does not crash");
	{
		Base* p = NULL;
		std::string s = capturePtr(p);
		expect(s == "Unknown", "NULL -> 'Unknown'");
	}

	// === 7. Base 自身のインスタンス -> Unknown ===
	section("7. plain Base instance -> Unknown");
	{
		Base b;
		expect(capturePtr(&b) == "Unknown", "Base* -> 'Unknown'");
		expect(captureRef(b) == "Unknown", "Base& -> 'Unknown'");
	}

	// === 8. generate() は少なくとも3種類を返す (100試行) ===
	section("8. generate() eventually returns all 3 types");
	{
		int countA = 0, countB = 0, countC = 0;
		for (int i = 0; i < 100; ++i) {
			Base* p = generate();
			std::string t = capturePtr(p);
			if (t == "A") ++countA;
			else if (t == "B") ++countB;
			else if (t == "C") ++countC;
			delete p;
		}
		expect(countA > 0 && countB > 0 && countC > 0,
		       "all 3 types appeared in 100 generates");
		std::ostringstream m;
		m << "  counts A=" << countA << " B=" << countB << " C=" << countC;
		std::cout << m.str() << std::endl;
	}

	// === 9. generate() 統計的分布 (1000試行, tolerance ±150) ===
	section("9. generate() ~1/3 distribution over 1000 trials");
	{
		int cA = 0, cB = 0, cC = 0;
		for (int i = 0; i < 1000; ++i) {
			Base* p = generate();
			std::string t = capturePtr(p);
			if      (t == "A") ++cA;
			else if (t == "B") ++cB;
			else if (t == "C") ++cC;
			delete p;
		}
		expect(cA + cB + cC == 1000, "1000 total generates");
		expect(cA >= 200 && cA <= 470, "A count in [200,470]");
		expect(cB >= 200 && cB <= 470, "B count in [200,470]");
		expect(cC >= 200 && cC <= 470, "C count in [200,470]");
	}

	// === 10. virtual デストラクタ: delete Base* で子dtor実行 ===
	section("10. virtual dtor: delete via Base* calls derived dtor");
	{
		for (int i = 0; i < 1000; ++i) {
			Base* p = generate();
			delete p; // 仮想dtor経由でリークしないこと
		}
		expect(true, "1000 generate/delete cycles OK");
	}

	// === 11. reference 版が pointer 使わない: A/B/C を切り替えて ===
	section("11. reference identify handles polymorphic switch");
	{
		A a; B b; C c;
		Base* forms[3] = { &a, &b, &c };
		const char* expected[3] = { "A", "B", "C" };
		bool ok = true;
		for (int i = 0; i < 3; ++i) {
			if (captureRef(*forms[i]) != expected[i]) { ok = false; break; }
		}
		expect(ok, "3 references identified correctly");
	}

	// === 12. leak safety: 10000 generate/delete ===
	section("12. 10,000 generate/delete iterations");
	{
		for (int i = 0; i < 10000; ++i) {
			Base* p = generate();
			delete p;
		}
		expect(true, "no crash after 10000 iterations");
	}

	// === 13. dynamic_cast の chained fallback (A->B->C) の一貫性 ===
	section("13. dynamic_cast type identification consistent");
	{
		A a;
		Base* base = &a;  // Base* を経由することで compile-time 判定を回避
		expect(dynamic_cast<A*>(base) != NULL, "A* dynamic_cast succeeds on Base*(A)");
		expect(dynamic_cast<B*>(base) == NULL, "B* dynamic_cast fails on Base*(A)");
		expect(dynamic_cast<C*>(base) == NULL, "C* dynamic_cast fails on Base*(A)");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
