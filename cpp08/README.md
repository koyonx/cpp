# C++ Module 08 — Templated containers, iterators, algorithms

## このモジュールで学ぶこと

**STL (Standard Template Library) を実際に使う** モジュール:

| コンポーネント | ここで扱う代表例 | 対応 ex |
|---|---|---|
| **Containers** | `std::vector`, `std::list`, `std::deque`, `std::stack` | ex00/ex01/ex02 |
| **Iterators** | `iterator` / `const_iterator` / `reverse_iterator` | ex00/ex02 |
| **Algorithms** | `std::find`, `std::sort`, `std::min_element`, `std::max_element` | ex00/ex01 |
| **Iterator traits** | `std::iterator_traits`, `std::distance` | ex01 |
| **継承 STL** | `std::stack` を継承して機能拡張 | ex02 |

**Module-specific rule (PDF明示):** 「STL を使え、使わないと bad grade」。ex01/ex02 は STL なしでも実装可能だが、**あえて使わないと減点**。

---

## そもそも用語おさらい

### イテレータとは
コンテナの中の要素を「順番に指し示すオブジェクト」。**ポインタの一般化**。
- `container.begin()` → 最初の要素を指す iterator
- `container.end()` → **最後の次** を指す iterator (past-the-end)
- `++it` で次へ、`*it` で値、`it1 != it2` で比較

### イテレータのカテゴリ (C++98)
- **Input**: `*it` で読み取り一度きり、`++` のみ。例: `istream_iterator`
- **Output**: 書き込み一度きり、`++` のみ。例: `ostream_iterator`
- **Forward**: 前進のみ、複数回読み書き可。例: `forward_list` (C++11)
- **Bidirectional**: 前後移動可。例: `std::list`, `std::map`
- **Random Access**: `+n`, `-n`, `it[n]`, `it1 - it2` 可。例: `std::vector`, `std::deque`

### なぜ `std::stack` はイテレート不可なのか
- `std::stack` は **コンテナアダプタ** — 「LIFO 抽象」を提供するだけで、順序をランダムに走査させない設計思想
- 内部は `std::deque<T>` (デフォルト) を持つが、protected メンバ `c` として隠している
- ex02 では継承して `this->c` にアクセスし、`c.begin()`/`c.end()` を公開する

### `std::distance` の計算量
- Random Access iterator → **O(1)** (単純な減算)
- それ以外 → **O(n)** (`++it` を last まで回す)
- ex01 の `addNumbers` では `std::distance` で残容量チェックしている

### `std::iterator_traits<Iter>::difference_type`
- iterator の差分を表す型 (通常 `ptrdiff_t`)
- 汎用テンプレートで iterator の情報を取り出すための「型トレイト」

---

## 各 ex 詳細

### ex00 — easyfind

**目的:** 汎用 (テンプレート) の「container に int が含まれるか探す」関数。STL の `std::find` を活用する練習。

**実装:**
```cpp
template <typename T>
typename T::iterator easyfind(T& container, int value) {
    typename T::iterator it = std::find(container.begin(), container.end(), value);
    if (it == container.end())
        throw std::runtime_error("easyfind: value not found");
    return it;
}
```

**キーポイント:**
- **`typename`** が必要な理由: `T::iterator` は「T の依存名」で、コンパイラは `T` が確定するまで型か値か判別できない。明示的に「型だ」と伝えるため `typename` を書く
- **`std::find`** はどんな iterator でも動く汎用 algorithm。O(n) 線形探索
- **例外か戻り値か:** PDF は選択制。ここでは throw を選ぶ (「見つからない = 期待外れの状態」)
- **const overload:** const container からも呼べるように 2 種類用意

**落とし穴:**
- 見つからない時に `end()` iterator を返すと使用側でチェック忘れが発生する → throw の方が安全 (PDF 推奨)
- Associative container (`std::map` など) は key/value ペアなので今回対象外 (PDF 明示)

### ex01 — Span

**目的:** 最大 N 個の int を保持する Span クラス。10,000+ 要素で shortestSpan/longestSpan を効率的に。

**設計:**
```cpp
class Span {
    unsigned int        _capacity;
    std::vector<int>    _data;
public:
    Span(unsigned int n);
    void addNumber(int value);         // full なら throw
    template <typename Iter>
    void addNumbers(Iter first, Iter last);  // range 追加
    int shortestSpan() const;   // <2 要素なら throw
    int longestSpan() const;    // <2 要素なら throw
    class SpanFullException : public std::exception { ... };
    class NoSpanException  : public std::exception { ... };
};
```

**`shortestSpan()` の実装 (STL 全開):**
```cpp
std::vector<int> sorted(_data);
std::sort(sorted.begin(), sorted.end());
long shortest = LONG_MAX;
for (size_t i = 1; i < sorted.size(); ++i) {
    long diff = static_cast<long>(sorted[i]) - static_cast<long>(sorted[i-1]);
    if (diff < shortest) shortest = diff;
}
```
- ソート後の隣接ペア min diff = 最短スパン
- O(n log n)、直感的
- **オーバーフロー対策:** `INT_MIN` と `INT_MAX` の差は int で溢れるので `long` で計算

**`longestSpan()`:**
```cpp
int mn = *std::min_element(_data.begin(), _data.end());
int mx = *std::max_element(_data.begin(), _data.end());
return static_cast<int>(static_cast<long>(mx) - static_cast<long>(mn));
```
- O(n) を 2 回。実は 1 回のループでもできる (`std::minmax_element` は C++11)

**`addNumbers(Iter, Iter)`:**
```cpp
template <typename Iter>
void addNumbers(Iter first, Iter last) {
    diff_t extra = std::distance(first, last);
    if (_data.size() + extra > _capacity) throw SpanFullException();
    _data.insert(_data.end(), first, last);
}
```
- **`std::distance`** で範囲サイズを取得 → capacity 超過チェック
- **`std::vector::insert(pos, first, last)`** で一括挿入 (STL の range constructor パターン)
- **Transaction 保証:** サイズ超過を先にチェックすることで、部分挿入で中途半端な状態にならない

**落とし穴:**
- shortestSpan の diff 計算で int overflow に注意 (long キャスト)
- `std::vector::reserve(n)` を ctor で呼んでおくと後の push_back が高速

### ex02 — MutantStack

**目的:** イテレート不可な `std::stack` を継承して iterator を追加する。

**実装:**
```cpp
template <typename T, typename Container = std::deque<T> >
class MutantStack : public std::stack<T, Container> {
public:
    // OCF ...
    typedef typename Container::iterator                iterator;
    typedef typename Container::const_iterator          const_iterator;
    typedef typename Container::reverse_iterator        reverse_iterator;
    typedef typename Container::const_reverse_iterator  const_reverse_iterator;

    iterator begin() { return this->c.begin(); }  // c は std::stack の protected メンバ
    iterator end()   { return this->c.end(); }
    // rbegin/rend/const 版も同様
};
```

**キーポイント:**
- **`std::stack::c`** は protected メンバ (基底コンテナへの参照)。継承すればアクセスできる
- **`this->c`** と書く理由: **依存名の探索**。テンプレート内で `c` と書くと「呼び出しコンテキストで探せ」となるので、明示的に `this->c` として「派生クラスの scope から」と伝える
- **iterator の typedef** で外から使いやすくする。`std::deque<T>::iterator` は random access iterator なので `+n`, `-n` 演算可能
- **すべての stack member (push/pop/top/size/empty など)** はそのまま継承されるので実装不要

**PDF 要件:**「MutantStack を std::list に置き換えても同じ出力」→ my test では `collectMutant()` と `collectList()` の文字列比較で厳密検証

**落とし穴:**
- `this->` を省くと `-Werror` でコンパイルエラーになることが多い (依存名の探索問題)
- `std::stack` は container adapter なので typename に注意:
  ```cpp
  typedef typename Container::iterator iterator;  // typename 必須
  ```
- iterator の traversal 順は「底 → top」。stack として push した順の逆ではない
  - `push(1); push(2); push(3);` → iterator: `1, 2, 3` (bottom-up), reverse: `3, 2, 1`

---

## コンパイル・実行

各 ex ディレクトリで:
```bash
make       # ビルド
make re    # クリーンビルド
./easyfind    # ex00 テスト
./span        # ex01 テスト
./mutantstack # ex02 テスト
```

## テストカバレッジ

| ex | assertions | valgrind |
|---|---|---|
| ex00 | 24 | 0 leaks / 0 errors |
| ex01 | 42 | 0 leaks / 0 errors |
| ex02 | 33 | 0 leaks / 0 errors |
| **合計** | **99** | **all clean** |

### 主な検証項目
- **ex00**: vector/list/deque での find、found/not-found、境界 (INT_MIN/MAX)、const container、重複値、10,000-elem 大配列、10,000 iteration leak 耐性
- **ex01**: PDF 例 (Span(5) with {6,3,17,9,11} → shortest=2, longest=14)、full 時 throw、<2要素 throw、Span(0)/Span(1) 縁ケース、同値 shortest=0、負の値、INT_MIN/MAX の overflow-safe 計算、range-based addNumbers (vector/list/deque)、部分挿入時の transaction 保証、100,000 要素、1000 lifecycle
- **ex02**: PDF 例 exact 再現、std::list との出力一致、std::stack API 継承 (push/pop/top/size/empty)、begin/end/rbegin/rend/const 版、random access iterator arithmetic、`std::find` との interop、vector-backed MutantStack、10,000 push + iterate、10,000 push/pop cycle

---

## レビュー時に聞かれそうなこと

- **`std::stack::c` にアクセスできる理由は？** → `c` は protected メンバなので基底クラスからは触れないが、**public 継承した派生クラス** からはアクセス可能
- **なぜ `this->c` と書く？** → テンプレートクラス内で継承した protected/private メンバを参照するとき、**依存名の探索**の関係で `this->` を付けないと非依存扱いになり見つからないことがある (C++ 標準の two-phase name lookup)
- **`typename Container::iterator` の `typename` は何のため？** → `Container::iterator` は「Container の依存名」で、コンパイラは型か static メンバか判別できないため、明示的に「型宣言だ」と伝える
- **`std::find` は何 iterator category を要求する？** → **input iterator** で十分 (前進と読み取りだけ)
- **`std::sort` は何 iterator category を要求する？** → **random access iterator** (要素の位置スワップに必要)
  - なので `std::list::sort()` は自前で持っている (`std::sort` は list に使えない)
- **`std::vector::reserve()` と `resize()` の違いは？** → `reserve(n)` は capacity を n に確保するだけ、size は変わらない。`resize(n)` は実際に n 個の要素を持つように調整
- **Container adapter とは？** → `std::stack`, `std::queue`, `std::priority_queue` の 3 つ。既存 container を wrapping して特定のアクセスパターンを提供する
- **shortestSpan の O(n log n) はもっと速くできる？** → 一般には無理。`sort` を使う実装が標準。数字が small range なら bucket sort で O(n) 可能

## 参考リンク

- [cppreference: containers](https://en.cppreference.com/w/cpp/container)
- [cppreference: std::find](https://en.cppreference.com/w/cpp/algorithm/find)
- [cppreference: std::sort](https://en.cppreference.com/w/cpp/algorithm/sort)
- [cppreference: std::stack](https://en.cppreference.com/w/cpp/container/stack)
- [cppreference: std::iterator_traits](https://en.cppreference.com/w/cpp/iterator/iterator_traits)
- [cppreference: dependent names](https://en.cppreference.com/w/cpp/language/dependent_name)
