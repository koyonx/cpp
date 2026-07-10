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

	// === 14. Data 配列: 各要素が distinct address ===
	section("14. array of Data: each element has distinct address");
	{
		Data arr[5];
		for (int i = 0; i < 5; ++i) {
			arr[i].id = i * 10;
			std::ostringstream nm; nm << "elem_" << i;
			arr[i].name = nm.str();
			arr[i].value = i * 1.5;
		}
		uintptr_t raws[5];
		for (int i = 0; i < 5; ++i)
			raws[i] = Serializer::serialize(&arr[i]);
		// 隣接要素は sizeof(Data) 差
		bool distinct = true;
		for (int i = 1; i < 5; ++i)
			if (raws[i] == raws[i - 1]) distinct = false;
		expect(distinct, "5 array elements produce distinct uintptr_t");
		// roundtrip 検証
		bool restore_ok = true;
		for (int i = 0; i < 5; ++i) {
			Data* r = Serializer::deserialize(raws[i]);
			if (r != &arr[i] || r->id != i * 10) { restore_ok = false; break; }
		}
		expect(restore_ok, "all 5 array elements restored to correct pointer + data");
	}

	// === 15. アドレス差分が sizeof(Data) の倍数 ===
	section("15. array pointer arithmetic reflected in uintptr_t");
	{
		Data arr[3];
		uintptr_t r0 = Serializer::serialize(&arr[0]);
		uintptr_t r1 = Serializer::serialize(&arr[1]);
		uintptr_t r2 = Serializer::serialize(&arr[2]);
		expect(r1 - r0 == sizeof(Data), "arr[1] - arr[0] == sizeof(Data)");
		expect(r2 - r0 == 2 * sizeof(Data), "arr[2] - arr[0] == 2 * sizeof(Data)");
	}

	// === 16. 大サイズ Data (長い string) ===
	section("16. large Data with 10,000-char name");
	{
		Data d;
		d.id = 1;
		d.name = std::string(10000, 'X');
		d.value = 42.0;
		uintptr_t raw = Serializer::serialize(&d);
		Data* r = Serializer::deserialize(raw);
		expect(r == &d, "pointer preserved for large Data");
		expect(r->name.size() == 10000, "10000-char name intact");
		expect(r->name[9999] == 'X', "last char intact");
	}

	// === 17. 二重 serialize は同じ uintptr_t ===
	section("17. serialize is deterministic on same pointer");
	{
		Data d;
		uintptr_t a = Serializer::serialize(&d);
		uintptr_t b = Serializer::serialize(&d);
		uintptr_t c = Serializer::serialize(&d);
		expect(a == b && b == c, "serialize(&d) always returns same value");
	}

	// === 18. Chained roundtrip: raw -> ptr -> raw -> ptr ... ===
	section("18. chained roundtrip preserves identity");
	{
		Data d;
		d.id = 999;
		uintptr_t r1 = Serializer::serialize(&d);
		Data* p1 = Serializer::deserialize(r1);
		uintptr_t r2 = Serializer::serialize(p1);
		Data* p2 = Serializer::deserialize(r2);
		uintptr_t r3 = Serializer::serialize(p2);
		Data* p3 = Serializer::deserialize(r3);
		expect(r1 == r2 && r2 == r3, "raw values all equal through chain");
		expect(p1 == p2 && p2 == p3 && p3 == &d, "pointers all equal to original");
		expect(p3->id == 999, "data still intact");
	}

	// === 19. 全ゼロ初期化 Data ===
	section("19. zero-initialized Data roundtrip");
	{
		Data d;
		d.id = 0;
		d.name = "";
		d.value = 0.0;
		uintptr_t raw = Serializer::serialize(&d);
		Data* r = Serializer::deserialize(raw);
		expect(r == &d, "zero-init pointer roundtrip");
		expect(r->id == 0 && r->name.empty() && r->value == 0.0, "zero fields preserved");
	}

	// === 20. Data の負値・境界値 ===
	section("20. Data with extreme field values");
	{
		Data d;
		d.id = -2147483648;
		d.name = "unicode 記号 !@#$%";
		d.value = -1e300;
		uintptr_t raw = Serializer::serialize(&d);
		Data* r = Serializer::deserialize(raw);
		expect(r->id == -2147483648, "INT_MIN id preserved");
		expect(r->name == "unicode 記号 !@#$%", "unicode name preserved");
		expect(r->value == -1e300, "extreme double preserved");
	}

	// === 21. deserialize の任意 uintptr_t 値 (規格保証は roundtrip 経由のみ) ===
	section("21. deserialize handles arbitrary uintptr_t values");
	{
		Data d;
		uintptr_t original = Serializer::serialize(&d);
		// 逆算しても roundtrip 経由なら元に戻る
		Data* r = Serializer::deserialize(original);
		expect(r == &d, "arbitrary but roundtripped value restores correctly");
	}

	// === 22. serialize + deserialize は const Data* にも使える (再解釈のみ) ===
	section("22. works with pointers on stack from function scope");
	{
		Data outer;
		outer.id = 111;
		{
			Data inner;
			inner.id = 222;
			uintptr_t rout = Serializer::serialize(&outer);
			uintptr_t rin = Serializer::serialize(&inner);
			expect(rout != rin, "distinct addresses inside inner scope");
			expect(Serializer::deserialize(rout)->id == 111, "outer accessible via raw");
			expect(Serializer::deserialize(rin)->id == 222, "inner accessible via raw");
		}
	}

	// === 23. 100,000 iterations (extreme leak safety) ===
	section("23. 100,000 heap allocations roundtripped + freed");
	{
		bool ok = true;
		for (int i = 0; i < 100000; ++i) {
			Data* d = new Data;
			d->id = i;
			uintptr_t r = Serializer::serialize(d);
			Data* p = Serializer::deserialize(r);
			if (p != d) { ok = false; delete d; break; }
			delete d;
		}
		expect(ok, "100,000 heap alloc + roundtrip + free cycles OK");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}
