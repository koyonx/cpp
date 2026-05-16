# cpp04 実装レビュー（修正反映後）

## 修正履歴

| 日付 | 演習 | 修正内容 |
|------|------|----------|
| 初版 | ex00 | `WrongAnimal` / `WrongCat` を新規実装。main に static binding 検証テストを追加 |
| 初版 | ex01 | `WrongAnimal` / `WrongCat` を ex00 から引き継ぎ。`Brain::setIdea` / `getIdea` を追加。`Cat::getBrain` / `Dog::getBrain` を追加。main に「Brainアドレス比較」「ideas[0]の独立変更」「subject 提示の `Dog basic; { Dog tmp = basic; }` パターン」「self-assignment」を追加 |
| 初版 | ex02 | ex01 から Brain / Cat / Dog / WrongAnimal / WrongCat を引き継ぎ。main に Brain アドレス比較・代入演算子の deep copy・WrongAnimal/Cat・self-assignment を追加。コメントで `Animal a;` が compile error になることを明示 |
| 初版 | ex03 | `AMateria::use` を non-pure に戻し空実装に。`AMateria::operator=` で `type` をコピーしないように修正。新規 `Fire` クラスを追加して「インベントリが任意の AMateria を扱える」ことを実証。main に 11 個のテストブロックを追加 |

---

## 修正後の評価項目チェック

### Preliminary tests (共通)

| 項目 | 状態 |
|------|------|
| `c++ -Wall -Wextra -Werror -std=c++98 -pedantic` でビルド成功 | ✅ 4演習全て警告ゼロ |
| C++98 標準準拠 | ✅ |
| ヘッダに実装を書いていない (テンプレ以外) | ✅ |
| `*alloc / *printf / free` 等の禁止関数 | ✅ |
| `using namespace` / `friend` | ✅ |
| 外部ライブラリ / STL コンテナ | ✅ `<iostream> <string>` のみ |
| Orthodox Canonical Form | ✅ 非インターフェイスクラスは全て4要素揃ってる |
| 二重インクルード防止 (include guards) | ✅ |

### ex00: Polymorphism

| チェック項目 | 状態 |
|-------------|------|
| Animal クラス + protected `std::string type` | ✅ |
| Cat / Dog が Animal を継承 | ✅ |
| Cat の type = "Cat", Dog の type = "Dog" | ✅ |
| `makeSound()` が virtual | ✅ |
| ポインタ越しに正しい makeSound が呼ばれる | ✅ |
| Constructor / Destructor のクラス固有メッセージ | ✅ |
| **WrongAnimal / WrongCat 実装** | ✅ **新規追加** |
| **WrongCat のサウンドは WrongCat として使ったときのみ出る** | ✅ **新規追加** |

### ex01: I do not want to set the world on fire

| チェック項目 | 状態 |
|-------------|------|
| Brain クラス、100個の std::string ideas | ✅ |
| Cat / Dog が private `Brain*` を保有 | ✅ |
| 構築時 `new Brain()`、破棄時 `delete brain` | ✅ |
| Animal 配列 (前半 Dog / 後半 Cat) を `Animal*` で delete | ✅ |
| デストラクタの順序が正しい | ✅ |
| Brain がリークしない (Animal::~Animal が virtual) | ✅ |
| コピーは deep copy | ✅ |
| **`Dog basic; { Dog tmp = basic; }` パターンを検証** | ✅ **新規追加** |
| **Brain アドレス比較で deep copy を視覚化** | ✅ **新規追加** |
| **ideas[i] の独立変更テスト** | ✅ **新規追加** |
| **self-assignment 安全性** | ✅ **新規追加** |

### ex02: Abstract class

| チェック項目 | 状態 |
|-------------|------|
| `Animal::makeSound() const = 0` | ✅ |
| `Animal a;` が compile error | ✅ 実機確認済み |
| ex01 と同じ動作 (Cat / Dog) | ✅ |
| Animal destructor が virtual | ✅ |
| Deep copy 維持 | ✅ |
| **WrongAnimal / WrongCat 引き継ぎ** | ✅ **新規** |

### ex03: Interface & recap

| チェック項目 | 状態 |
|-------------|------|
| ICharacter / IMateriaSource インターフェイス | ✅ subject 通り |
| MateriaSource 実装 | ✅ |
| Ice / Cure: clone(), type 小文字 ("ice", "cure") | ✅ |
| AMateria 抽象クラス、virtual ~AMateria(), protected type | ✅ |
| **`virtual void use(ICharacter&)` が non-pure** | ✅ **修正済** |
| **operator= で type をコピーしない** | ✅ **修正済** |
| Character: 4枠インベントリ | ✅ |
| 空き slot を 0→3 で探して equip | ✅ |
| フル時 / 範囲外 idx で何もしない | ✅ |
| unequip は delete しない (caller 責務) | ✅ |
| Character の deep copy (copy ctor + operator=) | ✅ |
| 代入時に既存 inventory を先に delete | ✅ |
| ~Character で残った materia を delete | ✅ |
| **subject 公式 main の出力一致** | ✅ `cat -e` で一致確認 |
| **追加テスト多数** | ✅ **11ブロック新規** |

#### ex03 追加テストの内訳

1. subject 公式 main の例（先頭で必ず実行）
2. インベントリ満杯 + 5回目の equip 無視 + extra を caller が delete
3. 未知 type で createMateria が NULL を返す + equip(NULL) 安全
4. 範囲外/空 slot での use / unequip 安全
5. Character copy constructor の deep copy + 元の unequip 後も copy 側 OK
6. Character operator= の deep copy
7. unequip 前に address を保存して自分で delete (leak 回避パターン)
8. MateriaSource 上限 4 + 5個目は無視、duplicate 学習OK
9. **`Fire` という独自 AMateria 派生クラスでインベントリが任意型対応であることを実証**
10. Character self-assignment 安全性
11. AMateria::operator= が type をコピーしないことを実証 (Ice = Cure しても Ice のまま)

---

## ビルドコマンド

```bash
# WSL Ubuntu 24.04 環境
cd ~/42-cursus/cpp/cpp04/ex00 && make && ./animal
cd ~/42-cursus/cpp/cpp04/ex01 && make && ./animal
cd ~/42-cursus/cpp/cpp04/ex02 && make && ./animal
cd ~/42-cursus/cpp/cpp04/ex03 && make && ./materia
```

すべて `-Wall -Wextra -Werror -std=c++98 -pedantic` でビルド成功、警告ゼロを確認済み。

---

## ファイル一覧

```
ex00/
  Animal.{hpp,cpp}
  Cat.{hpp,cpp}
  Dog.{hpp,cpp}
  WrongAnimal.{hpp,cpp}   ← 新規
  WrongCat.{hpp,cpp}      ← 新規
  main.cpp                ← 改良
  Makefile                ← Wrong* を追加

ex01/
  Animal.{hpp,cpp}
  Brain.{hpp,cpp}         ← setIdea/getIdea 追加
  Cat.{hpp,cpp}           ← getBrain() 追加
  Dog.{hpp,cpp}           ← getBrain() 追加
  WrongAnimal.{hpp,cpp}   ← ex00 から引き継ぎ
  WrongCat.{hpp,cpp}      ← ex00 から引き継ぎ
  main.cpp                ← 改良
  Makefile                ← Wrong* を追加

ex02/
  Animal.{hpp,cpp}        ← pure virtual makeSound
  Brain.{hpp,cpp}         ← ex01 から引き継ぎ
  Cat.{hpp,cpp}           ← ex01 から引き継ぎ
  Dog.{hpp,cpp}           ← ex01 から引き継ぎ
  WrongAnimal.{hpp,cpp}   ← ex01 から引き継ぎ
  WrongCat.{hpp,cpp}      ← ex01 から引き継ぎ
  main.cpp                ← 改良
  Makefile                ← Wrong* を追加

ex03/
  AMateria.{hpp,cpp}      ← use を non-pure に、operator= で type コピーしない
  Ice.{hpp,cpp}
  Cure.{hpp,cpp}
  Fire.{hpp,cpp}          ← 新規 (汎用性デモ用)
  ICharacter.hpp
  IMateriaSource.hpp
  Character.{hpp,cpp}
  MateriaSource.{hpp,cpp}
  main.cpp                ← 11ブロックの拡充
  Makefile                ← Fire を追加
```
