# cpp04 攻略のための知識集

このモジュールは「Subtype Polymorphism / Abstract Classes / Interfaces」がテーマ。
順を追って必要な概念を説明する。コードの暗記ではなく **なぜそうなるか** を理解することが、面接時の口頭試問（Ask the student about constructor and destructor orders など）で重要になる。

---

## 1. 継承（Inheritance）の基礎

### 1.1 構文

```cpp
class Base {
protected:
    std::string type;        // private だと派生先から触れない。protected がポイント
public:
    Base();
    virtual ~Base();
};

class Derived : public Base { // public 継承 = is-a 関係
public:
    Derived();
    virtual ~Derived();
};
```

### 1.2 アクセス指定（private / protected / public）

| 指定 | 同一クラスから | 派生クラスから | 外部から |
|------|-----------------|-----------------|----------|
| `private` | ✅ | ❌ | ❌ |
| `protected` | ✅ | ✅ | ❌ |
| `public` | ✅ | ✅ | ✅ |

cpp04 で `Animal::type` を **protected** にするのは、`Cat::Cat()` の中で `type = "Cat"` のように設定できる必要があるから。

### 1.3 派生クラスのコンストラクタは「親の構築 → 自分の構築」

```cpp
Cat::Cat() : Animal("Cat") {  // 初期化子リストで親コンストラクタ呼出
    std::cout << "Cat ctor\n";
}
```

明示しないと **デフォルトコンストラクタ** `Animal()` が暗黙に呼ばれる。引数ありコンストラクタしか持たない親では明示が必須。

### 1.4 コンストラクタ・デストラクタの呼び出し順

```
new Cat() の場合:
  → Animal() コンストラクタ
  → Cat()    コンストラクタ
delete cat の場合:
  → Cat()    デストラクタ
  → Animal() デストラクタ
```

**親→子の構築、子→親の破壊**。これはオブジェクトを内側からほどいていく順番なので暗記不要。
面接で「なぜこの順？」と聞かれたら：「親の上に子のデータが乗っかってるから、組み立ては土台から、解体は屋根から」と説明できる。

---

## 2. 仮想関数（virtual）と多態（Polymorphism）

### 2.1 動的バインディングとは

```cpp
Animal* p = new Cat();
p->makeSound();
```

`p` の **静的型** は `Animal*`、**動的型** は `Cat`。
通常のメンバ関数は静的型で解決される（早期バインディング）。
`virtual` を付けると **動的型** で解決される（遅延バインディング）。

### 2.2 仕組み：vtable / vptr

仮想関数を持つクラスはコンパイル時に **vtable**（仮想関数テーブル）が作られる：
```
Animal vtable:  [&Animal::makeSound]
Cat    vtable:  [&Cat::makeSound]
```
各インスタンスには `vptr`（vtable へのポインタ）が忍ばせてある。`p->makeSound()` は `p->vptr->[0]()` のように呼ばれるので、`Cat` のインスタンスなら必ず `Cat::makeSound` が呼ばれる。

### 2.3 virtual を付け忘れたとき（WrongAnimal の意義）

```cpp
class WrongAnimal {
public:
    void makeSound() const;  // ← virtual なし
};
class WrongCat : public WrongAnimal {
public:
    void makeSound() const;  // 関数名は同じだが override ではなく "hiding"
};

WrongAnimal* p = new WrongCat();
p->makeSound();  // → WrongAnimal::makeSound() が呼ばれる！
```

これが ex00 で WrongAnimal/WrongCat を要求している理由。**virtual を抜かすと静的型で解決される** ことを体感させる仕掛け。

### 2.4 デストラクタを virtual にする理由（最重要）

```cpp
Animal* p = new Cat();
delete p;
```

`Animal::~Animal()` が virtual でないと **`Cat::~Cat()` が呼ばれない**。
ex01 で Cat が `Brain*` を持ち、`Cat` のデストラクタで delete しているケースだと、
これは **メモリリーク** になる。

ルール：**多態的に delete される可能性があるクラス（基底クラス）は、デストラクタを必ず virtual にする**。

派生側のデストラクタには明示的に virtual を付けなくても、基底が virtual なら派生も自動で virtual になるが、明示しておくのが慣習。

### 2.5 override（C++98 にはキーワードはない）

C++11 以降は `override` キーワードで「これは override する意図」と明示できるが、cpp04 は C++98 なので **使えない**。
シグネチャ（戻り値、引数、const 修飾）が完全に一致していれば override になる。1文字でも違えば「別の関数を定義」になって multipule定義警告も出ない静かなバグになる。

---

## 3. 抽象クラスと純粋仮想関数（Pure virtual）

### 3.1 構文と意味

```cpp
class Animal {
public:
    virtual void makeSound() const = 0;   // 純粋仮想関数
};
```

`= 0` を付けると **pure virtual** になり、そのクラスは **抽象クラス（abstract class）** になる。
抽象クラスは **インスタンス化できない**：

```cpp
Animal a;          // コンパイルエラー
Animal* p = new Animal();  // コンパイルエラー
Animal* p = new Cat();     // OK（Cat は具体クラス）
```

### 3.2 抽象クラスでもポインタ／参照は持てる

```cpp
Animal* p = new Cat();        // OK
Animal& ref = *p;             // OK
void f(Animal* a) {           // OK
    a->makeSound();
}
```

これが多態の基盤。**ポインタ越し** にしか抽象クラスは扱えない。

### 3.3 pure virtual な関数にも本体は書ける

```cpp
// .hpp
virtual void makeSound() const = 0;
// .cpp
void Animal::makeSound() const {
    std::cout << "generic sound\n";
}
```

派生クラスが `Animal::makeSound()` を明示的に呼ぶこともできる。cpp04 では使わないけど知識として。

### 3.4 派生クラスが pure virtual を override しないと？

そのクラスもまた抽象クラスになる。すべての pure virtual を override して初めて具体クラス（instantiable）になる。

---

## 4. インターフェイス（Interface）

C++ には Java/C# の `interface` キーワードはない。代わりに **「pure virtual のみを持つ抽象クラス」** をインターフェイスと呼ぶ慣習がある。

```cpp
class ICharacter {
public:
    virtual ~ICharacter() {}                                     // 仮想デストラクタは必須
    virtual std::string const & getName() const = 0;
    virtual void equip(AMateria* m) = 0;
    virtual void unequip(int idx) = 0;
    virtual void use(int idx, ICharacter& target) = 0;
};
```

慣習：
- クラス名の頭に **`I`** を付ける（`ICharacter`、`IMateriaSource`）
- 部分実装を持つ抽象クラスは **`A`** を付ける（`AMateria`）

### 4.1 なぜ仮想デストラクタが必要か

`ICharacter* p = new Character("me"); delete p;` を許すため。前述のとおり、基底のデストラクタが virtual でないと派生のデストラクタが呼ばれない。

### 4.2 インターフェイス側でデストラクタ本体を空 `{}` にする理由

インターフェイス自体は何もメンバを持たないので、デストラクタですべきことが無い。
**ただし virtual にする必要があるので宣言は要る** → 空実装 `{}` を「ヘッダ内」に書くのが定番。

> 「ヘッダに実装書くなって言われてるじゃん？」と思うが、インターフェイス（ピュアな抽象クラス）はこのモジュールでは例外的に許容される。Subject の `ICharacter` 例自体がそうなっている。

---

## 5. Orthodox Canonical Form（OCF）

C++98 では、クラスは以下の **4つ** を必ず備えるのが推奨される（"big four"、C++11以降は big five / big six になる）：

1. **デフォルトコンストラクタ** `ClassName()`
2. **コピーコンストラクタ** `ClassName(const ClassName& other)`
3. **コピー代入演算子** `ClassName& operator=(const ClassName& other)`
4. **デストラクタ** `~ClassName()` （多態の基底なら virtual）

### 5.1 デフォルトコンストラクタ

「引数なしで構築できる」コンストラクタ。配列やコンテナへの格納で必要になる。Animal で `type` を「Animal」や空文字列で初期化する。

### 5.2 コピーコンストラクタ

```cpp
Cat(const Cat& other) : Animal(other) {
    brain = new Brain(*other.brain);     // ディープコピー
}
```

- 初期化子リストで「親のコピーコンストラクタ」を呼ぶ
- ポインタメンバは **必ず new で複製**（シャローコピー禁止）

### 5.3 コピー代入演算子

```cpp
Cat& Cat::operator=(const Cat& other) {
    if (this != &other) {                // 自己代入チェック
        Animal::operator=(other);        // 親の代入演算子を明示呼出
        *brain = *other.brain;           // または delete brain; brain = new Brain(*other.brain);
    }
    return *this;                        // 連鎖代入 a = b = c のため
}
```

- **自己代入チェック** （`if (this != &other)`） は習慣的に書く。書かないと `a = a` で `delete brain` した直後に `new Brain(*other.brain)` で参照する破壊済みメモリを参照してしまう実装になりかねない
- **親の `operator=` を呼ぶ** ことで親の状態もコピー
- **`*this` を返す** ことで `(a = b) = c` が書ける

### 5.4 デストラクタ

```cpp
virtual ~Cat() {
    delete brain;
}
```

- 多態の基底クラスなら **必ず virtual**
- 自分が `new` で確保した全メモリを `delete`

### 5.5 OCF の例外：インターフェイス

`ICharacter` のような pure interface に Canonical Form は不要。
- メンバを持たないので、デフォルトコンストラクタは自動生成で OK
- コピー演算は基底クラスとしては禁止することも多い（仮想継承の罠を避けるため）
- デストラクタだけは virtual + 空実装

---

## 6. Deep Copy vs Shallow Copy

### 6.1 問題のシナリオ（ex01）

```cpp
class Cat {
    Brain* brain;
public:
    Cat()  { brain = new Brain(); }
    ~Cat() { delete brain; }
    // コピー演算を書かないと…
};

Dog basic;                // brain = 0x1000
{
    Dog tmp = basic;      // tmp.brain = 0x1000（shallow copy）
}                         // tmp の ~Dog で 0x1000 を delete
// basic.brain は 0x1000（解放済み）を指したまま！
// basic のスコープ終了時に 0x1000 を再 delete → undefined behavior
```

**シャローコピーの問題**：
1. 同じヒープ領域を複数のオブジェクトが指す
2. 片方が delete すると、もう片方は dangling pointer を保持
3. もう片方も delete する → double-free（UB、最悪 SIGSEGV）

### 6.2 ディープコピーの解決策

```cpp
Cat(const Cat& other) : Animal(other) {
    brain = new Brain(*other.brain);    // 新規確保してから内容コピー
}

Cat& operator=(const Cat& other) {
    if (this != &other) {
        Animal::operator=(other);
        *brain = *other.brain;           // または delete してから new
    }
    return *this;
}
```

**ポイント**：
- ポインタを保有するクラスでは **必ず OCF を全部書く**（コンパイラが自動生成するものは全て shallow）
- 「3の法則（Rule of Three）」：デストラクタ／コピーコンストラクタ／コピー代入演算子のどれか1つでも自前で書く必要があるなら、3つすべて書く

---

## 7. 動的メモリ：new / delete とリーク防止

### 7.1 C 風 vs C++ 風

```cpp
// C 風（cpp04 では禁止）
int* p = (int*)malloc(sizeof(int));
free(p);

// C++ 風
int*  p   = new int;
int*  arr = new int[10];
delete   p;
delete[] arr;
```

`new` / `delete` を間違えるとリーク。`new[]` で確保したら **必ず `delete[]`** を使う（`delete` だと配列の他要素のデストラクタが呼ばれない）。

### 7.2 ex03 の罠

```cpp
src->learnMateria(new Ice());       // 引数で new、ポインタ所有権を渡す
me->equip(src->createMateria("ice"));  // createMateria が clone() を返す → me が所有
me->unequip(0);                      // ← この時点でスロット0が NULL になる
                                     //   元 Materia は delete されない仕様
                                     //   ⇒ unequip 前に address を保存していないとリーク
```

Subject の指示：
> Save the addresses before calling unequip(), or anything else, but don't forget that you have to avoid memory leaks.

正しい使い方：
```cpp
AMateria* tmp = /* スロット0の参照 */;
me->unequip(0);
delete tmp;                          // 床に落とした materia を自分で回収
```

### 7.3 リーク検出ツール

- **valgrind** （Linux）: `valgrind --leak-check=full ./animal`
- **Address Sanitizer** （clang/gcc）: コンパイル時に `-fsanitize=address`

42 の評価では valgrind を使って確認される。**配列要素を delete し忘れた / new[] を delete で解放した** などの細かいミスも引っかかる。

---

## 8. const 修飾の細かい話

`makeSound() const` の `const` は何を意味するか：

```cpp
class Animal {
public:
    virtual void makeSound() const;  // この const は…
};
```

- 関数が **このオブジェクトのメンバを変更しないこと** を保証する宣言
- `const Animal* p` の `p->makeSound()` を呼ぶには、`makeSound()` が const 修飾されていないとコンパイルエラー

Subject の例：
```cpp
const Animal* j = new Dog();
j->makeSound();   // const なメンバ関数しか呼べない
```

cpp04 の `makeSound`、`getType`、`getName` は **すべて const 修飾必須**。`equip` や `use` は内部状態を変えるので const ではない。

参照渡しの const：
```cpp
std::string const & getType() const;
//                ^ 返り値が const 参照（呼び出し側が中身を書き換えられない）
//                                  ^ メンバを変更しない
```

`std::string const &` は `const std::string &` と等価。場所だけ違う書き方（east-const vs west-const）。

---

## 9. 前方宣言（Forward Declaration）と循環依存

ex03 では `AMateria` と `ICharacter` が互いを参照する：
- `AMateria::use(ICharacter& target)` で `ICharacter` を使う
- `ICharacter::use(int idx, ICharacter& target)` の中身で `AMateria::use()` を呼ぶ

このとき両方の `.hpp` で互いを `#include` すると **無限ループ → コンパイルエラー**。

解決法：**前方宣言**

```cpp
// AMateria.hpp
class ICharacter;          // ← 前方宣言。中身を知らなくてもポインタ／参照は使える

class AMateria {
public:
    virtual void use(ICharacter& target);
};

// AMateria.cpp
#include "AMateria.hpp"
#include "ICharacter.hpp"  // ← 中身を使うのはここ
```

ルール：
- ヘッダ内で **ポインタ / 参照** だけなら前方宣言で OK
- **メンバ変数として保持** / **メンバ関数を呼ぶ** / **継承する** なら完全な型定義が必要 → `#include`
- 実装ファイル（.cpp）では遠慮なく `#include`

---

## 10. インクルードガード

```cpp
#ifndef ANIMAL_HPP
#define ANIMAL_HPP
// ... クラス定義
#endif
```

または C++ では `#pragma once` も多いが、42 cpp モジュールでは `#ifndef` 方式が推奨。**Subject では include guard 必須**（無いと grade 0）。

マクロ名は **ファイル名を大文字＋アンダースコア** にするのが慣習：`ANIMAL_HPP`、`A_MATERIA_HPP` など。複数のクラスで衝突しないように。

---

## 11. C++98 標準縛りの落とし穴

`-std=c++98` で禁止される、思わず使いがちな機能：

| 機能 | C++98 | 代替 |
|------|-------|------|
| `nullptr` | ❌ | `NULL` または `0` |
| `auto x = ...` | ❌（C++98 では別の意味） | 型を明記 |
| 範囲 for `for (auto& x : v)` | ❌ | 添字 for、iterator |
| `override` / `final` | ❌ | ただシグネチャを合わせる |
| `=default` / `=delete` | ❌ | OCF を全部手書き |
| `std::unique_ptr` / `std::shared_ptr` | ❌ | 生ポインタ |
| ラムダ式 `[](){}` | ❌ | 関数オブジェクト or 関数ポインタ |
| 初期化リスト `{1,2,3}` | ❌（restricted） | コンストラクタ呼び出し |
| `static_assert` | ❌ | テンプレート技 |
| `<chrono>`, `<thread>`, `<unordered_*>` | ❌ | C 関数 / 自前実装 |

`auto` は **C++98 では「自動記憶クラス指定子」** という別物（実質ノーオプ）。使うとコンパイラ警告が出る場合がある。

---

## 12. cpp04 デバッグの実践テクニック

### 12.1 コンストラクタ・デストラクタの出力で挙動を追う

これがそのまま Subject 要件「Constructors and destructors of each class must display specific messages」になっている。デバッグ目的だけでなく **採点項目**。

```
Animal type constructor called
Dog default constructor called
Brain default constructor called
```

これを見て：
- 親→子の順で構築されているか
- Brain が Dog/Cat ごとに1つずつ生成されているか
- 削除順は子→親で正しいか
- コピー時に Brain も新規生成されているか

を **目視で確認** できる。

### 12.2 配列で一括テスト

```cpp
Animal* zoo[10];
for (int i = 0; i < 5; i++) zoo[i] = new Dog();
for (int i = 5; i < 10; i++) zoo[i] = new Cat();
for (int i = 0; i < 10; i++) delete zoo[i];  // ← 全部 ~Dog/~Cat → ~Animal が呼ばれること
```

これが ex01 の典型的なテスト形。多態的 delete が正しく動くこと、Brain がリークしないことを確認する。

### 12.3 スコープを使った deep copy 検証

```cpp
Dog basic;
std::cout << "outer brain ptr: " << /* basic の brain アドレス */ << std::endl;
{
    Dog tmp = basic;
    std::cout << "inner brain ptr: " << /* tmp の brain アドレス */ << std::endl;
    // inner と outer のアドレスが異なれば deep copy 成功
}
// tmp が破棄されても basic は無傷で使えること
basic.makeSound();
```

ただし `Brain*` が private なので **アドレスを取り出す getter** を Brain か Cat に足すか、`Brain::setIdea / getIdea` で内容を書き換えて独立性を見せる必要がある。

### 12.4 valgrind

```
$ sudo apt install valgrind
$ valgrind --leak-check=full --show-leak-kinds=all ./animal
```

期待する出力：
```
==12345== All heap blocks were freed -- no leaks are possible
```

リークがあれば `definitely lost: X bytes` と表示される。

---

## 13. 評価（defense）でよく聞かれる質問の対策

1. **「コンストラクタ／デストラクタの呼び出し順を説明して」**
   → 親→子→（コピー対象の親→コピー対象の子）の構築、解体は逆順。

2. **「Animal のデストラクタを virtual にしないとどうなる？」**
   → `Animal*` で持ったまま `delete` すると `~Cat()` / `~Dog()` が呼ばれず、Brain がリークする。

3. **「pure virtual と virtual の違いは？」**
   → pure（`= 0`）は実装を持たない（持ててもいい）、そのクラスは抽象になりインスタンス化不可。

4. **「インターフェイスと抽象クラスの違いは？」**
   → C++ には interface キーワードがない。pure virtual のみで構成された抽象クラスを慣習的にインターフェイスと呼ぶ。

5. **「shallow copy がなぜ問題か？」**
   → 同じヒープを複数オブジェクトが指し、片方の delete で他方が dangling、二重 delete で UB。

6. **「Rule of Three とは？」**
   → デストラクタ／コピーコンストラクタ／コピー代入演算子のうち1つ実装が必要なら3つ全部書く。

7. **「`Animal a;` がコンパイルできない理由を ex02 で説明して」**
   → Animal が pure virtual を持つ抽象クラスだから。

8. **「ex03 の `unequip` がなぜ delete しない設計？」**
   → 装備を外した materia を誰が所有するかを呼出側に委ねる設計。ただしリーク回避は呼出側の責任になる。

9. **「AMateria の operator= で type をコピーすべきでない理由は？」**
   → type は具象クラス（Ice/Cure）のアイデンティティ。Ice インスタンスに Cure を代入したからといって type が "cure" になるのは矛盾する。

10. **「Character のインベントリは 4 つだが、満杯時の equip と未知 idx の use はどうなる？」**
    → 何もしない（subject 要件）。crash も assert も投げない。

---

## 14. 参考リンク（自分で深掘りするなら）

- cppreference.com — C++98/03 のシグネチャ確認
- Stroustrup, *The C++ Programming Language* — OOPの第一資料
- Effective C++ (Scott Meyers) — 多態・OCFの罠が網羅されている
- Bjarne Stroustrup の C++ FAQ — 抽象クラスやインターフェイスの設計思想

---

## まとめ：cpp04 で押さえるべき5つの柱

1. **継承の文法とアクセス指定**：`protected`、初期化子リスト、ctor/dtor の順
2. **virtual と pure virtual**：vtable のしくみと「virtual を忘れたときの怖さ」
3. **抽象クラスとインターフェイス**：pure virtual のみのクラスはインスタンス化不可
4. **Orthodox Canonical Form**：4要素必須、ポインタ持つなら deep copy
5. **メモリ管理**：new/delete のペア、virtual ~Destructor、リーク回避

cpp04 を超えると cpp05〜cpp08 はもう少し抽象的（例外、テンプレート、STL）になる。
ここで土台を固めれば後がぐっと楽になる。
