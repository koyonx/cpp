# CPP03 レビュー結果 & 必要知識まとめ

## レビュー結果（評価項目チェック）

### 前提条件
| 項目 | 結果 | 備考 |
|------|------|------|
| c++ + `-Wall -Wextra -Werror` でコンパイル | OK | 全exerciseで確認済み |
| C++98準拠 | OK | `-std=c++98 -pedantic` 付き |
| ヘッダに関数実装なし | OK | `.hpp`は宣言のみ |
| `using namespace` 不使用 | OK | |
| `friend` 不使用 | OK | |
| C関数（alloc/printf/free）不使用 | OK | |

### Exercise 00: Annnnnnd... ACTION!
| 項目 | 結果 | 備考 |
|------|------|------|
| ClapTrapクラス | OK | |
| private属性 (name, hitPoints, energyPoints, attackDamage) | OK | |
| 初期値 (HP=10, EP=10, AD=0) | OK | |
| attack(), takeDamage(), beRepaired() | OK | EP/HP消費のチェック付き |
| テスト十分 | OK | 11テスト |

### Exercise 01: Serena, My Love!
| 項目 | 結果 | 備考 |
|------|------|------|
| ScavTrapがClapTrapをpublic継承 | OK | |
| 属性の再宣言なし | OK | |
| ClapTrapの属性がprotected | OK | |
| 初期値 (HP=100, EP=50, AD=20) | OK | |
| attack(), takeDamage(継承), beRepaired(継承) | OK | |
| コンストラクタ/デストラクタのメッセージがClapTrapと異なる | OK | |
| 構築順序: ClapTrap→ScavTrap | OK | |
| 破棄順序: ScavTrap→ClapTrap | OK | |
| guardGate() | OK | |
| attack()がClapTrapと異なるメッセージ | OK | "ScavTrap" vs "ClapTrap" |

### Exercise 02: Assembly Line Work
| 項目 | 結果 | 備考 |
|------|------|------|
| FragTrapがClapTrapをpublic継承 | OK | |
| 属性の再宣言なし | OK | |
| 初期値 (HP=100, EP=100, AD=30) | OK | |
| 構築順序: ClapTrap→FragTrap | OK | |
| 破棄順序: FragTrap→ClapTrap | OK | |
| highFivesGuys() | OK | |
| **ScavTrap.{hpp,cpp}が含まれている** | OK (修正済み) | subject「Files from previous exercises」要件への対応 |

> **対応済み:** ex01からScavTrap.hpp/cppをコピーし、Makefileの`SRCS`に`ScavTrap.cpp`を追加した。subject (p.11) で「Files to turn in: **Files from previous exercises** + FragTrap.{h, hpp}, FragTrap.cpp」と指定されているため、ex01のScavTrap.hpp/cppも必要。

### Exercise 03: Ok, This Is Getting Weird
| 項目 | 結果 | 備考 |
|------|------|------|
| DiamondTrapがFragTrapとScavTrapを継承 | OK | |
| virtual継承を使用 | OK | ScavTrap/FragTrapが`virtual public ClapTrap` |
| DiamondTrap固有のprivate name | OK | |
| ClapTrap::nameに`_clap_name`を付加 | OK | |
| 属性値 (HP=FragTrap(100), EP=ScavTrap(50), AD=FragTrap(30)) | OK | |
| attack()がScavTrapのものを使用 | OK | `using ScavTrap::attack` |
| guardGate() + highFivesGuys() 両方使用可 | OK | |
| whoAmI()がDiamondTrap::nameとClapTrap::nameの両方にアクセス | OK | |

### 総合評価: **全項目 OK**

---

## subject「Files to turn in」要件の整理

各 exercise で提出すべきファイルは累積する。

| Exercise | 必須ファイル |
|----------|------------|
| ex00 | Makefile, main.cpp, ClapTrap.{h,hpp}, ClapTrap.cpp |
| ex01 | ex00のファイル全部 + ScavTrap.{h,hpp}, ScavTrap.cpp |
| ex02 | **ex00 + ex01のファイル全部** + FragTrap.{h,hpp}, FragTrap.cpp |
| ex03 | **ex00 + ex01 + ex02のファイル全部** + DiamondTrap.{h,hpp}, DiamondTrap.cpp |

ex02では「ScavTrapはもう要らない」と勘違いして削除しがちだが、subject では明確に「Files from previous exercises」と書かれているため、含める必要がある。Makefileの`SRCS`にも`ScavTrap.cpp`を追加してコンパイル対象とする（実際に使わなくても）。

---

## この課題を説明するために必要な知識

---

### 1. 継承 (Inheritance) — クラスの親子関係

#### 1-1. 継承とは何か

継承とは、既存のクラス（基底クラス/親クラス）の機能を引き継いで、新しいクラス（派生クラス/子クラス）を作る仕組み。

```cpp
class ClapTrap {          // 基底クラス（親）
protected:
    std::string name;
    unsigned int hitPoints;
    unsigned int energyPoints;
    unsigned int attackDamage;
public:
    void attack(const std::string& target);
    void takeDamage(unsigned int amount);
    void beRepaired(unsigned int amount);
};

class ScavTrap : public ClapTrap {  // 派生クラス（子）
public:
    void attack(const std::string& target);  // オーバーライド
    void guardGate();                        // 独自の機能を追加
};
```

**なぜ使うのか:**
- **コードの再利用**: `takeDamage()`や`beRepaired()`を書き直す必要がない
- **拡張性**: 既存コードを変更せずに新しい機能を追加できる
- **is-a関係の表現**: 「ScavTrapはClapTrapの一種である」

#### 1-2. 継承の種類 — public / protected / private

```cpp
class ScavTrap : public ClapTrap { };     // public継承（最も一般的）
class ScavTrap : protected ClapTrap { };  // protected継承
class ScavTrap : private ClapTrap { };    // private継承（デフォルト）
```

| 基底クラスでのアクセス | public継承後 | protected継承後 | private継承後 |
|----------------------|-------------|----------------|-------------- |
| public               | public      | protected      | private       |
| protected            | protected   | protected      | private       |
| private              | アクセス不可 | アクセス不可     | アクセス不可   |

**この課題ではpublic継承のみを使用。** public継承は「is-a」関係を表す:
- ScavTrap **is a** ClapTrap（ScavTrapはClapTrapの一種）
- ClapTrapのpublicメンバは、ScavTrapでもpublicのまま

---

### 2. アクセス修飾子 — private / protected / public

#### 2-1. 3つの違い

```
┌──────────┬────────────────┬────────────────┬────────────────┐
│          │ クラス自身     │ 派生クラス     │ 外部           │
├──────────┼────────────────┼────────────────┼────────────────┤
│ private  │ ○ アクセス可   │ × アクセス不可 │ × アクセス不可 │
│ protected│ ○ アクセス可   │ ○ アクセス可   │ × アクセス不可 │
│ public   │ ○ アクセス可   │ ○ アクセス可   │ ○ アクセス可   │
└──────────┴────────────────┴────────────────┴────────────────┘
```

#### 2-2. ex00 → ex01 での変更

```cpp
// ex00: ClapTrapの属性は private
class ClapTrap {
private:                    // ← 派生クラスからアクセス不可
    std::string name;
    unsigned int hitPoints;
};

// ex01: ClapTrapの属性は protected に変更
class ClapTrap {
protected:                  // ← 派生クラスからアクセス可能
    std::string name;
    unsigned int hitPoints;
};
```

**なぜprotectedに変えるのか:**
- ScavTrapのコンストラクタで`hitPoints = 100;`のように直接設定する必要がある
- `private`のままだと、派生クラスから直接アクセスできない
- `public`にすると外部から自由に変更できてしまい、カプセル化が壊れる
- `protected`は「派生クラスには公開するが、外部には非公開」という丁度良い制限

---

### 3. コンストラクタとデストラクタの呼び出し順序

#### 3-1. 構築順序（親 → 子）

派生クラスのオブジェクトを作ると、**基底クラスのコンストラクタが先に**呼ばれる。

```cpp
ScavTrap scav("Scavvy");
// 出力:
// ClapTrap Parameterized constructor called for Scavvy   ← 親が先
// ScavTrap Parameterized constructor called for Scavvy   ← 子が後
```

**なぜ親が先か:**
- 子クラスは親クラスのメンバ変数を使う可能性がある
- 親の部分が初期化されていないと、子クラスのコンストラクタで未初期化の変数にアクセスしてしまう
- 「土台（親）を先に作り、その上に建物（子）を作る」イメージ

#### 3-2. 破棄順序（子 → 親）— 構築の逆順

```cpp
{
    ScavTrap scav("Scavvy");
}  // スコープ終了
// 出力:
// ScavTrap Destructor called for Scavvy    ← 子が先
// ClapTrap Destructor called for Scavvy    ← 親が後
```

**なぜ子が先か:**
- 子のデストラクタが親のメンバを使う可能性がある
- 親を先に壊すと、子のデストラクタで破壊済みのデータにアクセスする危険性がある
- 「建物（子）を先に取り壊し、土台（親）を最後に壊す」イメージ

#### 3-3. 初期化リストでの親コンストラクタの呼び出し

```cpp
ScavTrap::ScavTrap(const std::string& trapName) : ClapTrap(trapName) {
//                                                 ^^^^^^^^^^^^^^^^
//                                    初期化リストで親のコンストラクタを明示的に呼ぶ
    hitPoints = 100;      // 親で10に初期化された後、子で100に上書き
    energyPoints = 50;
    attackDamage = 20;
}
```

- 初期化リストで`ClapTrap(trapName)`を呼ばないと、ClapTrapのデフォルトコンストラクタが呼ばれる
- 名前付きで構築したい場合は、必ず初期化リストで親のコンストラクタに引数を渡す

---

### 4. 関数のオーバーライド（名前隠蔽）

#### 4-1. オーバーライドとは

派生クラスで、基底クラスと**同じ名前・同じシグネチャ**の関数を再定義すること。

```cpp
// ClapTrapの attack()
void ClapTrap::attack(const std::string& target) {
    std::cout << "ClapTrap " << name << " attacks " << target << std::endl;
}

// ScavTrapの attack() — ClapTrapのattack()を「隠す」
void ScavTrap::attack(const std::string& target) {
    std::cout << "ScavTrap " << name << " attacks " << target << std::endl;
}
```

```cpp
ScavTrap scav("Scavvy");
scav.attack("Enemy");      // → "ScavTrap Scavvy attacks Enemy"  （ScavTrap版が呼ばれる）

ClapTrap clap("Clappy");
clap.attack("Enemy");      // → "ClapTrap Clappy attacks Enemy"  （ClapTrap版が呼ばれる）
```

#### 4-2. 名前隠蔽 (Name Hiding)

C++では、派生クラスで同名の関数を定義すると、基底クラスの同名関数が**全て隠される**。これは仮想関数でなくても起こる。

```cpp
class Base {
public:
    void func(int x);
    void func(double x);
};

class Derived : public Base {
public:
    void func(int x);       // Base::func(int)とBase::func(double)の両方が隠される
};

Derived d;
d.func(3.14);  // Base::func(double)ではなく、Derived::func(int)が呼ばれる（暗黙変換）
```

> **注意:** この課題ではvirtual関数を使っていない。C++98では多態性(ポリモーフィズム)を実現するためにvirtualキーワードが必要だが、この課題では単純な名前隠蔽で対応している。

---

### 5. ダイヤモンド継承問題と仮想継承

#### 5-1. ダイヤモンド継承とは

```
        ClapTrap
       /        \
  ScavTrap    FragTrap
       \        /
      DiamondTrap
```

DiamondTrapがScavTrapとFragTrapの両方を継承し、その両方がClapTrapを継承している。この菱形（ダイヤモンド）の継承関係が「ダイヤモンド継承」。

#### 5-2. 仮想継承なしの問題

仮想継承を使わないと、DiamondTrapの中にClapTrapのコピーが**2つ**存在する:

```
DiamondTrap:
├── ScavTrap部分:
│   └── ClapTrap部分 (1つ目のClapTrap)  ← name, hitPoints, ... のセットA
├── FragTrap部分:
│   └── ClapTrap部分 (2つ目のClapTrap)  ← name, hitPoints, ... のセットB
└── DiamondTrap固有部分
```

**問題点:**
- `diamond.name`が曖昧 — ScavTrap側のnameか、FragTrap側のnameか分からない
- メモリの無駄 — ClapTrapのデータが2重に保持される
- コンパイルエラー — どちらのClapTrapか指定しないとアクセスできない

#### 5-3. 仮想継承（virtual inheritance）による解決

```cpp
class ScavTrap : virtual public ClapTrap { };  // virtual をつける
class FragTrap : virtual public ClapTrap { };   // virtual をつける

class DiamondTrap : public ScavTrap, public FragTrap { };
```

仮想継承を使うと、ClapTrapのインスタンスは**1つだけ**になる:

```
DiamondTrap:
├── ScavTrap部分 ─────┐
├── FragTrap部分 ─────┤── 共有ClapTrap部分（1つだけ）
└── DiamondTrap固有部分│      name, hitPoints, energyPoints, attackDamage
```

**ポイント:**
- `virtual`は中間クラス（ScavTrap, FragTrap）につける — DiamondTrapではなく
- 共有されるClapTrap部分は1つだけなので、曖昧さが解消される

#### 5-4. 仮想継承時のコンストラクタ呼び出し

通常の継承では、基底クラスのコンストラクタは直接の子が呼ぶ。しかし仮想継承では、**最も派生したクラス（most derived class）** が仮想基底クラスのコンストラクタを呼ぶ。

```cpp
DiamondTrap::DiamondTrap(const std::string& diamondName)
    : ClapTrap(diamondName + "_clap_name"),  // ← 仮想基底クラスを最派生クラスが直接初期化
      ScavTrap(diamondName),
      FragTrap(diamondName),
      name(diamondName)
{
    hitPoints = 100;      // FragTrapの値
    energyPoints = 50;    // ScavTrapの値
    attackDamage = 30;    // FragTrapの値
}
```

**構築順序:**
1. `ClapTrap`のコンストラクタ（仮想基底 → 最初に呼ばれる）
2. `ScavTrap`のコンストラクタ（宣言順で先）
3. `FragTrap`のコンストラクタ
4. `DiamondTrap`のコンストラクタ

**破棄順序:** 上記の逆順（DiamondTrap → FragTrap → ScavTrap → ClapTrap）

**なぜ最派生クラスが仮想基底を初期化するのか:**
- ScavTrapとFragTrapの両方がClapTrapを初期化しようとすると、どちらの初期化を使うか曖昧になる
- そこで、最終的なクラス（DiamondTrap）が責任を持って1回だけ初期化する

---

### 6. `using` 宣言 — 特定の関数を選択する

#### 6-1. 曖昧さの解消

DiamondTrapはScavTrapとFragTrapの両方を継承しているため、`attack()`が曖昧になる:
- ScavTrapには独自の`attack()`がある
- FragTrapにはClapTrapから継承した`attack()`がある

```cpp
class DiamondTrap : public ScavTrap, public FragTrap {
public:
    using ScavTrap::attack;  // ScavTrapのattack()を使うことを明示
};
```

これにより、`diamond.attack("Enemy")`はScavTrapのattack()を呼ぶ。

> **注意:** ここでの`using`は名前空間の`using namespace`とは異なる。`using 基底クラス::メンバ名`は、基底クラスの特定のメンバを派生クラスのスコープに導入する宣言であり、禁止されている`using namespace`とは別物。

---

### 7. 多重継承 (Multiple Inheritance)

#### 7-1. 多重継承とは

1つのクラスが複数のクラスを同時に継承すること。

```cpp
class DiamondTrap : public ScavTrap, public FragTrap {
    // ScavTrapとFragTrap両方のメンバ関数・属性を持つ
};
```

DiamondTrapは:
- ScavTrapの`guardGate()`を使える
- FragTrapの`highFivesGuys()`を使える
- 両方のattack()を持つ → `using`で解決

#### 7-2. 多重継承の注意点

- **名前の衝突**: 両親に同名の関数があると曖昧になる → `using`やスコープ解決演算子(`ScavTrap::attack`)で解決
- **ダイヤモンド問題**: 共通の祖先がいる場合 → 仮想継承で解決
- **設計の複雑化**: 多重継承は慎重に使う必要がある

---

### 8. スコープ解決演算子 `::` の使い方

```cpp
// 1. 名前空間のアクセス
std::cout << "Hello" << std::endl;

// 2. 基底クラスのメンバに明示的にアクセス
void DiamondTrap::whoAmI() {
    std::cout << "DiamondTrap name: " << name << std::endl;           // DiamondTrap::name
    std::cout << "ClapTrap name: " << ClapTrap::name << std::endl;   // ClapTrap::name
}

// 3. 基底クラスの演算子を呼ぶ
DiamondTrap& DiamondTrap::operator=(const DiamondTrap& other) {
    ClapTrap::operator=(other);  // 基底クラスの代入演算子を明示的に呼ぶ
    name = other.name;
    return *this;
}
```

`ClapTrap::name`とすることで、DiamondTrap独自の`name`ではなく、基底クラスClapTrapの`name`にアクセスできる。

---

### 9. DiamondTrapの属性値の由来

DiamondTrapの属性は、特定の親クラスから値を受け取る:

```
┌──────────────┬──────────────┬──────────────┐
│ 属性         │ 値           │ 由来         │
├──────────────┼──────────────┼──────────────┤
│ hitPoints    │ 100          │ FragTrap     │
│ energyPoints │ 50           │ ScavTrap     │
│ attackDamage │ 30           │ FragTrap     │
│ attack()     │ ScavTrapの実装│ ScavTrap     │
│ name         │ 独自のname   │ DiamondTrap  │
│ ClapTrap::name│ name + "_clap_name" │ ClapTrap │
└──────────────┴──────────────┴──────────────┘
```

---

### 10. Orthodox Canonical Form（正統カノニカル形式）の継承クラスでの実装

継承クラスでも4つの特殊関数が必要:

```cpp
class ScavTrap : public ClapTrap {
public:
    // (1) デフォルトコンストラクタ — 親のデフォルトコンストラクタを呼ぶ
    ScavTrap();

    // (2) コピーコンストラクタ — 親のコピーコンストラクタを呼ぶ
    ScavTrap(const ScavTrap& other);

    // (3) コピー代入演算子 — 親の代入演算子を呼ぶ
    ScavTrap& operator=(const ScavTrap& other);

    // (4) デストラクタ — 親のデストラクタは自動で呼ばれる
    ~ScavTrap();
};
```

**実装のポイント:**

```cpp
// コピーコンストラクタ: 初期化リストで親のコピーコンストラクタを呼ぶ
ScavTrap::ScavTrap(const ScavTrap& other) : ClapTrap(other) {
//                                          ^^^^^^^^^^^^^^
//                    ScavTrapはClapTrapの派生なので、ClapTrap(const ClapTrap&)に渡せる
}

// コピー代入演算子: 親の代入演算子を明示的に呼ぶ
ScavTrap& ScavTrap::operator=(const ScavTrap& other) {
    if (this != &other) {
        ClapTrap::operator=(other);  // 親の部分をコピー
    }
    return *this;
}
```

---

### 11. 名前隠蔽の落とし穴 — DiamondTrapにおける `name`

DiamondTrapはprivate属性として`name`を持つ。これはClapTrapの`name`と**同じ変数名**だが、別の変数:

```cpp
class DiamondTrap : public ScavTrap, public FragTrap {
private:
    std::string name;  // ← DiamondTrap独自のname (ClapTrap::nameとは別物)
};
```

**DiamondTrapインスタンスは2つのnameを持つ:**
- `DiamondTrap::name` — DiamondTrap独自のもの (例: `"Diamond"`)
- `ClapTrap::name` — 継承で受け継いだもの (例: `"Diamond_clap_name"`)

```cpp
void DiamondTrap::whoAmI() {
    std::cout << name << std::endl;            // DiamondTrap::name → "Diamond"
    std::cout << this->name << std::endl;      // 同上 → "Diamond"
    std::cout << ClapTrap::name << std::endl;  // ClapTrap::name → "Diamond_clap_name"
}
```

**なぜ2つのnameが必要なのか:**
subject (p.12) で明示的に要求されている:
- `DiamondTrap::name`は「ロボットの名前」 (e.g., "Diamond")
- `ClapTrap::name`はそれに`_clap_name`を付けた形 (e.g., "Diamond_clap_name")

これは「同じ変数名を派生クラスで再宣言すると基底クラスのものが隠蔽される」という名前隠蔽 (name hiding) の典型例。`ClapTrap::`スコープ解決で明示的にアクセスする必要がある。

---

### 12. 仮想継承時の構築/破棄の詳細

#### 12-1. 仮想基底クラスの構築は最派生クラスが担う

通常の継承では、派生クラスが直接の基底クラスを初期化する。しかし**仮想継承**では、仮想基底クラスは「最派生クラス (most derived class)」が直接初期化する。中間クラスの初期化リストに書かれた仮想基底クラスへの呼び出しは**無視される**。

```cpp
// ScavTrapがstandalone (most derived)
ScavTrap scav("Test");
// → ScavTrapの`: ClapTrap(trapName)` が機能して ClapTrap("Test") が呼ばれる

// DiamondTrapが most derived
DiamondTrap diamond("Diamond");
// → DiamondTrapの `: ClapTrap("Diamond_clap_name")` が呼ばれる
// → ScavTrap・FragTrapの `: ClapTrap(...)` は無視される
```

#### 12-2. DiamondTrap構築時の実際の流れ

```cpp
DiamondTrap::DiamondTrap(const std::string& diamondName)
    : ClapTrap(diamondName + "_clap_name"),  // ← 仮想基底を最派生が初期化
      ScavTrap(diamondName),
      FragTrap(diamondName),
      name(diamondName)
{
    hitPoints = 100;
    energyPoints = 50;
    attackDamage = 30;
}
```

`DiamondTrap diamond("Diamond")` を実行すると:

1. **ClapTrap("Diamond_clap_name")** が呼ばれる (initializer listの位置に関わらず最初)
   - name = "Diamond_clap_name", HP=10, EP=10, AD=0
   - 出力: `ClapTrap Parameterized constructor called for Diamond_clap_name`
2. **ScavTrap("Diamond")** が呼ばれる
   - ScavTrapの `: ClapTrap(trapName)` は**無視**される
   - 本体: HP=100, EP=50, AD=20
   - 出力: `ScavTrap Parameterized constructor called for Diamond_clap_name` (ClapTrap::nameは"Diamond_clap_name")
3. **FragTrap("Diamond")** が呼ばれる
   - FragTrapの `: ClapTrap(fragName)` も**無視**される
   - 本体: HP=100, EP=100, AD=30
   - 出力: `FragTrap Parameterized constructor called for Diamond_clap_name`
4. **DiamondTrap本体**
   - HP=100, EP=50, AD=30 で上書き
   - 出力: `DiamondTrap Parameterized constructor called for Diamond` (DiamondTrap::nameを参照)

**ポイント:** ScavTrap/FragTrapのコンストラクタ内で`name`を出力すると、そこで参照される`name`はClapTrap::nameなので「Diamond_clap_name」が表示される。これはバグではなく仮想継承の正しい挙動。

#### 12-3. 「継承」を最大限活かすDiamondTrap本体の書き方

ScavTrap本体実行後: HP=100, EP=50, AD=20
FragTrap本体実行後: HP=100, EP=100, AD=30 (ScavTrapの値が上書きされる)

subjectの要求:
- HP=100 (FragTrap) ← FragTrap本体で**既に正しい**
- EP=50 (ScavTrap) ← FragTrap本体で100に上書きされている → 50に戻す必要あり
- AD=30 (FragTrap) ← FragTrap本体で**既に正しい**

#### Bad: 全部hardcodeするとどうなるか

```cpp
DiamondTrap::DiamondTrap(const std::string& diamondName)
    : ClapTrap(diamondName + "_clap_name"), ScavTrap(diamondName), FragTrap(diamondName), name(diamondName) {
    hitPoints = 100;       // ← 不要 (FragTrapが既に設定してる)
    energyPoints = 50;
    attackDamage = 30;     // ← 不要 (FragTrapが既に設定してる)
}
```

これは「継承」していない。FragTrapを継承しているのに、その値を活かさず固定値で上書きしている。レビューで「継承の意味がない」と指摘されかねない。

#### Good: 最小限の上書きで継承を活かす

```cpp
DiamondTrap::DiamondTrap(const std::string& diamondName)
    : ClapTrap(diamondName + "_clap_name"), ScavTrap(diamondName), FragTrap(diamondName), name(diamondName) {
    energyPoints = 50;     // FragTrap本体で100に上書きされたものを、ScavTrapの値に戻す
}
```

- HP=100 と AD=30 はFragTrapの初期化処理で自動的に設定される (= 「FragTrapから継承」している)
- EPだけはFragTrapが100にしてしまうので、ScavTrapの50に戻す
- これでsubjectの「Hit points (FragTrap), Energy points (ScavTrap), Attack damage (FragTrap)」を構築の流れで実現できる

**ポイント:** 「継承で取得できるものは継承で取得し、構築順序の都合で上書きが必要なものだけ明示的に書く」ことで、コードの意図が明確になる。

#### 12-4. 破棄順序

破棄は構築の完全な逆順:
1. DiamondTrap デストラクタ
2. FragTrap デストラクタ
3. ScavTrap デストラクタ
4. ClapTrap デストラクタ (仮想基底は最後)

---

### 13. 仮想継承のメモリレイアウトと vptr

仮想継承を使うと、コンパイラは「仮想基底クラスへのポインタ (vbptr)」を内部的に追加する。

```
通常の継承 (ダイヤモンド問題あり):
DiamondTrap {
    ScavTrap {
        ClapTrap { name, hitPoints, ... }   // コピー1
    }
    FragTrap {
        ClapTrap { name, hitPoints, ... }   // コピー2 (重複!)
    }
    name (DiamondTrap独自)
}

仮想継承 (ダイヤモンド問題解決):
DiamondTrap {
    ScavTrap {
        vbptr → ↓
    }
    FragTrap {
        vbptr → ↓
    }
    name (DiamondTrap独自)
    ClapTrap { name, hitPoints, ... }  // 1つだけ、共有
}
```

- 仮想継承により、ClapTrapのインスタンスは1つだけ
- ScavTrap/FragTrapは内部の`vbptr`を介して共有のClapTrapにアクセス
- メモリ的にやや複雑だが、安全性と意味論の正しさが保証される

---

### 14. `using` 宣言 vs `using namespace`

これらは**全く別物**である点に注意:

| 構文 | 意味 | この課題での扱い |
|------|------|----------------|
| `using ScavTrap::attack;` | 基底クラスのメンバを派生クラスのスコープに導入 | **使用OK** (ex03で必要) |
| `using namespace std;` | 名前空間全体を取り込む | **禁止** (subject違反) |

```cpp
// OK: 名前隠蔽の解決手段としての using宣言
class DiamondTrap : public ScavTrap, public FragTrap {
public:
    using ScavTrap::attack;  // attackの曖昧さを解決
};

// NG: subject禁止
using namespace std;  // ← grade -42
```

#### 14-1. なぜusing宣言が必要か

DiamondTrapの`attack()`は曖昧:
- ScavTrapから`attack()`を継承 (ScavTrap独自定義)
- FragTrapから`attack()`を継承 (ClapTrapから継承したもの)

`using`なしで`diamond.attack("foo")`と書くと:
```
error: request for member 'attack' is ambiguous
```

3つの解決策:
1. `using ScavTrap::attack;` (推奨) ← この課題で採用
2. DiamondTrapで独自の`attack()`を実装
3. 呼び出し時に`diamond.ScavTrap::attack("foo")`と明示 (使う側が面倒)

---

### 15. よく聞かれる質問と回答例

### Q: なぜex00ではprivateだった属性を、ex01でprotectedに変えるのか？
**A:** 派生クラス（ScavTrap）のコンストラクタで`hitPoints = 100`のように直接属性を設定する必要があるため。`private`のままでは派生クラスからアクセスできない。`public`にすると外部から自由に変更でき、カプセル化が崩れる。`protected`は派生クラスにのみアクセスを許可する中間的なアクセスレベル。

### Q: コンストラクタの呼び出し順序はなぜ親→子なのか？
**A:** 子クラスは親クラスのメンバ変数を使う可能性がある。もし子が先に初期化されると、まだ初期化されていない親のメンバにアクセスしてしまう。建物の例え: 土台（親）が完成してから上の階（子）を建てる。デストラクタはその逆順で、上の階（子）を先に壊してから土台（親）を壊す。

### Q: 仮想継承がないとどうなるのか？
**A:** ClapTrapのインスタンスが2つ存在してしまう。例えば`diamond.name`にアクセスしようとすると、ScavTrap経由のnameかFragTrap経由のnameか曖昧になり、コンパイルエラーになる。メモリも2倍消費される。`virtual`をつけることで、ClapTrapのインスタンスを1つに統合する。

### Q: `using ScavTrap::attack`と`using namespace`の違いは？
**A:** 全く別物。`using ScavTrap::attack`は特定の基底クラスの特定のメンバ関数を派生クラスのスコープに導入する宣言。`using namespace std`は名前空間全体を取り込む宣言で、この課題では禁止されている。`using 基底クラス::メンバ`は名前隠蔽の解決手段として正当な使い方。

### Q: なぜDigmondTrapのコンストラクタで直接ClapTrapを初期化するのか？
**A:** 仮想継承では、仮想基底クラスのコンストラクタは「最も派生したクラス」が呼ぶ規則がある。ScavTrapやFragTrapが各自ClapTrapを初期化しようとすると、どちらの初期化を使うか決められない。そのため、最終的なDiamondTrapが責任を持って1回だけClapTrapを初期化する。

### Q: DiamondTrapにprivate nameが必要な理由は？
**A:** 課題の仕様で、DiamondTrapは独自のnameを持ち、ClapTrap::nameには`"<name>_clap_name"`を設定する。whoAmI()で「自分の名前」と「ClapTrapとしての名前」の両方を表示するために、2つのnameが必要。同名のメンバ変数を派生クラスで宣言すると、基底クラスのメンバは隠されるが、`ClapTrap::name`で明示的にアクセスできる。

### Q: ScavTrapのattack()はオーバーライドか？virtual関数か？
**A:** この課題ではvirtualキーワードを使っていない。ScavTrapのattack()はClapTrapのattack()を「名前隠蔽」(name hiding)している。ポインタや参照を通じた多態性は働かない。例えば`ClapTrap* ptr = &scav; ptr->attack("target");`とすると、ClapTrapのattack()が呼ばれる。virtualをつけた場合のみ、ScavTrapのattack()が呼ばれる（この課題では不要）。

### Q: なぜDiamondTrapの中にClapTrap::nameとDiamondTrap::nameの2つのnameがあるのか？仮想継承で1つに統合されるはずでは？
**A:** 仮想継承で1つに統合されるのは「ClapTrapインスタンス」のうちの`name`変数のみ。DiamondTrap内で改めて`std::string name;`と宣言された`DiamondTrap::name`は完全に別の変数。subjectは「ロボット名 (DiamondTrap::name)」と「ClapTrap名 (= ロボット名+"_clap_name")」を区別して持つことを要求しており、これを実現するために2つのnameが必要。`whoAmI()`で`name`と`ClapTrap::name`の両方を表示することで、両者の存在を確認できる。

### Q: ex02でなぜScavTrapファイルも必要？ex02のコードはScavTrapを使っていないのに。
**A:** subject (p.11) で「Files to turn in: **Files from previous exercises** + FragTrap.{h, hpp}, FragTrap.cpp」と指定されている。「Files from previous exercises」とは前のexercise群の全ファイルを意味するため、ex01のScavTrapファイルもex02に含める必要がある。Makefileの`SRCS`にも`ScavTrap.cpp`を追加してコンパイル対象とする（main.cppから使われなくても）。peer reviewerが厳格に確認するとscubject違反として指摘される。

### Q: ex03でScavTrap/FragTrapのコンストラクタが`: ClapTrap(trapName)`と書いているのに、DiamondTrapを作るとそれが無視される理由は？
**A:** 仮想継承では、仮想基底クラスのコンストラクタは「最派生クラス (most derived class)」が直接呼ぶ規則がある。DiamondTrapが最派生のとき、DiamondTrapの初期化リストに書いた`: ClapTrap("Diamond_clap_name")`が採用される。ScavTrap/FragTrapの初期化リストにある`ClapTrap(...)`は**スキップ**される。一方、`ScavTrap scav("Test")`のようにScavTrap自体が最派生のときは、ScavTrapの`: ClapTrap("Test")`がちゃんと呼ばれる。両方のケースで動くように両方のクラスがClapTrapの初期化を書いておく必要がある。

### Q: DiamondTrapで`hitPoints = 100; energyPoints = 50; attackDamage = 30;`と全部hardcodeするのは「継承していない」ことにならない？
**A:** **ならないとは言い切れず、批判の余地がある。** 厳密に言えば、全部hardcodeするのは「継承を活かしていない」と指摘される可能性が高い。subjectの「Hit points (FragTrap)」「Attack damage (FragTrap)」は「FragTrapから継承された値である」という意味であり、FragTrapの初期化処理を活用するのが本来の継承の使い方。

**理想的な実装:**
```cpp
DiamondTrap::DiamondTrap(const std::string& diamondName)
    : ClapTrap(diamondName + "_clap_name"), ScavTrap(diamondName), FragTrap(diamondName), name(diamondName) {
    energyPoints = 50;  // EPだけScavTrapの値に戻す（FragTrapが100に上書きしたため）
}
```

構築順序の都合：
1. ClapTrap → HP=10, EP=10, AD=0
2. ScavTrap本体 → HP=100, EP=50, AD=20
3. FragTrap本体 → HP=100, EP=100, AD=30  ← 終了時点
4. DiamondTrap本体 → ここに来る前の状態を見ると、HP・ADは既にFragTrap値で正しい

つまりHP=100とAD=30は**FragTrapから継承された結果として既に設定されている**。EPだけFragTrapが100に上書きしてしまったので、ScavTrapの50に戻すために明示的に代入する。

これにより「FragTrapから継承される値はFragTrapに任せ、ScavTrapから取りたい値だけ明示的に書く」という形になり、subjectの仕様（「Hit points (FragTrap), Energy points (ScavTrap), Attack damage (FragTrap)」）と実装が対応する。

### Q: 構築/破棄メッセージの順序が正しいかどうか、どうやって確認する？
**A:** 単独のScavTrapやFragTrap、そしてDiamondTrapのインスタンスをスコープ内に作り、そのスコープから抜ける時のメッセージを観察する。期待される順序:
- **構築**: ClapTrap → (ScavTrap →) (FragTrap →) [DiamondTrap]
- **破棄**: 構築の完全な逆順
仮想継承を使ったDiamondTrapの場合、ClapTrapが最初に構築され、最後に破棄される。コンストラクタ/デストラクタのメッセージで`name`を表示すれば、どのオブジェクトのものか視覚的に確認できる。

### Q: クラス定義で空の`private:`セクションは必要？
**A:** 不要。例えば以下は意味のないコード:
```cpp
class ScavTrap : public ClapTrap {
public:
    ScavTrap();
    void guardGate();
private:    // ← 空のprivateセクションは無意味

};
```
`private:`は「これ以降のメンバはprivate」という宣言なので、後続にメンバが何もないなら書く意味がない。コンパイルは通るが、コードの読み手に「ここに何かprivateメンバを追加するつもりだった？」と疑問を持たせる。削除すべき。

### Q: -Wshadowはなぜサブジェクトに言及されているのか？
**A:** subject (p.13) で「Do you know the -Wshadow and -Wno-shadow compiler flags?」とヒントが書かれている。`-Wshadow`は「派生クラスで基底クラスのメンバ変数と同名の変数を宣言」した時に警告を出すフラグ。DiamondTrapで`std::string name;`と宣言するとClapTrap::nameと名前が衝突するため、`-Wshadow`を有効にすると警告される。ただし、subjectは「同じ変数名で宣言せよ」と要求しているので、これは意図的な衝突。`-Wno-shadow`で警告を抑制できる。`-Wall -Wextra`にはshadow警告は含まれないため、本課題では特に対処不要。
