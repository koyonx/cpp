# C++ Module 07 — C++ templates

## このモジュールで学ぶこと

C++ の **テンプレート機構** — 型を後から差し込める「型に対する関数/クラスの雛形」を書く技法:

| 分類 | 使いどころ | 本モジュールでの例 |
|---|---|---|
| **関数テンプレート (function template)** | 型ごとに同じロジックを書きたい関数 | ex00: `swap<T>`, `min<T>`, `max<T>`, ex01: `iter<T,F>` |
| **クラステンプレート (class template)** | 型ごとに同じ構造を持つコンテナ | ex02: `Array<T>` |
| **テンプレート推論 (deduction)** | 呼び出し引数から型を自動導出 | `::swap(a, b)` — 型明示しなくても T が確定 |
| **明示的テンプレート引数** | 曖昧な時や意図的に型を選びたい時 | `::min<int>(a, b)` |

C++98 制約:
- STL コンテナ (vector/list/map…) と `<algorithm>` は禁止 → 自作 Array で「なぜ STL があるのか」を体感する
- テンプレートは基本的にヘッダに書く（`.hpp` or `.tpp` に切り出し可）
- `friend` / `using namespace` 禁止

---

## そもそも用語おさらい

### テンプレートとは
「型を後から埋める **雛形**」。
```cpp
template <typename T>
void swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }
```
これは 1 つの関数ではなく「T を差し込むと関数を作れるレシピ」。呼び出し側で `swap(1, 2)` と書くと、コンパイラが `T = int` を推論して `swap<int>` を実体化する。

### なぜテンプレートは **ヘッダに書く** 必要があるのか
- テンプレート自体は「関数ではなく雛形」なので、使う側 (別の .cpp) がテンプレート定義を見えないと実体化できない
- 別 .cpp に書くと link 時に「そんな関数ないよ」となる
- 対策: 全部ヘッダに書く。分割したいときは `.tpp` に書いて `.hpp` の末尾で `#include "X.tpp"`
- 42 PDF も「Templates must be defined in the header files」と明示

### 型推論 vs 明示指定
- `::min(a, b)` — 引数から T を自動推論
- `::min<int>(a, b)` — 明示的に T=int
- 曖昧な時 (両引数の型が違う等) は明示で解決

### `new T[n]()` の `()` の意味
```cpp
int* p = new int();     // *p == 0 (value-initialized)
int* q = new int;       // *q == 未定義 (default-initialized for POD)
int* r = new int[5]();  // 全要素 == 0
int* s = new int[5];    // 全要素未定義 (POD の場合)
```
PDF ex02 Tip: 「int* a = new int(); *a を表示してみて」→ **0**。組み込み型でも `()` を付ければゼロ初期化される。Array<T>(n) の要素をデフォルト値にしたいなら `new T[n]()` を使う。

---

## 各 ex 詳細

### ex00 — swap / min / max function templates

**目的:** 関数テンプレートの基本。任意の型に対して動作する swap/min/max を書く。

**実装のキーポイント:**
```cpp
template <typename T>
void swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }

template <typename T>
const T& min(const T& a, const T& b) { return (a < b) ? a : b; }

template <typename T>
const T& max(const T& a, const T& b) { return (a > b) ? a : b; }
```

**PDF ルール:**
- swap は返却なし、値を入れ替える
- min/max は「等しいなら **2 番目** を返す」→ `(a < b) ? a : b` は等しいとき false → b を返す ✓

**落とし穴:**
- swap の引数は `T&` (非 const)。`const int c; ::swap(c, ...)` はコンパイルエラー
- min/max は const 参照返し。`::min(1, 2)` のような一時値でも安全 (full-expression 終了まで生存)
- `::swap` の `::` はグローバル名前空間を明示。`std::swap` (C++11 は `<utility>`) と衝突を避ける意図もある
- 呼び出しは `::swap(a, b)` — 型推論に任せる。明示したいときは `::swap<int>(a, b)`

### ex01 — iter function template

**目的:** 配列と関数を受け取り、各要素に関数を適用する高階関数。「テンプレートで const/非-const 両対応する方法」を学ぶ。

**実装:**
```cpp
template <typename T, typename F>
void iter(T* array, size_t const length, F func) {
    for (size_t i = 0; i < length; ++i)
        func(array[i]);
}
```

**なぜこれで const/非-const 両対応できるのか:**
- 呼び出し側が `int arr[5]` を渡すと T=int, T*=int*, `array[i]` は int&
- 呼び出し側が `const int arr[5]` を渡すと T=const int, T*=const int*, `array[i]` は const int&
- 関数 F がその型を受け付けられれば OK

**PDF ヒント:**「third parameter can be an instantiated function template」 — つまり `iter(arr, 5, printAny<int>)` みたいに、関数テンプレートを型指定して渡せる (関数テンプレート自体は関数ポインタになれないので、実体化した瞬間から関数ポインタ)。

**落とし穴:**
- 関数テンプレートを直接渡すと「どのインスタンス？」で曖昧になる。明示的に `printAny<int>` と型指定
- `size_t const length` の const は PDF 明示要件
- F を関数ポインタ型で受けると柔軟性が落ちる。テンプレート型 F にすれば functor / lambda 相当 / 関数ポインタ / 関数テンプレート実体化を全部受け入れられる

### ex02 — Array<T> class template

**目的:** クラステンプレートで自作コンテナを作る。**STL 禁止環境**で「なぜ vector が便利なのか」を体感。

**設計:**
```cpp
template <typename T>
class Array {
    T*              _data;
    unsigned int    _size;
public:
    Array();                            // empty
    Array(unsigned int n);              // n-element, default-init
    Array(const Array<T>& other);       // deep copy
    Array<T>& operator=(const Array<T>& other);
    ~Array();
    T&          operator[](unsigned int index);
    const T&    operator[](unsigned int index) const;
    unsigned int size() const;
    class OutOfBoundsException : public std::exception { ... };
};
```

**キーポイント:**
- **`new T[n]()`** で全要素をデフォルト初期化 (int なら 0, string なら空)
- **Deep copy**: コピーコンストラクタ / operator= で `new[]` + ループコピー
- **operator[] は 2 バージョン**: 非-const (書き込み) と const (読み取り)
- **範囲外 → 例外**: PDF は `std::exception` を投げよと言っているので、`OutOfBoundsException : public std::exception` を用意
- **`.tpp` パターン**: `Array.hpp` はクラス宣言のみ、末尾で `#include "Array.tpp"`。実装は `.tpp` へ。読みやすさ向上

**落とし穴:**
- **`Preventive allocation is forbidden`**: 事前に多めに確保するな、と PDF が言っている。素朴な「n 個ぴったり new」で OK
- **`unsigned int index`**: 負のインデックス (`a[-1]`) は unsigned に暗黙変換され超巨大な値になる → 自動的に out-of-bounds → 例外
- **operator=**: `if (this != &other)` の自己代入チェック。`delete[]` してから `new[]`
- **例外安全性**: `new` 失敗時のリソースリークは今回は単純な実装で無視 (strong exception guarantee ではない)
- **`Array<Array<T> >` (nested)**: スペースが必要 (`>>` は C++11 まで shift 演算子として parse される)

---

## コンパイル・実行

各 ex ディレクトリで:
```bash
make          # ビルド
make clean    # .o 削除
make fclean   # 実行ファイルも削除
make re       # fclean → all
```

**注意:** ex02 の Makefile は `-I.` を追加している。PDF 提供 main.cpp が `#include <Array.hpp>` (角括弧) を使うため、include search path にカレントディレクトリを追加している。

### 実行

```bash
./whatever   # ex00 — 41-assertion テストスイート
./iter       # ex01 — 17-assertion テストスイート
./array      # ex02 — 37-assertion テストスイート (PDF 提供テスト込み)
```

## テストカバレッジ

| ex | assertions | valgrind |
|---|---|---|
| ex00 | 41 | 0 leaks / 0 errors |
| ex01 | 17 | 0 leaks / 0 errors |
| ex02 | 37 | 0 leaks / 0 errors |
| **合計** | **95** | **all clean** |

### 主な検証項目
- **ex00**: PDF int 例と string 例、swap/min/max を int/double/char/string/user-defined class で検証、min/max 等値時に **2 番目引数のアドレスが返る** ことを `&result == &b` で厳密検証、INT_MIN/MAX 境界、chain (`::min(::min(a,b),c)`), 明示 template 引数 `<int>`, 自己 swap 安全性
- **ex01**: mutating (doublify) / non-mutating (addToSum), const 配列, 空配列 (length 0), 大配列 (10,000), 関数テンプレート実体化 (`printAny<int>`), 順序保証 (0..N-1), const/非-const 混在, `size_t const length`
- **ex02**: PDF 提供 750要素テスト、デフォルト空 array, `Array(n)` の zero-init 検証 (int/double/string), `operator[]` 読み書き, out-of-bounds throw (境界=size / huge / negative=-1), 空 array の任意 index throw, const `operator[]`, copy ctor 独立性 (source ↔ copy 相互影響なし), operator= 独立性 + 自己代入 + chained + size 更新, `size()` const-callable, 10,000 lifecycle と 10,000 copy の leak 耐性, nested `Array<Array<int> >`, `Array<int>(0)` = 空

---

## レビュー時に聞かれそうなこと

- **なぜテンプレートはヘッダに書く？** → テンプレートは「関数ではなくレシピ」。実体化時に定義が見えていないと link できない。翻訳単位ごとに実体化される
- **`typename` と `class` の違いは？** → 関数/クラステンプレートの引数宣言では **同じ意味**。歴史的経緯で `class` が先、後から `typename` が追加。今は `typename` が推奨されがち (依存名の宣言でも使うので統一しやすい)
- **`inline` を付けなくてもテンプレートはヘッダに定義できる理由は？** → テンプレートの実体化は「暗黙的にインライン扱い」される (one-definition-rule 上の緩和)
- **`::swap` と `std::swap` の違いは？** → `::` はグローバル名前空間。`std::swap` は C++ 標準ライブラリの swap (C++11 は `<utility>` 経由)。42 の PDF では自作 `::swap` を要求している
- **`new T[n]()` の `()` は？** → 各要素を **値初期化 (value-initialization)**。組み込み型は 0、クラス型はデフォルトコンストラクタが呼ばれる。`()` を省くと組み込み型はゴミ値になる可能性
- **PDF の「Preventive allocation is forbidden」とは？** → 「予備メモリを多めに確保して amortized で伸ばす」ような最適化を禁止。std::vector の内部 capacity 拡張のような仕組みを避ける、という意味。素直に n 個ぴったり確保する
- **なぜ `iter` は 1 つの template で const/非-const 両対応できるのか？** → 型推論で T が自動的に const-qualified になるから。`iter(const_arr, 5, f)` → T=const int → `array[i]` は const int&

## 参考リンク

- [cppreference: function template](https://en.cppreference.com/w/cpp/language/function_template)
- [cppreference: class template](https://en.cppreference.com/w/cpp/language/class_template)
- [cppreference: template argument deduction](https://en.cppreference.com/w/cpp/language/template_argument_deduction)
- [cppreference: value initialization](https://en.cppreference.com/w/cpp/language/value_initialization)
- [Why templates must be in headers](https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl)
