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
Initial input: [3, 5, 9, 7, 4]

[depth 0] Input: [3, 5, 9, 7, 4]
[depth 0] Odd size -> straggler = 4, remaining = [3, 5, 9, 7]
[depth 0] Step 1 - Pairs (larger, smaller): [(5,3), (9,7)]
[depth 0] Step 2 - Recursing on winners: [5, 9]
  [depth 1] Input: [5, 9]
  [depth 1] Even size, no straggler
  [depth 1] Step 1 - Pairs (larger, smaller): [(9,5)]
  [depth 1] Step 2 - Recursing on winners: [9]
    [depth 2] Input: [9]
    [depth 2] size <= 1, return unchanged
  [depth 1] Step 2 done - winners sorted: [9]
  [depth 1] Step 3 - Reordered pairs: [(9,5)]
  [depth 1] Step 4a - Main chain from winners: [9]
  [depth 1] Step 4b - Prepend b1=5 (smaller of first pair, trivially min): [5, 9]
  [depth 1] Step 5a - Jacobsthal seq: 1, 3
  [depth 1] Step 5b - Insertion order (indices into pairs): (none)
  [depth 1] Step 6 - No straggler
  [depth 1] Final at this depth: [5, 9]
[depth 0] Step 2 done - winners sorted: [5, 9]
[depth 0] Step 3 - Reordered pairs: [(5,3), (9,7)]
[depth 0] Step 4a - Main chain from winners: [5, 9]
[depth 0] Step 4b - Prepend b1=3 (smaller of first pair, trivially min): [3, 5, 9]
[depth 0] Step 5a - Jacobsthal seq: 1, 3, 5
[depth 0] Step 5b - Insertion order (indices into pairs): b2=7
[depth 0] Step 5c - Inserting b2=7 via binary search:
    -> upper_bound(7) points to index 2
    After insert: [3, 5, 7, 9]
[depth 0] Step 6 - Inserting straggler=4 via binary search:
    -> upper_bound(4) points to index 1
    After insert: [3, 4, 5, 7, 9]
[depth 0] Final at this depth: [3, 4, 5, 7, 9]

Sorted output: [3, 4, 5, 7, 9]
```

## 各ステップの読み方

| ステップ | 説明 | この例での動き |
|---|---|---|
| **Straggler 退避** | 奇数個なら末尾 1 個を後回し | `[3,5,9,7,4]` → 4 退避、残り `[3,5,9,7]` |
| **Step 1: Pair up** | 2 個ずつ組み、各組内で (larger, smaller) に整理 | `(3,5) → (5,3)`, `(9,7) → (9,7)` |
| **Step 2: Recurse** | winners のみで再帰的にソート | `[5, 9]` を再帰 → base case で `[5, 9]` |
| **Step 3: Reorder** | winners の sorted 順に pairs を並べ替え | 変更なし |
| **Step 4a: Main chain** | 並べた winners を主鎖 S に | `S = [5, 9]` |
| **Step 4b: Prepend b1** | 先頭 pair の loser は自明に最小 | `S = [3, 5, 9]` (3 を先頭に) |
| **Step 5a: Jacobsthal seq** | 挿入順序を決めるための Jacobsthal 数 | `1, 3, 5, ...` |
| **Step 5b: Insertion order** | Jacobsthal 順で残り losers を並べる | `b2=7` のみ (この例は要素数少) |
| **Step 5c: Binary insert** | 各 loser を upper_bound で位置決めて insert | `7` → index 2 に挿入 → `[3, 5, 7, 9]` |
| **Step 6: Straggler** | 退避しておいた要素を binary insert | `4` → index 1 → `[3, 4, 5, 7, 9]` |

## より大きい例で試す

```bash
./trace_sort 8 5 3 1 4 2 7 6
```

n=8 だと 3 段の再帰と Jacobsthal 挿入が観察できます。

```bash
./trace_sort 15 3 8 21 5 11 1 7 42 6 19 2 13 4 9
```

n=15 で Jacobsthal 数列 `1, 3, 5, 11, ...` の効果を確認できます (挿入順序が `b3, b2, b5, b4, b11, b10, ..., b6` の Jacobsthal パターンで並ぶ)。

## ex02 との差異

| | ex02 (提出用) | ex02_trace (教育用) |
|---|---|---|
| ロジック | 完全に同じ | 完全に同じ |
| container | `vector` + `deque` (2種) | `vector` のみ (可視化目的) |
| トレース出力 | なし | 各ステップ完全出力 |
| 実行時間計測 | あり (`us` 単位) | なし |
| 性能 | 実測用 | 出力コストが主 |

**ex02 自体を弄っていない** ので、提出物には影響しません。「ロジックを理解する」用途で好きに使ってください。
