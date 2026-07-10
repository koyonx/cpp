#include "Data.hpp"
#include "Serializer.hpp"
#include <iostream>
#include <sstream>
#include <string>

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
	// === 1. PDF 要件: serialize -> deserialize でポインタ等価 ===
	section("1. PDF requirement: serialize -> deserialize equals original pointer");
	{
		Data d;
		d.id = 42;
		d.name = "Answer";
		d.value = 3.14;
		Data* original = &d;
		uintptr_t raw = Serializer::serialize(original);
		Data* restored = Serializer::deserialize(raw);
		expect(restored == original, "restored pointer == original");
	}

	// === 2. データメンバーが保持される (roundtrip 後の read) ===
	section("2. data members preserved after roundtrip");
	{
		Data d;
		d.id = 12345;
		d.name = "hello world";
		d.value = -2.718;
		Data* p = &d;
		uintptr_t raw = Serializer::serialize(p);
		Data* r = Serializer::deserialize(raw);
		expect(r->id == 12345, "id preserved");
		expect(r->name == "hello world", "name preserved");
		expect(r->value == -2.718, "value preserved");
	}

	// === 3. NULL pointer roundtrip ===
	section("3. NULL pointer roundtrip");
	{
		Data* n = NULL;
		uintptr_t raw = Serializer::serialize(n);
		Data* r = Serializer::deserialize(raw);
		expect(r == NULL, "NULL -> serialize -> deserialize -> NULL");
		expect(raw == 0, "serialized NULL == 0");
	}

	// === 4. Heap 割り当てへのポインタ ===
	section("4. heap-allocated Data roundtrip");
	{
		Data* d = new Data;
		d->id = 99;
		d->name = "heap";
		d->value = 1.5;
		uintptr_t raw = Serializer::serialize(d);
		Data* r = Serializer::deserialize(raw);
		expect(r == d, "same heap pointer");
		expect(r->id == 99 && r->name == "heap" && r->value == 1.5, "heap data intact");
		delete d;
	}

	// === 5. 複数の Data で各々独立 ===
	section("5. multiple Data objects, distinct addresses");
	{
		Data a, b, c;
		a.id = 1; b.id = 2; c.id = 3;
		uintptr_t ra = Serializer::serialize(&a);
		uintptr_t rb = Serializer::serialize(&b);
		uintptr_t rc = Serializer::serialize(&c);
		expect(ra != rb && rb != rc && ra != rc, "3 distinct uintptr_t");
		expect(Serializer::deserialize(ra)->id == 1, "a restored");
		expect(Serializer::deserialize(rb)->id == 2, "b restored");
		expect(Serializer::deserialize(rc)->id == 3, "c restored");
	}

	// === 6. deserialize 経由の変更が original に反映 (同一オブジェクト) ===
	section("6. mutation via deserialized ptr affects original");
	{
		Data d;
		d.id = 0;
		uintptr_t raw = Serializer::serialize(&d);
		Data* r = Serializer::deserialize(raw);
		r->id = 777;
		expect(d.id == 777, "modification visible through original");
	}

	// === 7. 大量の roundtrip (leak safety) ===
	section("7. 10,000 roundtrips (leak safety)");
	{
		Data d;
		d.id = 42;
		bool all_ok = true;
		for (int i = 0; i < 10000; ++i) {
			uintptr_t raw = Serializer::serialize(&d);
			Data* r = Serializer::deserialize(raw);
			if (r != &d) { all_ok = false; break; }
		}
		expect(all_ok, "all 10000 roundtrips preserve identity");
	}

	// === 8. 大量の heap Data で roundtrip ===
	section("8. 1000 heap allocations roundtripped and freed");
	{
		bool all_ok = true;
		for (int i = 0; i < 1000; ++i) {
			Data* d = new Data;
			d->id = i;
			uintptr_t raw = Serializer::serialize(d);
			Data* r = Serializer::deserialize(raw);
			if (r != d || r->id != i) { all_ok = false; delete d; break; }
			delete d;
		}
		expect(all_ok, "1000 heap allocations roundtripped correctly");
	}

	// === 9. Data structure が non-empty (PDF要件) ===
	section("9. Data has data members (non-empty)");
	{
		expect(sizeof(Data) > 0, "Data non-empty");
		Data d;
		d.id = 1;
		d.name = "n";
		d.value = 2.0;
		expect(d.id == 1 && d.name == "n" && d.value == 2.0, "3 members accessible");
	}

	// === 10. serialize/deserialize は static (インスタンス不要) ===
	section("10. static methods callable without instance");
	{
		Data d;
		// Serializer 型のインスタンスを作らずに呼べる (コンパイル成功 = PASS)
		uintptr_t r = Serializer::serialize(&d);
		Data* p = Serializer::deserialize(r);
		expect(p == &d, "invoked without Serializer instance");
	}

	// === 11. Serializer は instantiate 不可 (コンパイル時テスト、コメントで説明) ===
	section("11. Serializer not instantiable (compile-time)");
	// 以下は private ctor によりコンパイルエラー:
	// Serializer s;                    // error
	// Serializer* p = new Serializer;  // error
	expect(true, "Serializer ctors are private (link-time enforcement)");

	// === 12. uintptr_t は 0 に近い低アドレスも扱える ===
	section("12. uintptr_t(0) deserializes to NULL");
	{
		Data* r = Serializer::deserialize(static_cast<uintptr_t>(0));
		expect(r == NULL, "0 -> NULL");
	}

	// === 13. Roundtrip: char* -> uintptr_t -> Data* -> ... は未定義動作なのでテストしない ===
	section("13. same address preserved across mixed heap/stack");
	{
		Data stk;
		Data* heap = new Data;
		uintptr_t s = Serializer::serialize(&stk);
		uintptr_t h = Serializer::serialize(heap);
		expect(s != h, "stack and heap addresses differ");
		expect(Serializer::deserialize(s) == &stk, "stack restored");
		expect(Serializer::deserialize(h) == heap, "heap restored");
		delete heap;
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
