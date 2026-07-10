# C++ Module 06 — C++ casts

## このモジュールで学ぶこと

C++ の 4 種類のキャストのうち、C 言語スタイルのキャスト `(T)x` に代わる **型安全な 3 種類**を実際に使い分ける:

| キャスト | 用途 | ex |
|---|---|---|
| `static_cast<T>(x)` | 算術型どうし、基底↔派生（コンパイル時保証あり） | ex00 |
| `reinterpret_cast<T>(x)` | ポインタ↔整数、無関係なポインタ間（ビットパターン再解釈） | ex01 |
| `dynamic_cast<T>(x)` | 実行時型判定を伴う polymorphic ダウンキャスト | ex02 |
| `const_cast<T>(x)` | `const` / `volatile` の付け外し（本モジュールでは扱わない） | — |

**Additional Rule (PDF明示):** 各 ex では「その用途に最も適したキャストを 1 つ選び defense で説明できるようにしろ」と要求されている。この repo では:
- ex00 → `static_cast`
- ex01 → `reinterpret_cast`
- ex02 → `dynamic_cast`

---

## そもそも用語おさらい

### 3 種類のキャストの使い分け

**`static_cast<T>(x)` — 「型システムが理解できる変換」**
- `int` ↔ `float` ↔ `double` ↔ `char` などの算術変換
- 基底クラス ↔ 派生クラス（コンパイル時に継承関係が既知）
- `void*` → 具体型ポインタ
- コンパイル時に「変換可能か」がチェックされる。無理な変換はコンパイルエラーになる

**`reinterpret_cast<T>(x)` — 「ビットパターンをそのまま別型として扱う」**
- ポインタ ↔ 整数（`uintptr_t`）
- 無関係な型のポインタ間 (`int*` ↔ `char*`)
- コンパイル時チェックがほぼない = 誤用すると未定義動作
- 「ここは危ないから慎重にやっている」というマーカーとして機能

**`dynamic_cast<T>(x)` — 「実行時の型判定」**
- ポリモーフィックな（=仮想関数を持つ）型でのみ使える
- ダウンキャスト時に「実際にその型か？」を実行時チェック
- ポインタで失敗 → `NULL` を返す
- 参照で失敗 → `std::bad_cast` を投げる
- RTTI (Run-Time Type Information) を使うので、若干のオーバーヘッドあり

### `uintptr_t` とは
- `<stdint.h>` (C99) で定義される「ポインタを格納できるサイズの符号なし整数」
- 環境依存: 64-bit システムなら 64-bit、32-bit なら 32-bit
- `void*` を `uintptr_t` にキャストすると、元の値に戻せることが保証される（規格）

### RTTI (Run-Time Type Information)
- 仮想関数を持つ型に自動的に付加される「型情報テーブル」
- `dynamic_cast` と `typeid` がこれを使う
- 継承階層のどこか 1 つでも仮想関数（or 仮想 dtor）があれば有効化される
- 本モジュール ex02 の `Base` は仮想デストラクタを持つので RTTI が働く

---

## 各 ex 詳細

### ex00 — ScalarConverter (static_cast)

**目的:** 文字列で与えられた C++ リテラルを検出し、`char` / `int` / `float` / `double` の 4 型に変換して表示。

**実装のキーポイント:**
- 静的メソッドのみのクラス。**インスタンス化不可**にするため private コンストラクタ（宣言のみ、定義なし）
- 型検出フロー:
  1. 疑似リテラル (`nan`, `+inf`, `-inf`, `nanf`, `+inff`, `-inff`) → `std::numeric_limits<double>::infinity()` などを直接投入
  2. 1文字 かつ 数字でない → char リテラル（`'a'` の a）
  3. `strtod` でパース。残り文字が `""` → int/double、`"f"` → float、それ以外 → 無効
- 表示:
  - char: `[0, 127]` かつ `std::isprint` → `'*'`、範囲内だが非表示 → `Non displayable`、範囲外/NaN/inf → `impossible`
  - int: `INT_MIN..INT_MAX` の範囲チェック、NaN/inf → `impossible`
  - float/double: NaN → `nanf`/`nan`、±inf → `±inff`/`±inf`、それ以外 → `std::fixed << std::setprecision(1)`
- **`static_cast` を使う場所:** `static_cast<char>(d)`, `static_cast<int>(d)`, `static_cast<float>(d)` — 算術型変換全般

**落とし穴:**
- `d != d` は NaN 判定のイディオム（NaN は自分自身と等しくない、という IEEE 754 規約）
- `<cmath>` の `std::isnan` は C++11 追加。C++98 では移植性を担保するため `d != d` を使う
- `strtod` は `errno = ERANGE` でオーバーフロー判定。C++ の例外じゃないので errno を先にリセット必須
- float オーバーフロー: `static_cast<float>(1e40)` は無音で `+inf` になる。キャスト後に `f == std::numeric_limits<float>::infinity()` を確認して `+inff` を出す
- `std::cout` の書式状態（fixed / setprecision）は **永続的**。convert 後にリセットしないと後続の出力が壊れる → `unsetf(fixed)` + `setprecision(6)` で戻す

### ex01 — Serializer (reinterpret_cast)

**目的:** ポインタと整数（`uintptr_t`）の相互変換を実装し、`ptr → uintptr_t → ptr` のラウンドトリップで元と同一の値になることを確認する。

**実装のキーポイント:**
```cpp
uintptr_t Serializer::serialize(Data* ptr) {
    return reinterpret_cast<uintptr_t>(ptr);
}
Data* Serializer::deserialize(uintptr_t raw) {
    return reinterpret_cast<Data*>(raw);
}
```
- `Data` 構造体は「非空」を PDF 要件で満たすため `int id` / `std::string name` / `double value` の 3 メンバ
- `Serializer` は静的メソッドのみ、private ctor でインスタンス化不可

**なぜ `reinterpret_cast` ？**
- ポインタと整数は「型としてはまったく別物」だが、実体はビットパターンとして同じサイズ
- `static_cast` はこの変換を許さない（コンパイルエラー）
- `reinterpret_cast` は「型システムを無視してビットをそのまま別の型として解釈する」ためのキャスト
- 規格上、`void*` → `uintptr_t` → `void*` のラウンドトリップは元と等価が保証されている

**落とし穴:**
- `<stdint.h>` は C99 ヘッダだが C++98 環境でも使える。`<cstdint>` は C++11 なので **使わない**
- `reinterpret_cast<Data*>(0)` は `NULL` と等価（規格保証）
- ラウンドトリップした後にポインタ経由でメンバに書き込むと、元のオブジェクトが変更される — 同じアドレスなので当たり前だが、テストで明示的に確認

### ex02 — Base/A/B/C + identify (dynamic_cast)

**目的:** ポリモーフィックな階層で、`Base*` や `Base&` から実際の派生型を実行時に特定する。

**実装のキーポイント:**
- `Base` は「public virtual dtor だけ」を持つ = RTTI を有効化する最小構成
- A, B, C は空のクラス、`public` 継承
- `generate()` は `rand() % 3` で 3 択、`new A/B/C` 返却
- `identify(Base* p)` はポインタ版 `dynamic_cast<T*>` を順に試す（失敗すると `NULL`）
- `identify(Base& p)` は参照版 `dynamic_cast<T&>` を順に試す。失敗すると `std::bad_cast` を投げるので try/catch で拾う

**「pointer inside function is forbidden」への対応:**
- reference 版で `dynamic_cast<A*>(&p)` は禁止（`&p` はポインタ）
- 代わりに `dynamic_cast<A&>(p)` を使い、`try/catch` で `std::bad_cast` を捕捉

**「typeinfo header is forbidden」への対応:**
- `std::bad_cast` の宣言は `<typeinfo>` にあるので明示的に catch できない
- 代わりに `catch (...)` で全捕捉（実際に投げられるのは `std::bad_cast` のみと分かっているため安全）

**落とし穴:**
- 派生型を静的に判っている `A a; dynamic_cast<C*>(&a);` は **コンパイル時に警告**（`-Werror` でエラー化）される。テストで書きたい場合は `Base* b = &a;` と一旦 upcast すべし
- `Base` 自身のインスタンスに identify を呼ぶと、A/B/C いずれの dynamic_cast も失敗 → `Unknown` を出力する
- NULL ポインタに対しての `dynamic_cast<T*>(NULL)` は `NULL` を返す（規格）。安全に扱える

---

## コンパイル・実行

各 ex ディレクトリで:
```bash
make          # ビルド
make clean    # .o だけ消す
make fclean   # 実行ファイルも消す
make re       # fclean → all
```

コンパイルフラグ:
```
-Wall -Wextra -Werror -std=c++98 -pedantic
```

### 実行方法

```bash
# ex00 — 単発の literal を渡す or 引数なしで内蔵テスト実行
./convert 42.0f
./convert nan
./convert           # -> 79 assertion のテストスイート実行

# ex01 — 引数なし、Data の roundtrip テストを実行
./serializer

# ex02 — 引数なし、A/B/C の identify テストを実行
./identify
```

## テストカバレッジ

| ex | assertions | valgrind |
|---|---|---|
| ex00 | 79 | 0 leaks / 0 errors |
| ex01 | 23 | 0 leaks / 0 errors |
| ex02 | 29 | 0 leaks / 0 errors |
| **合計** | **131** | **all clean** |

主な検証項目:
- **ex00**: PDF 例（0, nan, 42.0f）exact match、char/int/float/double 全域、pseudo-literals、境界値、オーバーフロー、無効入力、10,000 回 leak 耐性、stream state 復元
- **ex01**: PDF roundtrip 要件、NULL、heap/stack、複数オブジェクト独立性、mutation-visibility、10,000 回 leak 耐性
- **ex02**: 3 concrete それぞれ pointer/reference で identify、両版一致、NULL / plain Base → `Unknown`、100/1000 回 generate 統計分布、10,000 回 gen/delete で virtual dtor 動作

---

## レビュー時に聞かれそうなこと

- **なぜ ex00 で `static_cast` ？** → 算術型どうしの変換はコンパイル時に「変換可能」と分かる。ランタイム RTTI は不要。C 言語スタイルの `(int)d` より意図が明確で、変換不可能な組み合わせをコンパイルエラーで弾いてくれるから安全
- **なぜ ex01 で `reinterpret_cast` ？** → ポインタ（アドレスを保持する型）と `uintptr_t`（整数）はビットパターンとしては同サイズだが型システム上は無関係。`static_cast` では変換できない。「型を無視してビットを再解釈する」ためのキャストが `reinterpret_cast`
- **なぜ ex02 で `dynamic_cast` ？** → `Base*` が実行時に指しているのが A なのか B なのか、コンパイル時には分からない。RTTI を使って実行時判定するのが `dynamic_cast` の唯一の役割
- **`Base` に virtual デストラクタがない場合はどうなる？** → RTTI が働かないので `dynamic_cast` はコンパイルエラーになる（"cannot dynamic_cast … source is not polymorphic"）
- **`dynamic_cast<T*>` と `dynamic_cast<T&>` の違いは？** → ポインタ版は失敗すると `NULL` を返す（例外なし）。参照版は失敗すると `std::bad_cast` を投げる（NULL 参照は存在しないので、そもそも NULL を返せない）
- **`static_cast` と C 言語スタイル `(T)x` の違いは？** → C 言語キャストは 4 種類のキャストを状況に応じて自動選択する = 意図が読めない・危険な変換もサイレントに通る。C++ の名前付きキャストは目的別なので、コードの意図が明示され、誤用がコンパイル時に見つかりやすい

## 参考リンク

- [cppreference: static_cast](https://en.cppreference.com/w/cpp/language/static_cast)
- [cppreference: reinterpret_cast](https://en.cppreference.com/w/cpp/language/reinterpret_cast)
- [cppreference: dynamic_cast](https://en.cppreference.com/w/cpp/language/dynamic_cast)
- [uintptr_t](https://en.cppreference.com/w/cpp/types/integer)
- [RTTI overview](https://en.wikipedia.org/wiki/Run-time_type_information)
