# C++ Module 09 — STL 実践

cpp09 は 42-cpp の **最終モジュール**。特に ex02 (PmergeMe / Ford-Johnson) は**歴代コンピュータ科学のトップクラスに難しい古典アルゴリズム**。ここではまず「なぜ難しいのか」から丁寧に解説します。

## Module-specific rule (最重要)

**「一度使ったコンテナは以降の ex で使えない」** — 各 ex は異なる container を選ぶ必要がある。

| ex | 使用コンテナ | 選んだ理由 |
|---|---|---|
| ex00 BitcoinExchange | `std::map<std::string, double>` | 日付の順序性を活かした O(log n) の closest-lower-date 検索 (`lower_bound`) |
| ex01 RPN | `std::stack<int>` | RPN 評価の「push 数値、演算子で pop 2 個」構造そのもの |
| ex02 PmergeMe | `std::vector<int>` **と** `std::deque<int>` | 2 つの container で同じソートを実装、性能比較 |

---

## そもそも用語おさらい

### `std::map` の内部と `lower_bound` / `upper_bound`
- 内部は **赤黒木 (Red-Black Tree)** — 常に平衡が保たれる二分探索木
- 挿入/検索/削除すべて **O(log n)**
- キーで自動的にソート済み (デフォルトは `operator<`)
- `lower_bound(key)`: 「key **以上**」の最初の要素を指す iterator (見つからなければ `end()`)
- `upper_bound(key)`: 「key **より大** (超過)」の最初の要素

**「date 以下の最も新しい日付」を探す典型テクニック:**
```cpp
auto it = m.lower_bound(date);           // date 以上の最初
if (it == m.end() || it->first != date)  // 完全一致じゃなかった
    --it;                                 // 一つ前 = date 未満の最新
return it->second;                        // 対応する rate
```

### `std::stack` の内部
- **container adapter** — 内部コンテナはデフォルトで `std::deque<T>`
- 本実装では `std::stack<int, std::list<int> >` として中身を `std::list` に明示している
  (ex02 で `std::deque` を使うため、「同じコンテナを使っている」と言われないように)
- 提供する操作: `push`, `pop`, `top`, `size`, `empty` のみ
- RPN 評価と完全に相性が良い: 「数値見たら push、演算子見たら pop×2 して計算して push」

### `std::vector` vs `std::deque`
| 特徴 | `std::vector` | `std::deque` |
|---|---|---|
| メモリ配置 | 単一連続領域 | 複数の固定サイズブロック (通常 4KB × N) |
| `push_back` | 償却 O(1)、時に realloc (全コピー) | O(1)、確実に途中コピーなし |
| `push_front` | ✗ (O(n)) | ✓ O(1) |
| ランダムアクセス | O(1) (ポインタ算) | O(1) (だが計算がちょっと重い) |
| iterator invalidation | insert で全部無効化 (realloc 時) | 中央 insert で無効化、両端は安全 |

**なぜ ex02 で両方使う？** 同じアルゴリズムを 2 つの container で書くと、内部レイアウトの違いによる **実行時間差**が観察できる。教育的意義。

---

## 各 ex 詳細

### ex00 — BitcoinExchange

**目的:** CSV データベースから日付ごとの為替レートを読み込み、入力ファイルの `date | value` に対して `value × rate` を計算して表示。

**入力仕様 (PDF):**
```
date | value
2011-01-03 | 3
2011-01-09 | 1.2
2012-01-11 | -1        ← Error: not a positive number.
2001-42-42             ← Error: bad input => 2001-42-42
2012-01-11 | 2147483648 ← Error: too large a number.
```

**「closest lower date」の実装:**
- `std::map` は date を key として lexicographic order で保存
- `YYYY-MM-DD` 形式は「**辞書順 = 時系列順**」の性質を持つ (padding が固定なので)
- `lower_bound(target_date)` で「以上の最初」を取得
- そこから 1 つ戻れば「target 以下の最新」

**日付検証 (`isValidDate`):**
- 長さ 10 文字、`YYYY-MM-DD` フォーマット
- 月 1-12、日は月別 (31/28/29/30/31/30/31/31/30/31/30/31)
- **うるう年判定:** `(y % 4 == 0 && y % 100 != 0) || y % 400 == 0`
  - 4 の倍数だが 100 の倍数は普通の年、ただし 400 の倍数はうるう年
  - 1900 年 → うるう年ではない、2000 年 → うるう年

**エラー分岐:**
- `Error: could not open file.` — ファイルなし
- `Error: not a positive number.` — value < 0
- `Error: too large a number.` — value > 1000
- `Error: bad input => <line>` — フォーマット不正 / 日付不正 / `nan`
- `Error: no data available for this date => <date>` — DB の最古の日付より前 (「直近の過去」が存在しない)

**出力精度:** デフォルト (6 桁) だと `1000 * 47115.93` が `4.71159e+07` になるので `std::setprecision(10)` を指定

### ex01 — RPN (Reverse Polish Notation)

**目的:** 逆ポーランド記法の式を評価。

**RPN とは？**
- 演算子を **オペランドの後ろに** 書く記法
- 通常記法 `3 + 4` は RPN で `3 4 +`
- 通常記法 `(3 + 4) * 5` は RPN で `3 4 + 5 *`
- **括弧が不要** — 優先順位が式の順序に暗黙的に反映される

**評価アルゴリズム (stack を使う):**
```
1. 空の stack を用意
2. tokenごとに:
   - 数値なら stack に push
   - 演算子なら:
       b = stack.top(); stack.pop()
       a = stack.top(); stack.pop()
       result = a op b
       stack.push(result)
3. 最終的に stack に 1 個だけ残った値が答え
```

**オーバーフロー:** 演算は `long` で計算し、`int` の範囲外ならエラー (int のオーバーフローは未定義動作)

**注意:**
- pop の順序に注意: `a b -` は `a - b` (先に push した a が下)
- 除算/減算のような**非可換演算**でバグりやすい

**PDF 制約:**
- 数字は 10 未満 (1桁のみ)
- 中間結果と最終結果は 10 以上でも OK
- 演算子は `+ - * /` のみ

### ex02 — PmergeMe (Ford-Johnson / merge-insertion sort)

**目的:** Ford-Johnson アルゴリズムで正整数列をソート。2 種類の container で実装。

**Ford-Johnson とは何か？なぜ重要か？**

これは **1959 年に Lester Ford Jr. と Selmer Johnson が発表した** ソートアルゴリズム。特徴は:

> **比較回数が理論的な下限 `⌈log₂ n!⌉` に非常に近い**。n ≤ 11 では比較回数が最適 (それ以上少ない比較で必ずソートできるアルゴリズムは存在しない)。

- QuickSort/MergeSort は O(n log n) だが比較回数の**定数倍**は大きい
- Ford-Johnson は「必要最小限の比較で全順序を確定させる」ことに特化
- ただし挿入で要素をずらすコストが大きいので、実用上は速くない (比較が非常に高価な場面向け)

**なぜ難しいのか？**
- 単純な merge sort に「Jacobsthal 数を使った挿入順序決定」という組合せ論的な最適化が加わる
- 「pair の larger を再帰ソート → smaller を binary insertion で挿入」という 2 相構造
- 挿入順序を最適化する Jacobsthal 数列 (1, 3, 5, 11, 21, 43, ...) は「Fibonacci 的な漸化式」

#### アルゴリズム step-by-step (例: `[3, 5, 9, 7, 4]`)

要素は値ではなく **元配列の添字** で持ち回る (再帰の後に「どの a とどの b がペアか」を値で探すと、重複値で曖昧になり、余計な比較も発生するため)。説明では値で書く。

**Step 0: 奇数個なら最後の 1 個 (straggler) はペアを組まない**
```
input: [3, 5, 9, 7, 4]  → straggler = 4 (相方の a がいない b として最後に扱う)
```

**Step 1: 2 個ずつペアにして 1 回ずつ比較 (大きい方 a、小さい方 b)**
```
(3, 5) → a=5, b=3
(9, 7) → a=9, b=7          ← 比較 2 回
```

**Step 2: a だけを再帰的にソート**
```
[5, 9] → [5, 9]             ← 比較 1 回
a1=5 (相方 b1=3), a2=9 (相方 b2=7), b3=4 (straggler)
```

**Step 3: main chain S = [b1, a1, a2]**
```
S = [3, 5, 9]   ← b1 < a1 <= 他の a なので b1 は比較なしで先頭
```

**Step 4: 残りの b を Jacobsthal 順に二分挿入 (探索範囲は「相方 a より左」だけ)**
```
グループ (1, 3]: b3, b2 の順
  b3=4 (straggler): S 全体 [3, 5, 9] (3 個) を探索 → 2 回 → [3, 4, 5, 9]
  b2=7: a2=9 より左 [3, 4, 5] (3 個) を探索    → 2 回 → [3, 4, 5, 7, 9] ✓
合計 2 + 1 + 2 + 2 = 7 回 = Ford-Johnson の n=5 の最悪値 F(5)
```

`ex02_trace` で任意の入力の途中経過 (各挿入の探索範囲・比較回数) を見られる。

#### Jacobsthal 数について

**定義:** `J₀ = 0, J₁ = 1, Jₙ = Jₙ₋₁ + 2·Jₙ₋₂`
**数列:** `0, 1, 1, 3, 5, 11, 21, 43, 85, 171, 341, 683, 1365, ...`

**なぜ挿入順序に Jacobsthal を使う？**

- 二分探索は **2ᵏ - 1 個の範囲ならちょうど k 回** で位置が決まる (2ᵏ 個だと k+1 回必要になることがある)
- b_j は b_j < a_j が既知なので「a_j より左」だけを探せばよい
- グループ (J_{k-1}, J_k] を **番号の大きい方から** 挿入すると、どの b を入れるときも探索範囲が `2ᵏ - 1` 個以下になる
  - 例: b3 の範囲は [b1, a1, a2] の 3 個。次に b2 を入れるとき、b3 が a2 の右に入っていれば範囲は 2 個、左に入っていれば 3 個 → どちらでも 2 回
  - 番号の小さい方から入れると、先に入れた b が後の探索範囲を広げてしまい、比較が 1 回増えることがある

挿入順序:
- Group 1: `b₃, b₂` (探索範囲 ≤ 3 → 2 回)
- Group 2: `b₅, b₄` (≤ 7 → 3 回)
- Group 3: `b₁₁, b₁₀, ..., b₆` (≤ 15 → 4 回)
- Group 4: `b₂₁, b₂₀, ..., b₁₂` (≤ 31 → 5 回)
- ...

**最悪比較回数:** `F(n) = Σ_{k=1..n} ⌈log₂(3k/4)⌉` (n=5:7, 10:22, 12:30, 21:66, 22:71)。
テスト 47/48 で比較回数を数え、n=1..64 で F(n) 以下・n=5 の全順列で最悪値がちょうど 7 であることを確認している。

#### 実装のキーポイント (`PmergeMe.cpp`)

```cpp
// idx: values の添字列。values[idx[i]] の昇順に並べ替える
void PmergeMe::mergeInsertVector(const std::vector<int>& values, std::vector<std::size_t>& idx) {
    if (idx.size() <= 1) return;
    bool hasStraggler = (idx.size() % 2 == 1);
    std::size_t straggler = hasStraggler ? idx.back() : 0;

    // Step 1: ペアごとに 1 回比較。partner[a] = b を覚える
    for (i < m) { x = idx[2i]; y = idx[2i+1]; if (isLess(values[x], values[y])) swap(x, y);
                  winners.push_back(x); partner[x] = y; }

    // Step 2: a だけ再帰
    mergeInsertVector(values, winners);

    // Step 3: S = [b1, a1, ..., am]、aPos[i] = S 内の a_(i+1) の位置
    // Step 4: Jacobsthal グループごとに大きい番号から
    //   bound = (straggler なら S.size()) : aPos[i]   ← a_i より左だけ探索
    //   p = binaryInsertVector(values, S, x, bound);
    //   挿入位置 p 以降の a は 1 つずれる → aPos を更新
    idx = S;
}
```

**Container ごとに実装する理由 (PDF明示):**
- `std::vector` と `std::deque` は内部構造が違うので、**同じロジックでも実行時間が違う**
- `template<Container>` で一本化すると「両方使ってる」感が薄れる → PDF は分離を推奨
- 実測 (macOS, 3000 要素): vector ≈ 13ms, deque ≈ 20ms
  - どちらも途中 insert は要素をずらす O(n)。vector は連続メモリの memmove で速い
  - deque は近い端の側をずらすので移動量は少ないが、ブロックをまたぐ要素単位のコピーとインデックス計算が重い

**タイミング取得:**
- `<sys/time.h>` の `gettimeofday()` を使ってマイクロ秒精度
- C++11 の `<chrono>` は禁止 (C++98 縛り)
- `clock()` は分解能が粗い (通常 1ms 単位) ので採用しない

**エラー処理:**
- 引数が全て正整数 (0 以上) かチェック
- 非数値、負数、`INT_MAX` 超えは `Error` を stderr へ

---

## コンパイル・実行

各 ex ディレクトリで:
```bash
make       # ビルド
./btc input.txt       # ex00 (data.csv が同じディレクトリに必要)
./RPN "3 4 +"         # ex01
./PmergeMe 3 5 9 7 4  # ex02

# 内蔵テストスイート実行
./btc --test
./RPN --test
./PmergeMe --test
```

## テストカバレッジ

| ex | assertions | valgrind |
|---|---|---|
| ex00 BitcoinExchange | 113 | 0 leaks / 0 errors |
| ex01 RPN | 76 | 0 leaks / 0 errors |
| ex02 PmergeMe | 59 | 0 leaks / 0 errors |
| **合計** | **248** | **all clean** |

### 主な検証項目

**ex00 (113 assertions)**
- `isValidDate`: 5 valid + 14 invalid (short/long, non-digit, month 0/13, day 0/32, non-leap-year Feb 29, 1900 vs 2000 leap ルール)
- `parseValue`: int/float/科学記法、空文字/letters/trailing garbage/leading space の rejection
- `loadDatabase`: 実 data.csv + missing file + mock CSV
- `getRate`: exact match、closest lower、before-all-entries、after-all-entries
- `processInput`: PDF 例の値/エラー全一致、missing file、empty file、負数/1001/1000境界/no-separator/bad-date/bad-value 各分岐
- OCF、100 cycles leak、3000+ 大 DB
- DB より古い日付のエラー、大きな結果が指数表記にならない、`nan` の拒否

**ex01 (76 assertions)**
- PDF 例 3 種 (`42`, `42`, `0`) 厳密一致 + error case (`(1+1)`)
- 4 演算子基本、単一トークン、負の中間、整数除算切り捨て、0 除算
- 不足オペランド、過剰オペランド、空/空白のみ、多桁数字、%/^ 未対応演算子、letters
- 中間 > 9 (PDF: 中間結果はルール適用外)、負中間の伝播、evaluate state reset
- OCF、10k evaluations、全演算子式、単項マイナス reject、連続演算子、0 operand
- int オーバーフロー (9^11) のエラー

**ex02 (59 assertions)**
- Empty、単一、2 要素、PDF 例 `3 5 9 7 4 → 3 4 5 7 9`
- 既ソート、逆順、全同値、多 duplicates
- **奇数個 (straggler)**: 1,3,5,7,9,11 全パス
- 偶数個: 2..12 全パス
- vector と deque の結果一致
- **std::sort との一致**: 25/100(×10)/3000/**10,000** 全一致
- boundary (`INT_MAX`, 0)、n=1..20 全網羅
- parseInput 検証: valid/negative/non-numeric/empty/overflow/no-args/0-accepted
- OCF、sort 冪等性、多 duplicates、`sortAndReport` PDF 形式
- **比較回数**: n=1..64 で Ford-Johnson の最悪値 F(n) 以下 (vector/deque)、n=5 全 120 順列で最悪値がちょうど 7

---

## レビュー時に聞かれそうなこと

- **`std::map` の内部データ構造は？** → **赤黒木 (Red-Black Tree)**。挿入/検索/削除すべて O(log n)、常に平衡が保たれる
- **`lower_bound` と `upper_bound` の違いは？** → `lower_bound(k)` は「k **以上**」の最初、`upper_bound(k)` は「k **より大**」の最初。境界値を含めるかの違い
- **RPN で `3 4 -` の答えは？** → `-1` (pop 順で `a=3, b=4`, 計算は `a - b = 3 - 4 = -1`)。RPN では**後ろから読むと通常記法になる**
- **なぜ ex02 で container を 2 つ？** → PDF 要件。両方で同じ結果、実行時間の違いを見るため
- **Ford-Johnson の計算量は？** → **O(n log n)** (漸近的には merge sort と同等) だが**比較回数の定数倍が最小**。特に n ≤ 11 では comparison-optimal
- **Jacobsthal 数列の役割は？** → 挿入順序を決めることで、各 binary insertion の探索範囲を常に `2ᵏ-1` に保つ → 比較回数を最小化
- **`std::vector` と `std::deque` はどちらが速い？** → 今回は vector (3000 要素で約 13ms vs 20ms)。途中 insert はどちらも O(n) だが、vector は連続メモリなので memmove 一発でずらせてキャッシュにも乗る。deque は近い端をずらすので移動量は約半分だが、ブロック単位の管理と添字計算のオーバヘッドが勝つ
- **なぜ添字で持ち回る？** → 再帰後に「a とペアだった b」を値で探すと、重複値で曖昧になる上に余計な比較 (と O(n²) の探索) が発生する。添字なら `partner[a]` で O(1)
- **比較回数が Ford-Johnson どおりだと示せる？** → `./PmergeMe --test` のテスト 47/48。全比較を `isLess` に通して数え、理論値 F(n) 以下を確認している
- **なぜ template ではなく 2 実装を書く？** → PDF が明示: 「container ごとに実装せよ」。教育的に「同じアルゴリズムでも container が違えば性能が変わる」ことを実感するため
- **`std::sort` を使えば済まない？** → その通り。だが本モジュールは **STL container を意識的に使う**、**古典アルゴリズムを自力で書く** ことが目的
- **timing に `gettimeofday` を使う理由は？** → C++98 の範囲でマイクロ秒精度を得るため。`<chrono>` は C++11。`clock()` は分解能が粗い

## 参考リンク

- Ford, L. R. Jr. & Johnson, S. M. (1959). "A Tournament Problem". *American Mathematical Monthly*, 66(5), 387-389.
- Knuth, D. E. (1998). *The Art of Computer Programming, Volume 3: Sorting and Searching* (2nd ed.), Chapter 5.3.1 "Merge Insertion", pp. 184-186.
- [Wikipedia: Merge-insertion sort](https://en.wikipedia.org/wiki/Merge-insertion_sort)
- [Jacobsthal number - OEIS A001045](https://oeis.org/A001045)
- [cppreference: std::map](https://en.cppreference.com/w/cpp/container/map)
- [cppreference: std::stack](https://en.cppreference.com/w/cpp/container/stack)
- [cppreference: std::vector vs std::deque](https://en.cppreference.com/w/cpp/container)
