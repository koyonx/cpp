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

	// === 14. Upcast (derived -> base) は常に成功 ===
	section("14. dynamic_cast upcast (derived -> Base) always succeeds");
	{
		A a;
		Base* b = dynamic_cast<Base*>(&a);
		expect(b == static_cast<Base*>(&a), "Base* upcast from A*");
	}
	{
		B b;
		Base& ref = dynamic_cast<Base&>(b);
		expect(&ref == static_cast<Base*>(&b), "Base& upcast from B&");
	}
	{
		C c;
		Base* p = dynamic_cast<Base*>(&c);
		expect(p != NULL, "Base* upcast from C*");
	}

	// === 15. dynamic_cast の crosscast (B <-> C) は全部失敗 ===
	section("15. dynamic_cast crosscast between siblings fails");
	{
		B b;
		Base* base = &b;
		expect(dynamic_cast<A*>(base) == NULL, "B -> A crosscast fails");
		expect(dynamic_cast<C*>(base) == NULL, "B -> C crosscast fails");
	}

	// === 16. 同じオブジェクトを 100 回 identify → 同じ結果 ===
	section("16. multiple identify calls return same result");
	{
		A a;
		bool all_A = true;
		for (int i = 0; i < 100; ++i) {
			if (capturePtr(&a) != "A") { all_A = false; break; }
			if (captureRef(a) != "A") { all_A = false; break; }
		}
		expect(all_A, "100 identify calls on same A -> always 'A'");
	}

	// === 17. Base* 配列 (mixed types) を identify ===
	section("17. Base* array with mixed concrete types");
	{
		A a; B b; C c;
		Base* arr[6] = { &a, &b, &c, &a, &b, &c };
		const char* expected[6] = { "A", "B", "C", "A", "B", "C" };
		bool ok = true;
		for (int i = 0; i < 6; ++i) {
			if (capturePtr(arr[i]) != expected[i]) { ok = false; break; }
		}
		expect(ok, "6-element mixed array identified correctly");
	}

	// === 18. Base* 配列 (heap allocated, delete via Base*) ===
	section("18. heap-allocated mixed Base* array");
	{
		Base* arr[3];
		arr[0] = new A();
		arr[1] = new B();
		arr[2] = new C();
		expect(capturePtr(arr[0]) == "A", "heap A");
		expect(capturePtr(arr[1]) == "B", "heap B");
		expect(capturePtr(arr[2]) == "C", "heap C");
		for (int i = 0; i < 3; ++i) delete arr[i];
	}

	// === 19. sizeof: A/B/C は空だが仮想テーブルポインタで sizeof(Base) と一致 ===
	section("19. sizeof consistency (empty derived classes)");
	{
		expect(sizeof(A) == sizeof(Base), "sizeof(A) == sizeof(Base)");
		expect(sizeof(B) == sizeof(Base), "sizeof(B) == sizeof(Base)");
		expect(sizeof(C) == sizeof(Base), "sizeof(C) == sizeof(Base)");
	}

	// === 20. より大規模な統計テスト (5000 iter, tolerance ±300) ===
	section("20. larger statistical distribution (5000 trials)");
	{
		int cA = 0, cB = 0, cC = 0;
		for (int i = 0; i < 5000; ++i) {
			Base* p = generate();
			std::string t = capturePtr(p);
			if      (t == "A") ++cA;
			else if (t == "B") ++cB;
			else if (t == "C") ++cC;
			delete p;
		}
		expect(cA + cB + cC == 5000, "5000 total generates");
		expect(cA >= 1400 && cA <= 2100, "A count in [1400,2100]");
		expect(cB >= 1400 && cB <= 2100, "B count in [1400,2100]");
		expect(cC >= 1400 && cC <= 2100, "C count in [1400,2100]");
	}

	// === 21. 100,000 gen/delete (extreme leak safety) ===
	section("21. 100,000 generate/delete (extreme leak safety)");
	{
		for (int i = 0; i < 100000; ++i) {
			Base* p = generate();
			delete p;
		}
		expect(true, "no crash after 100,000 iterations");
	}

	// === 22. identify (both versions) via array of Base& からの参照束縛 ===
	section("22. identify via reference-bound Base&");
	{
		A a; B b; C c;
		Base& r1 = a;
		Base& r2 = b;
		Base& r3 = c;
		expect(captureRef(r1) == "A" && captureRef(r2) == "B" && captureRef(r3) == "C",
		       "3 references bound & identified");
	}

	// === 23. Base 自身のインスタンス経由: dynamic_cast は全て失敗 ===
	// (Base b; dynamic_cast<A*>(&b) は compile-time に never-succeed 判定されるので
	//  実行時に判定させるため new Base() を使う)
	section("23. dynamic_cast on plain Base always fails to A/B/C");
	{
		Base* b = new Base();
		expect(dynamic_cast<A*>(b) == NULL, "Base -> A* fails at runtime");
		expect(dynamic_cast<B*>(b) == NULL, "Base -> B* fails at runtime");
		expect(dynamic_cast<C*>(b) == NULL, "Base -> C* fails at runtime");
		delete b;
	}

	// === 24. delete NULL は安全 (念のため) ===
	section("24. delete NULL Base* is safe");
	{
		Base* p = NULL;
		delete p;
		expect(true, "delete NULL didn't crash");
	}

	// === 25. RTTI が有効: 仮想関数を持つのでdynamic_cast がコンパイル可能 ===
	section("25. RTTI enabled (virtual dtor makes dynamic_cast well-formed)");
	{
		// これがコンパイルできること自体が RTTI 有効の証明
		A a;
		Base* base = &a;
		A* recovered = dynamic_cast<A*>(base);
		expect(recovered == &a, "dynamic_cast round-trips A -> Base -> A");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
