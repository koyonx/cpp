# ex02_trace — Ford-Johnson の途中経過を可視化する版

`ex02` の PmergeMe (Ford-Johnson merge-insertion sort) は仕組みが複雑で、実行結果だけ見ても「なぜソートされるのか」が分かりづらいので、**アルゴリズムの各ステップを step-by-step で出力する** 別バージョンをここに用意しました。

- 提出用ではなく **教育用** の別実装 (ex02 のロジックはそのまま、トレース出力を差し込んだだけ)
- 再帰の深さで indent される
- 各ステップに `[depth N] Step X - <説明>` のラベル

## ビルド・実行

```bash
make
./trace_sort              # 引数なし → PDF 例 [3, 5, 9, 7, 4]
./trace_sort 8 5 3 1 4 2  # 好きな数列を渡す
```

## 出力例 (`./trace_sort` = PDF 例 `[3, 5, 9, 7, 4]`)

```
[depth 0] Input: [3, 5, 9, 7, 4]
[depth 0] Odd size -> straggler = 4 (treated as the last b, with no partner a)
[depth 0] Step 1 - Pairs (a=larger, b=smaller): (5,3) (9,7)   [2 comparisons]
[depth 0] Step 2 - Recursing on a's: [5, 9]
  [depth 1] Input: [5, 9]
  [depth 1] Even size, no straggler
  [depth 1] Step 1 - Pairs (a=larger, b=smaller): (9,5)   [1 comparisons]
  [depth 1] Step 2 - Recursing on a's: [9]
    [depth 2] Input: [9]
    [depth 2] size <= 1, return unchanged
  [depth 1] Step 2 done - a's sorted: [9]
  [depth 1] Step 3 - Main chain [b1, a1..a1]: [5, 9]   (b1=5 < a1, so no comparison needed)
  [depth 1] Pending b's:
  [depth 1] Final at this depth: [5, 9]
[depth 0] Step 2 done - a's sorted: [5, 9]
[depth 0] Step 3 - Main chain [b1, a1..a2]: [3, 5, 9]   (b1=3 < a1, so no comparison needed)
[depth 0] Pending b's: b2=7(<a2=9) b3=4(straggler)
[depth 0] Step 4 - Jacobsthal group (1, 3]: insert b3 down to b2
    b3=4: search [3, 5, 9] (3 elements, whole chain)
      -> index 1 (2 comparisons): [3, 4, 5, 9]
    b2=7: search [3, 4, 5] (3 elements, left of a2)
      -> index 3 (2 comparisons): [3, 4, 5, 7, 9]
[depth 0] Final at this depth: [3, 4, 5, 7, 9]

Sorted output: [3, 4, 5, 7, 9]
Total comparisons: 7
```

## 各ステップの読み方

| ステップ | 説明 | この例での動き |
|---|---|---|
| **Straggler** | 奇数個なら末尾 1 個はペアを組まず、相方のいない最後の b として扱う | `4` |
| **Step 1: Pair up** | 2 個ずつ組んで 1 回比較。大きい方 a、小さい方 b | `(5,3)`, `(9,7)` |
| **Step 2: Recurse** | a だけで再帰的にソート | `[5, 9]` |
| **Step 3: Main chain** | `S = [b1, a1, a2, ...]`。b1 < a1 なので比較なしで先頭 | `[3, 5, 9]` |
| **Step 4: Jacobsthal 挿入** | グループ (1,3], (3,5], (5,11], ... を番号の大きい方から。b_j は a_j より左だけ探索 | b3=4 → `[3,4,5,9]`, b2=7 → `[3,4,5,7,9]` |

探索範囲がいつも 3 個・7 個・15 個 … (2ᵏ - 1) に収まっていることが、Jacobsthal 順の効果。

## より大きい例で試す

```bash
./trace_sort 8 5 3 1 4 2 7 6
```

n=8 だと 3 段の再帰と Jacobsthal 挿入が観察できます。最後の `Total comparisons` を Ford-Johnson の最悪値 F(n) (n=8 なら 16) と比べてみてください。

```bash
./trace_sort 15 3 8 21 5 11 1 7 42 6 19 2 13 4 9
```

n=15 で Jacobsthal 数列 `1, 3, 5, 11, ...` の効果を確認できます (挿入順序が `b3, b2, b5, b4, b11, b10, ..., b6` の Jacobsthal パターンで並ぶ)。

## ex02 との差異

| | ex02 (提出用) | ex02_trace (教育用) |
|---|---|---|
| ロジック | 同じ (添字で持ち回る) | 同じ |
| container | `vector` + `deque` (2種) | `vector` のみ (可視化目的) |
| トレース出力 | なし | 各ステップ・各挿入の探索範囲と比較回数 |
| 実行時間計測 | あり (`us` 単位) | なし |
| 性能 | 実測用 | 出力コストが主 |

ex02 とは別のプログラムなので、提出物には影響しません。「ロジックを理解する」用途で好きに使ってください。
