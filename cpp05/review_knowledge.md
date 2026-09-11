# cpp05 レビュー対策まとめ

評価シートの各項目を `何を聞かれているか / なぜ大事か / 自分のコードでどう満たしているか / 想定問答` の 4 観点で整理する。

---

## 0. 今回の調査で修正した点（重要）

### 🔴 致命的だったもの: `-Werror` でコンパイルが通らなかった

修正前、**全 4 exercise がビルド不能**だった。

```
main.cpp:178:5: error: explicitly assigning value of variable of type 'Bureaucrat' to itself
                       [-Werror,-Wself-assign-overloaded]
  178 |         s = s;
```

- `s = s;`（自己代入テスト）が clang の `-Wself-assign-overloaded` に引っかかり、`-Werror` でエラー化していた
- この警告は **clang 固有**。Linux の g++ では出ないが、42 のキャンパスは Mac (clang) が主流なので、そのままだと **Prerequisites の「The code must compile with c++ and the flags -Wall -Wextra -Werror」で即 No** になる
- 修正: ポインタ経由に変えて「自明な自己代入」判定を回避

```cpp
Bureaucrat s("Self", 42);
Bureaucrat* self = &s;
s = *self;               // 意味は同じだがコンパイラは自己代入と断定できない
```

該当 4 箇所を修正済み (`ex00/main.cpp`, `ex01/main.cpp`, `ex02/main.cpp` ×3, `ex03/main.cpp`)。

### 🟡 Intern を「メンバ関数ポインタ配列」に書き換え

評価シートの文言が `array of pointers to **member** functions` なので、文字どおりメンバ関数ポインタにした（詳細は ex03 の章）。

### 🟡 Makefile にヘッダ依存を追加

`$(OBJS): $(HDRS)` を追加。これが無いと `.hpp` を編集しても `.o` が再ビルドされず、評価中に評価者がヘッダを触ったとき `make` が「何もすることがありません」と言い出して混乱する。

---

## 1. Prerequisites（前提チェック）

| 項目 | 状態 | 根拠 |
|---|---|---|
| `c++` + `-Wall -Wextra -Werror` でコンパイル | ✅ | Makefile の `CXXFLAGS` に加え `-std=c++98 -pedantic` も付与。全 ex で警告 0 |
| C++98 準拠 | ✅ | `-std=c++98 -pedantic` で通る。`nullptr` / `auto` / `noexcept` / `<random>` 等は不使用 |
| ヘッダに関数実装なし（テンプレート除く） | ✅ | 全ての `.hpp` は宣言のみ。例外クラスの `what()` も `.cpp` で定義 |
| C 関数 (`*alloc`, `*printf`, `free`) 不使用 | ✅ | grep 済み。0 件 |
| `using namespace` / `friend` 不使用 | ✅ | grep 済み。0 件 |
| 外部ライブラリ・C++11 機能不使用 | ✅ | 標準ヘッダのみ。STL コンテナ/`<algorithm>` も不使用 |
| include guard | ✅ | 全 `.hpp` にあり |
| ヘッダ単独インクルード可 | ✅ | 14 ヘッダすべて単独でコンパイル確認済み |
| メモリリーク | ✅ | `leaks --atExit` で全 ex `0 leaks for 0 total leaked bytes` |

### `std::rand()` は大丈夫？（聞かれる可能性あり）

`RobotomyRequestForm` で `std::rand()` を使っている。これは **問題ない**。PDF の General rules は禁止関数を明示列挙している:

> The following functions are forbidden too: `*printf()`, `*alloc()` and `free()`. If you use them, your grade will be 0 and that's it.

`rand()` はこのリストに含まれない。かつ C++98 には `<random>` が無いので「50% の確率」を実装する手段が他にない。評価シートの Forbidden Function 欄も `(*alloc, *printf, free)` と括弧書きで同じ 3 種を指している。

同様に `main.cpp` のテスト後始末で使う `std::remove()`（`<cstdio>`, ファイル削除）も禁止リスト外。C++98 にファイル削除の代替が無いため。

---

## 2. Ex00: Bureaucrat

### 評価項目: 定数 name / grade 1〜150 / 例外 / アクセサ / increment・decrement / `<<` オーバーロード

#### 自分のコードでどう満たしているか

```cpp
// Bureaucrat.hpp
const std::string _name;   // ← const、private
int               _grade;
```

- **grade 検証**: `Bureaucrat.cpp:8-11`

```cpp
Bureaucrat::Bureaucrat(const std::string& name, int grade)
    : _name(name), _grade(grade) {
    if (grade < 1)   throw GradeTooHighException();
    if (grade > 150) throw GradeTooLowException();
}
```

- **increment / decrement**: `Bureaucrat.cpp:33-43`

```cpp
void Bureaucrat::incrementGrade() {
    if (_grade - 1 < 1) throw GradeTooHighException();
    _grade -= 1;                       // grade 3 → grade 2（数字が減る = 昇格）
}
void Bureaucrat::decrementGrade() {
    if (_grade + 1 > 150) throw GradeTooLowException();
    _grade += 1;
}
```

**ポイント**: 「先にチェック、後で変更」の順。逆にすると例外送出時に grade が壊れた値のまま残る。`main.cpp` の test 8 / test 10 で「失敗しても grade が変わっていないこと」を検証している。

- **例外は `std::exception` 派生**: `Bureaucrat.hpp:26-33`

```cpp
class GradeTooHighException : public std::exception {
public:
    virtual const char* what() const throw();
};
```

`catch (std::exception& e)` で捕まえられることを `main.cpp` test 18 で実証済み。

- **`<<` オーバーロード**: PDF 指定の書式 `<name>, bureaucrat grade <grade>.` に**完全一致**。`main.cpp` test 21 で `oss.str() == "FormatTest, bureaucrat grade 42."` と厳密比較している。

#### 想定問答

**Q. コンストラクタで throw したら、既に初期化された `_name` はリークしないの？**
A. しません。C++ は「コンストラクタが例外で抜けたとき、既に構築完了したメンバのデストラクタを逆順で呼ぶ」と規定しています（stack unwinding）。`_name`（std::string）のデストラクタが呼ばれるので確保済みバッファは解放されます。ただし**そのオブジェクト自身のデストラクタは呼ばれません**（構築が完了していないので）。

**Q. なぜ `_grade` を初期化子リストで代入してから検証するの？**
A. `_grade` は const ではないので後から代入できますが、初期化子リストで一貫して初期化しています。例外が飛べばオブジェクトは存在しないことになるので、不正な値が外から観測されることはありません。

**Q. `what()` の後ろの `throw()` は何？**
A. C++98 の例外指定で「この関数は例外を投げない」宣言です。`std::exception::what()` の宣言がそうなっているので、オーバーライド側も同じか、より厳しい指定でないとコンパイルエラーになります。C++11 以降は `noexcept` に置き換わりました。

**Q. `operator=` が name をコピーしないのはバグでは？**
A. `_name` が `const std::string` なので代入不可能です。仕様上 name は不変なので、grade だけコピーするのが正しい設計です。`main.cpp` test 13 でこの挙動を明示テストしています。

---

## 3. Ex01: Form

### 評価項目: name / signed(初期 false) / gradeToSign / gradeToExecute、全て private、name と grade は const

```cpp
// Form.hpp:10-14
private:
    const std::string _name;
    bool              _signed;        // ← ここだけ非 const（署名で変わるので）
    const int         _gradeToSign;
    const int         _gradeToExecute;
```

**評価シートに "All these attributes are private and not protected" と明記**されている。protected にしていないこと、ex02 でも AForm の属性が private のままであることを指差せるようにしておく。

### `beSigned()` / `signForm()`

```cpp
// Form.cpp:31-35
void Form::beSigned(const Bureaucrat& b) {
    if (b.getGrade() > _gradeToSign) throw GradeTooLowException();
    _signed = true;
}
```

PDF: *"It changes the form's status to signed if the bureaucrat's grade is high enough (greater than or equal to the required one)"* → `grade <= gradeToSign` なら OK。つまり `grade > gradeToSign` のときだけ throw。**境界値 `grade == gradeToSign` は成功**（test 8 で検証）。

```cpp
// Bureaucrat.cpp:41-49
void Bureaucrat::signForm(Form& form) const {
    try {
        form.beSigned(*this);
        std::cout << _name << " signed " << form.getName() << std::endl;
    } catch (std::exception& e) {
        std::cout << _name << " couldn't sign " << form.getName()
                  << " because " << e.what() << "." << std::endl;
    }
}
```

PDF の出力書式 `<bureaucrat> signed <form>` / `<bureaucrat> couldn't sign <form> because <reason>.` に一致。

### 例外メッセージの設計（`<reason>` に埋め込まれる）

`what()` の戻り値は `because` に続く**理由節としてそのまま文になる**ので、クラス名プレフィックスを付けていない。

```cpp
const char* Form::GradeTooLowException::what() const throw() {
    return "the grade is too low";     // ○ "... because the grade is too low."
 // return "Form: grade too low";      // × "... because Form: grade too low." → 文が壊れる
}
```

実際の出力:

```
Alice signed Contract
Bob couldn't sign Contract because the grade is too low.
```

ex02 の `executeForm` も同じ文に埋め込まれるので、`AForm` / `Bureaucrat` の全例外で表現を統一している。

```
Walter couldn't execute presidential pardon because the form is not signed.
Walter couldn't execute presidential pardon because the grade is too low.
Sarah executed presidential pardon
```

**想定問答 — どのクラスが投げた例外か分からなくならない？**
A. メッセージから型を判別する必要はありません。型で catch すれば `Form::GradeTooLowException` か `Bureaucrat::GradeTooLowException` かは静的に区別できます（`main.cpp` で具体型での catch をテスト済み）。`what()` は PDF 指定の出力文の一部として使われるので、文として読める表現を優先しました。

### 循環インクルードの回避

- `Bureaucrat.hpp` は `class Form;`（**前方宣言**）だけ持つ
- `Form.hpp` は `#include "Bureaucrat.hpp"` する
- `Bureaucrat.cpp` で初めて `#include "Form.hpp"`

これで「Bureaucrat.hpp → Form.hpp → Bureaucrat.hpp → …」の無限ループが起きない。

#### 想定問答

**Q. なぜ前方宣言で足りるの？**
A. `Bureaucrat.hpp` では `Form` を**参照 (`Form&`) としてしか使っていない**からです。参照やポインタはサイズが決まっているので、クラスの完全な定義は不要（incomplete type で OK）。実際にメンバ関数 `beSigned()` を呼ぶ `.cpp` 側でだけ完全定義が要ります。

**Q. 既に署名済みの Form にもう一度 `beSigned` したら？**
A. 例外は投げず、`_signed = true` のままです。仕様に明記が無いので冪等（idempotent）な設計を選びました。test 12 で検証しています。

**Q. `signForm` が例外を外に投げないのは？**
A. PDF が「失敗時はメッセージを表示する」と指定しているので、`signForm` が例外ハンドラの役割を持ちます。呼び出し側は try/catch 不要。test 25 で「絶対に伝播しない」ことを検証しています。

---

## 4. Ex02: AForm + concrete forms

### 評価項目: 抽象基底クラス / 3 つの具象クラス / コンストラクタ引数は target 1 つ / `execute()` / `executeForm()`

| クラス | sign / exec | 動作 |
|---|---|---|
| `ShrubberyCreationForm` | 145 / 137 | `<target>_shrubbery` に ASCII の木を書き込む |
| `RobotomyRequestForm` | 72 / 45 | ドリル音 → 50% で成功／失敗 |
| `PresidentialPardonForm` | 25 / 5 | `<target> has been pardoned by Zaphod Beeblebrox.` |

grade の数値は `main.cpp` test 1 で PDF どおりか厳密チェックしている。

### 設計: テンプレートメソッドパターン（PDF の "one way is more elegant"）

評価シートいわく「execute を純粋仮想にして各サブクラスでチェックする」か「基底で チェック → 派生の実行関数を呼ぶ」の**どちらでも可**。本実装は**後者**。

```cpp
// AForm.hpp
public:
    void execute(const Bureaucrat& executor) const;      // 共通の枠（非 virtual）
protected:
    virtual void action() const = 0;                     // 子が埋める穴
```

```cpp
// AForm.cpp:33-37
void AForm::execute(const Bureaucrat& executor) const {
    if (!_signed)                              throw FormNotSignedException();
    if (executor.getGrade() > _gradeToExecute) throw GradeTooLowException();
    action();                                  // ← 派生クラスへ委譲
}
```

**利点を言えるようにしておく:**
- 前提チェック（署名済み・grade 十分）が 1 箇所に集約 = DRY
- 新しい Form 型を足すとき `action()` だけ書けばよく、チェック漏れが構造的に起きない
- `action()` を `protected` にすることで、チェックを飛ばして直接実行することが外部からできない

### 抽象クラスであること

`action() = 0` があるので `AForm a;` はコンパイルエラー（確認済み: `variable type 'AForm' is an abstract class`）。

### 仮想デストラクタ

```cpp
virtual ~AForm();
```

`AForm* f = new ShrubberyCreationForm("x"); delete f;` で子のデストラクタが呼ばれるために必須。無いと `_target`（std::string）がリークする。`main.cpp` test 16 で各型 500 回 new/delete し、`leaks` で 0 リークを確認。

### 派生クラスの OCF

```cpp
ShrubberyCreationForm& ShrubberyCreationForm::operator=(const ShrubberyCreationForm& other) {
    if (this != &other) {
        AForm::operator=(other);     // ← 先に親を代入
        _target = other._target;
    }
    return *this;
}
```

コピーコンストラクタも `: AForm(other), _target(other._target)` と親の初期化を初期化子リストで明示している。これを書かないと親のデフォルトコンストラクタが呼ばれてしまう。

#### 想定問答

**Q. 「コンストラクタは target 1 引数だけ」なのにデフォルトコンストラクタがあるのは矛盾では？**
A. PDF が「Module 02〜09 の全クラスは Orthodox Canonical Form」を要求しているので、デフォルトコンストラクタは必須です。評価シートも「非インターフェースクラスが OCF でなければ採点するな」と書いています。「実用上使う公開コンストラクタは target 1 引数のもの」で、デフォルトコンストラクタは OCF 要件を満たすためのものです。

**Q. `action()` が引数を取らないのはなぜ？**
A. 必要ないからです。`executor` を使う処理（署名済みか・grade が足りるか）は全て `AForm::execute()` 側で完了しており、`action()` に到達した時点で「実行してよい」ことは確定しています。具象3クラスのどれも executor の情報を使わないので、渡しても `(void)executor;` で捨てるだけの死んだ引数になります。インターフェースは実際に必要な情報だけを取るべきなので削りました。将来 executor を使う Form（例: 実行者名をログに残す）が出てきたら、そのときに引数を足せばよい話です。

**Q. `execute()` のチェック順序（署名 → grade）に意味は？**
A. 「そもそも署名されていない書類は grade に関係なく実行不能」という業務的な優先順位にしました。test 21 で grade 150 の bureaucrat が未署名フォームを実行しようとしたとき、`GradeTooLow` ではなく `FormNotSigned` が飛ぶことを検証しています。

**Q. `_signed` を派生から直接触れないのは不便では？**
A. PDF が「属性は private のまま、基底クラスに属する」と明示しているので、派生は `getSigned()` 経由でのみ参照します。今回は `execute()` が基底にあるので派生が `_signed` を見る必要すらありません。

**Q. Shrubbery の `action()` が失敗したら？**
A. `std::ofstream` が開けない場合（権限が無い等）は `std::cerr` にエラーを出して return します。例外にはしていません。

---

## 5. Ex03: Intern

### 評価項目: `makeForm()` / **Good dispatching**

評価シートの文言:

> The `makeForm()` function should use some kind of **array of pointers to member functions** to handle the creation of Forms.
> If it's using an unclean method, like if/elseif/elseif/else branchings, ... please count this as wrong.

### 実装（文言どおりメンバ関数ポインタ配列）

```cpp
// Intern.hpp
private:
    AForm* createShrubberyCreationForm(const std::string& target) const;
    AForm* createRobotomyRequestForm(const std::string& target) const;
    AForm* createPresidentialPardonForm(const std::string& target) const;

    typedef AForm* (Intern::*FormFactory)(const std::string& target) const;

    struct FormEntry {
        const char* name;
        FormFactory factory;
    };

    static const FormEntry   _forms[];
    static const std::size_t _formCount;
```

```cpp
// Intern.cpp:8-14
const Intern::FormEntry Intern::_forms[] = {
    {"shrubbery creation",  &Intern::createShrubberyCreationForm},
    {"robotomy request",    &Intern::createRobotomyRequestForm},
    {"presidential pardon", &Intern::createPresidentialPardonForm}
};

const std::size_t Intern::_formCount = sizeof(_forms) / sizeof(_forms[0]);
```

```cpp
// Intern.cpp:32-41
AForm* Intern::makeForm(const std::string& name, const std::string& target) const {
    for (std::size_t i = 0; i < _formCount; ++i) {
        if (name == _forms[i].name) {
            std::cout << "Intern creates " << name << std::endl;
            return (this->*_forms[i].factory)(target);   // ← メンバ関数ポインタ呼び出し
        }
    }
    std::cerr << "Intern: unknown form name \"" << name << "\"" << std::endl;
    return NULL;
}
```

### 説明できるようにしておく文法

- `typedef AForm* (Intern::*FormFactory)(const std::string&) const;`
  → 「`Intern` の const メンバ関数で、`const std::string&` を取り `AForm*` を返すもの」へのポインタ型
- `&Intern::createShrubberyCreationForm` — メンバ関数のアドレスは**必ず `&クラス名::` 付き**（関数名だけでは取れない）
- `(this->*ptr)(target)` — メンバ関数ポインタ呼び出しは `.*` / `->*` 演算子を使い、**括弧が必須**（`->*` より `()` の優先順位が高いため）
- `_formCount` は `sizeof(_forms) / sizeof(_forms[0])` で自動計算 → テーブルに 1 行足すだけで拡張できる

### PDF の例が動くこと

```
$ Intern someRandomIntern;
$ AForm* rrf = someRandomIntern.makeForm("robotomy request", "Bender");
Intern creates robotomy request
AForm "robotomy request" [signed=no, gradeToSign=72, gradeToExecute=45]
```

不明な名前は `std::cerr` にエラーを出して `NULL` を返す（PDF: "If the provided form name does not exist, print an explicit error message"）。

#### 想定問答

**Q. なぜ if/else じゃダメ？**
A. PDF が明示的に「excessive if/elseif/else structure は評価で受け入れられない」と書いています。設計上も、テーブル駆動なら Form 型の追加が「テーブルに 1 行」で済み、分岐の書き漏らしが起きません。

**Q. `NULL` を返す設計と例外を投げる設計、どっちが正しい？**
A. PDF は「explicit error message を表示せよ」としか言っていないので、どちらでも通ります。今回は「返り値で失敗を表現し、呼び出し側が `if (f)` で判定する」設計にしました。`delete NULL` は C++ 規格上安全な no-op なので、呼び出し側の後始末も壊れません（test 16 で検証）。

**Q. `makeForm` が返したポインタは誰が解放する？**
A. 呼び出し側です。`new` した生ポインタを返す以上、所有権は移譲されます。C++98 なので `unique_ptr` は使えません。`main.cpp` の全テストで対応する `delete` を書き、`leaks` で 0 リークを確認しています。

**Q. `Intern` は状態を持たないのに OCF が要る？**
A. PDF の「Module 02〜09 の全クラスは OCF」ルールが適用されます。例外クラスだけが免除で、`Intern` は免除対象ではありません。コピーコンストラクタと `operator=` は何もコピーしませんが、明示的に定義しています。

**Q. `_forms` を `static` にした理由は？**
A. テーブルは全 `Intern` インスタンスで共通の不変データだからです。インスタンスごとに持つ必要がなく、`static const` なら実行時に一度だけ初期化されます。

---

## 6. main.cpp のテストについて

評価シート冒頭に毎回出てくる:

> there has to be the main function that contains enough tests to prove the program works as expected. If there isn't, do not grade this exercise.

各 ex の `main.cpp` は自作の `expect()` ヘルパで PASS/FAIL を出し、最後にサマリを表示する形式:

```
=====================================
RESULT: 55 passed, 0 failed.
=====================================
```

| ex | テスト数 | 結果 |
|---|---|---|
| ex00 | 55 | 全 PASS |
| ex01 | 63 | 全 PASS |
| ex02 | 47 | 全 PASS |
| ex03 | 32 | 全 PASS |

カバーしている観点:
- 有効値の全域（ex00 は grade 1〜150 を全部試す）
- 境界値（`gradeToSign` ちょうど / +1、`INT_MIN` / `INT_MAX`）
- 例外の型（具体型で catch できるか、`std::exception&` でも catch できるか）
- 例外送出後の状態保存（失敗した increment で grade が変わっていないか）
- OCF 全て（コピーコンストラクタ、`operator=`、自己代入、連鎖代入 `a = b = c`、`operator=` が `*this` を返すか）
- `operator<<` の書式厳密一致・チェーン可能性・`ostream&` を返すか
- ポリモーフィズム（`AForm*` 配列経由の `execute`、virtual デストラクタ）
- 副作用の実測（Shrubbery のファイル生成内容、Robotomy の 1000 回試行で成功率が 35〜65% に収まるか）
- リーク耐性（10,000 回の失敗ループ、500 回の new/delete）

`StdoutCapture` クラスで `std::cout.rdbuf()` を差し替え、出力書式まで文字列比較している。

---

## 7. 評価当日のチェックリスト

```bash
# 1. クリーンビルド（警告 0 を見せる）
cd ex00 && make re && cd ..
cd ex01 && make re && cd ..
cd ex02 && make re && cd ..
cd ex03 && make re && cd ..

# 2. 実行（各 RESULT が 0 failed であること）
./ex00/bureaucrat | tail -3
./ex01/form       | tail -3
./ex02/form_actions | tail -3
./ex03/intern     | tail -3

# 3. 禁止事項の grep（評価者の前で叩くと説得力がある）
grep -rn "using namespace\|friend\|malloc\|calloc\|free(\|printf" --include='*.cpp' --include='*.hpp' .

# 4. リーク（Linux なら valgrind、Mac なら leaks）
valgrind --leak-check=full ./ex03/intern
leaks --atExit -- ./ex03/intern

# 5. 後始末（*_shrubbery が残るので）
cd ex02 && make fclean && rm -f *_shrubbery && cd ..
cd ex03 && make fclean && rm -f *_shrubbery && cd ..
```

### 提出前の注意
- `make fclean` を実行してから push する（`.o` と実行ファイル、`*_shrubbery` をコミットしない）
- ex02 / ex03 の実行後に `boundary_shrub_shrubbery` などのファイルが生成される。リポジトリに混ざっていないか `git status` で確認

---

## 8. 詰まりやすい概念のおさらい

### grade は「1 が最高、150 が最低」
直感と逆。`incrementGrade()` は**数字を減らす**（昇格）。PDF にも「incrementing a grade 3 should result in a grade 2」と念押しされている。評価シートにも同じ文言があるので、ここは必ず聞かれる。

### 例外クラスは OCF 免除
PDF ex00 の情報ボックスに明記:
> Please note that exception classes do not have to be designed in Orthodox Canonical Form. However, every other class must follow it.

だから `GradeTooHighException` に copy ctor / `operator=` / デストラクタが無くても減点されない。逆に `Bureaucrat` / `Form` / `AForm` / 3 つの具象 Form / `Intern` は全部 OCF 必須。

### `const` メンバと `operator=`
`const std::string _name;` があるクラスでは `operator=` で name をコピーできない。これは仕様（name は不変）なので正しい。評価でよく突っ込まれるポイントなので、`main.cpp` で明示的にテストしている。

### 参照で catch する理由
```cpp
catch (std::exception& e)   // ○
catch (std::exception  e)   // × スライシングが起きる
```
値で受けると派生クラスの部分が切り落とされ（object slicing）、`what()` が基底の実装を呼んでしまう。参照ならポリモーフィズムが効く。
