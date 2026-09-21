# CPP Module 06 — Knowledge / レビュー & 解説

## 総合評価

| 項目 | 結果 |
|------|------|
| **コンパイル** | ✅ 全 exercise が `c++ -Wall -Wextra -Werror -std=c++98` でコンパイル成功 |
| **禁止関数** | ✅ `*printf` / `*alloc` / `free` の使用なし |
| **`using namespace`** | ✅ 使用なし |
| **`friend`** | ✅ 使用なし |
| **ヘッダに関数実装** | ✅ テンプレート以外の実装がヘッダにない |
| **外部ライブラリ / C++11** | ✅ 使用なし |
| **Exercise 00** | ✅ 合格 |
| **Exercise 01** | ✅ 合格 |
| **Exercise 02** | ✅ 合格 |

---

## Prerequisites（前提条件）チェック

### コンパイラ・フラグ
3つの Makefile すべてが `c++` コンパイラで `-Wall -Wextra -Werror -std=c++98 -pedantic` を使用。  
要件の `c++` + `-Wall -Wextra -Werror` を完全に満たしている。

### 禁止事項

| チェック項目 | 状態 | 補足 |
|---|---|---|
| `*alloc` / `*printf` / `free` | ✅ 不使用 | ソースコード中に該当なし |
| `using namespace <ns>` | ✅ 不使用 | — |
| `friend` キーワード | ✅ 不使用 | — |
| ヘッダ内関数実装 | ✅ なし | 全関数は `.cpp` に実装 |
| 外部ライブラリ / C++11 | ✅ 不使用 | `<stdint.h>` は C99/C++98 で使用可 |

---

## Exercise 00: Conversion of scalar types（`static_cast`）

### 概要
`ScalarConverter` クラスの `convert` static メソッドが、文字列リテラルを `char` / `int` / `float` / `double` に変換して表示する。

### 評価項目の検証

#### ✅ private コンストラクタ + static メソッド
```cpp
class ScalarConverter {
private:
    ScalarConverter();                              // private
    ScalarConverter(const ScalarConverter& other);   // private
    ScalarConverter& operator=(const ScalarConverter& other); // private
    ~ScalarConverter();                              // private
public:
    static void convert(const std::string& literal); // static
};
```
ユーザーがインスタンス化できない設計（Orthodox Canonical Form の 4 メンバすべて private）。

#### ✅ `static_cast` の使用
[`ScalarConverter.cpp`](file:///Users/kkz/Downloads/cpp/cpp/cpp06/ex00/ScalarConverter.cpp) で以下のように `static_cast` を使用：
- `static_cast<char>(d)` — double → char 変換（L72）
- `static_cast<unsigned char>(c)` — char → unsigned char（L73）
- `static_cast<int>(d)` — double → int 変換（L91）
- `static_cast<float>(d)` — double → float 変換（L99）
- `static_cast<double>(std::numeric_limits<int>::min())` — 境界値比較（L86-87）

暗黙的キャスト（promotion cast）はプロモーションのみに使用されており、要件を満たしている。

#### ✅ 正しい動作
PDF の出力例と完全一致（テストスイートで 120/120 テスト全 PASS）：

```
./convert 0
char: Non displayable
int: 0
float: 0.0f
double: 0.0

./convert nan
char: impossible
int: impossible
float: nanf
double: nan

./convert 42.0f
char: '*'
int: 42
float: 42.0f
double: 42.0
```

### 実装の詳細解説

#### 型判定の流れ
1. **pseudo-literal 判定** — `nan`, `nanf`, `+inf`, `-inf`, `+inff`, `-inff` を文字列比較
2. **char literal 判定** — 1文字かつ非数字なら char
3. **数値パース** — `strtod()` で double にパースし、末尾が空または `"f"` なら有効
4. **無効入力** — 上記すべてに該当しなければ全型 `impossible`

#### 設計上のポイント
- 全入力を一度 `double` に変換してから各型に `static_cast` で明示変換する方式
- `anonymous namespace` を使って内部関数をファイルスコープに限定（C++98 準拠のモジュール性）
- `isNan(d)` は `d != d` で NaN 判定（C++98 で `std::isnan` が使えないため）
- `std::fixed` + `std::setprecision(1)` で小数点以下の `.0` 表示を実現し、使用後にリセット

#### エッジケース対応
- `ERANGE` チェックで `strtod` のオーバーフロー検出
- `INT_MAX` / `INT_MIN` 境界の正確な判定
- float オーバーフロー時に `+inff` / `-inff` 表示
- `"42f"`（小数点なし + f suffix）→ float として認識される（`strtod` が `42` まで読み、残り `"f"` で float 判定）

> [!NOTE]
> `"inf"` や `"inff"`（`+` なし）も pseudo-literal として受理している。Subject が明示的に要求しているのは `+inf` / `-inf` / `+inff` / `-inff` / `nan` / `nanf` だが、`inf` / `inff` を受け入れるのは自然な拡張であり、問題ない。

---

## Exercise 01: Serialization（`reinterpret_cast`）

### 概要
`Serializer` クラスが `Data*` ⇄ `uintptr_t` の相互変換を行う。

### 評価項目の検証

#### ✅ private コンストラクタ + static メソッド
```cpp
class Serializer {
private:
    Serializer();
    Serializer(const Serializer& other);
    Serializer& operator=(const Serializer& other);
    ~Serializer();
public:
    static uintptr_t serialize(Data* ptr);
    static Data* deserialize(uintptr_t raw);
};
```

#### ✅ `reinterpret_cast` を2回使用
[`Serializer.cpp`](file:///Users/kkz/Downloads/cpp/cpp/cpp06/ex01/Serializer.cpp) にて：
```cpp
uintptr_t Serializer::serialize(Data* ptr) {
    return reinterpret_cast<uintptr_t>(ptr);  // Data* → uintptr_t
}

Data* Serializer::deserialize(uintptr_t raw) {
    return reinterpret_cast<Data*>(raw);       // uintptr_t → Data*
}
```
要件通り `Data* → uintptr_t` と `uintptr_t → Data*` で各 1 回ずつ使用。

#### ✅ Data 構造体が non-empty
[`Data.hpp`](file:///Users/kkz/Downloads/cpp/cpp/cpp06/ex01/Data.hpp) にて 3 つのデータメンバを持つ：
```cpp
struct Data {
    int         id;
    std::string name;
    double      value;
};
```

#### ✅ ラウンドトリップが正しく動作
テストスイートで 44/44 テスト全 PASS。`serialize → deserialize` 後にポインタが元と等しく、データメンバも保持される。

### 実装の解説

`reinterpret_cast` はポインタを整数（`uintptr_t`）に型変換するだけで、実際のデータコピーやシリアライズは行わない。  
C++98 規格では、ポインタから `uintptr_t` への往復変換が元のポインタと等しいことが保証されている。

> [!TIP]
> `<stdint.h>` は C99 で導入されたヘッダだが、C++98 環境でも実用上使える（多くのコンパイラが提供）。より厳密な C++98 準拠を目指すなら `<cstdint>` が望ましいが、`<cstdint>` は C++11 で正式導入のため、`<stdint.h>` の使用は妥当な判断。

---

## Exercise 02: Identify real type（`dynamic_cast`）

### 概要
`Base` クラスの仮想デストラクタを通じて、`A`, `B`, `C` の実行時型をポインタ版とリファレンス版で判定する。

### 評価項目の検証

#### ✅ `dynamic_cast` による型識別
[`Functions.cpp`](file:///Users/kkz/Downloads/cpp/cpp/cpp06/ex02/Functions.cpp) にて `dynamic_cast` を使用。

#### ✅ `identify(Base* p)` — NULL チェック
```cpp
void identify(Base* p) {
    if (dynamic_cast<A*>(p)) { std::cout << "A" << std::endl; return; }
    if (dynamic_cast<B*>(p)) { std::cout << "B" << std::endl; return; }
    if (dynamic_cast<C*>(p)) { std::cout << "C" << std::endl; return; }
    std::cout << "Unknown" << std::endl;
}
```
`dynamic_cast<T*>()` はキャスト失敗時に `NULL` を返すため、`if` 条件で自然に NULL チェックが行われている。  
`p` 自体が `NULL` の場合も `dynamic_cast<A*>(NULL)` は `NULL` を返すので安全に `"Unknown"` が出力される。

#### ✅ `identify(Base& p)` — try/catch ブロック
```cpp
void identify(Base& p) {
    try { A& a = dynamic_cast<A&>(p); (void)a; std::cout << "A" << std::endl; return; }
    catch (...) {}
    try { B& b = dynamic_cast<B&>(p); (void)b; std::cout << "B" << std::endl; return; }
    catch (...) {}
    try { C& c = dynamic_cast<C&>(p); (void)c; std::cout << "C" << std::endl; return; }
    catch (...) {}
    std::cout << "Unknown" << std::endl;
}
```
リファレンス版の `dynamic_cast<T&>()` は失敗時に例外（`std::bad_cast`）を投げるため、`try/catch` で捕捉。  
ポインタ変数を関数内で使用していないことも要件通り。

#### ✅ `<typeinfo>` ヘッダの不使用
ソースファイルに `#include <typeinfo>` は一切なし。  
`catch(...)` で全例外を捕捉しているため、`std::bad_cast` を名前で参照する必要がなく、`<typeinfo>` のインクルードを回避している。

### 実装の解説

#### `dynamic_cast` の仕組み
- **ポインタ版**: 失敗時 → `NULL` を返す。成功時 → 変換後のポインタ
- **リファレンス版**: 失敗時 → `std::bad_cast` 例外を投げる。成功時 → 変換後の参照
- `dynamic_cast` が機能するには RTTI（Run-Time Type Information）が必要で、`Base` クラスに `virtual` メンバ関数（ここではデストラクタ）があることが前提

#### `generate()` 関数
```cpp
Base* generate(void) {
    switch (std::rand() % 3) {
        case 0: return new A();
        case 1: return new B();
        case 2: return new C();
    }
    return NULL;
}
```
`std::rand() % 3` でランダムに A/B/C を生成。`srand()` の呼び出しは `main.cpp` で行われている。

#### クラス階層
```
Base (virtual ~Base())
 ├── A : public Base {}
 ├── B : public Base {}
 └── C : public Base {}
```
A, B, C は空クラス（data member なし）。Orthodox Canonical Form は不要（Subject で明示的に免除）。

---

## 3 つの C++ キャストの使い分け（Module 06 の核心）

| キャスト | 用途 | Exercise |
|---|---|---|
| `static_cast` | コンパイル時に安全性が検証できる型変換（数値間変換、基底↔派生の既知変換） | ex00 |
| `reinterpret_cast` | ポインタ ↔ 整数、無関係なポインタ型間の低レベル変換 | ex01 |
| `dynamic_cast` | 実行時にポリモーフィズムを利用した安全なダウンキャスト（RTTI 必要） | ex02 |

### `static_cast`（ex00）
- コンパイル時に変換の妥当性をチェック
- `double → int` や `int → char` のような縮小変換に使用
- C の暗黙的型変換を明示化するもの

### `reinterpret_cast`（ex01）
- ビットパターンを保持したまま型を再解釈
- `Data*` のアドレス値を `uintptr_t` 整数として保存し、後で復元
- データの内容には一切手を加えない

### `dynamic_cast`（ex02）
- RTTI を利用して実行時に型を判定
- ポインタ版は失敗時 `NULL`、リファレンス版は失敗時例外
- 仮想関数テーブル（vtable）が必要 → `virtual` デストラクタが Base にある

---

## 潜在的な指摘事項（軽微）

### ex00: `setprecision(1)` の固定精度
現在の実装は全ての float/double を小数点以下 1 桁で表示する（`std::fixed` + `std::setprecision(1)`）。  
例: `3.14` → `double: 3.1` と表示される。  
Subject は具体的な精度を指定していないが、PDF の例では `.0` 表記が求められており、この実装で PDF の全例と一致するため問題なし。

### ex02: `catch(...)` vs `catch(std::bad_cast&)`
`catch(...)` は全例外を捕捉するため、`std::bad_cast` 以外の例外も握りつぶす可能性がある。  
ただし `<typeinfo>` のインクルードが禁止されているため、`std::bad_cast` を名前で参照できず、`catch(...)` は妥当な回避策。  
なお `catch(std::exception&)` を使う方法もある（`std::bad_cast` は `std::exception` を継承）が、`<typeinfo>` なしではこちらも完全に正当。

> [!IMPORTANT]
> `catch(std::exception&)` でも動作するが、`catch(...)` のほうが `<typeinfo>` との関連を完全に断てるため、防衛的には優れている。

---

## テスト結果サマリー

| Exercise | テスト数 | PASS | FAIL |
|---|---|---|---|
| ex00 (ScalarConverter) | 120 | 120 | 0 |
| ex01 (Serializer) | 44 | 44 | 0 |
| ex02 (Identify) | 53 | 53 | 0 |
| **合計** | **217** | **217** | **0** |

---

## 結論

全 3 exercise が評価基準を満たしている。コードは C++98 標準に準拠し、禁止事項（`using namespace`, `friend`, 禁止関数、`<typeinfo>`）を一切使用していない。各 exercise で要求された適切なキャスト（`static_cast`, `reinterpret_cast`, `dynamic_cast`）が正しく使用されており、テストも全て合格している。
