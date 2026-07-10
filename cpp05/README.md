# C++ Module 05 — Repetition and Exceptions

## このモジュールで学ぶこと

- **例外 (exception)** の投げ方・受け方
- **ネストクラス (nested class)** の書き方（`Bureaucrat::GradeTooHighException` みたいなやつ）
- **抽象クラス (abstract class)** と純粋仮想関数 `= 0`
- **仮想デストラクタ (virtual destructor)** の必要性
- **テンプレートメソッドパターン**（親クラスが枠を用意し、子クラスが穴を埋める設計）
- **ファクトリパターン**（文字列から適切な型のオブジェクトを作る）

C++98 縛りなので、`std::unique_ptr` も `nullptr` も使えません。`NULL` と生ポインタ、`try/catch`、手動 `delete` の世界です。

---

## そもそも用語おさらい

### `try` / `catch` / `throw`
```cpp
try {
    Bureaucrat b("Alice", 999); // 無効な grade
} catch (std::exception& e) {
    std::cout << e.what() << std::endl; // 例外の内容を取り出す
}
```
- `throw XxxException();` で例外を投げる
- `catch (std::exception& e)` は「`std::exception` を継承した全ての例外」を捕まえる
- `e.what()` は「何が起きたか」を文字列で返す仮想関数

### OCF (Orthodox Canonical Form)
C++ クラスに必ず 4 つ書けというルール:
1. デフォルトコンストラクタ `Foo()`
2. コピーコンストラクタ `Foo(const Foo&)`
3. コピー代入演算子 `Foo& operator=(const Foo&)`
4. デストラクタ `~Foo()`

**例外クラスは OCF 免除**（PDF に明記）。ただしそれ以外はモジュール 02〜09 で必須。

### `const` メンバ変数と `operator=`
```cpp
class Bureaucrat {
    const std::string _name; // ← const だから代入できない
    int               _grade;
};
```
- コピー代入では `_grade` しかコピーできません。名前は不変。
- これは 42 のレビューでよく突っ込まれるので `main.cpp` でこの挙動を明示的にテストしています。

### 仮想デストラクタ
```cpp
AForm* f = new ShrubberyCreationForm("garden");
delete f; // これで ShrubberyCreationForm::~ShrubberyCreationForm() が呼ばれるか？
```
- 親クラスのデストラクタが `virtual` でないと、子のデストラクタが呼ばれず**リーク**します
- `AForm.hpp` で `virtual ~AForm();` としているのはこのため

---

## 各 ex 詳細

### ex00 — Bureaucrat

**目的:** 例外クラスの基本

**キーポイント:**
- `Bureaucrat::GradeTooHighException` / `GradeTooLowException` は **ネストクラス**
- `what()` の宣言は `virtual const char* what() const throw();`
    - `throw()` は「この関数は例外を投げない」宣言（C++98 の書き方）
    - C++11 以降は `noexcept` になる
- grade は **1 が最高**、150 が最低（直感と逆！）。`incrementGrade()` は数字を「減らす」

**落とし穴:**
- `_grade - 1 < 1` を境界チェック。うっかり `_grade < 1` と書くと、grade 1 で increment→0 になっても throw しない
- コンストラクタで throw する場合、既に初期化子リストで初期化された `_name` はどうなる？ → **問題なし**。C++ は例外送出時、既に構築完了したメンバをデストラクタで巻き戻します（=リークしない）

### ex01 — Form + signForm

**目的:** クラス同士の相互参照（循環インクルード）を回避

**キーポイント:**
- `Bureaucrat.hpp` は `class Form;`（前方宣言）
- `Bureaucrat.cpp` で初めて `#include "Form.hpp"`
- `Form::beSigned(const Bureaucrat& b)` は grade チェックだけ。エラー時は `throw Form::GradeTooLowException()`
- `Bureaucrat::signForm(Form& form)` が `beSigned` を呼び、例外を捕まえて自分で「成功／失敗のログ」を出す

**落とし穴:**
- 「一度署名した Form に再度署名しても例外にしない」＝**冪等 (idempotent)** な設計を選びました（テスト [10]）。仕様は明記されていないので設計判断

### ex02 — AForm 抽象クラス + concrete forms

**目的:** 継承と抽象クラス、テンプレートメソッドパターン

**設計:**
```cpp
class AForm {
public:
    void execute(const Bureaucrat& executor) const; // ← 共通の枠
protected:
    virtual void action(const Bureaucrat& executor) const = 0; // ← 子が埋める穴
};

void AForm::execute(const Bureaucrat& executor) const {
    if (!_signed)                        throw FormNotSignedException();
    if (executor.getGrade() > _gradeToExecute) throw GradeTooLowException();
    action(executor); // ← 子クラス固有の処理へ委譲
}
```
これが **テンプレートメソッドパターン**。共通の前処理（署名チェック・grade チェック）を親に書き、後は子に任せる。PDF の「one way is more elegant」はこれのことです。

**具体クラス:**
| クラス | sign / exec | 何をする？ |
|---|---|---|
| `ShrubberyCreationForm` | 145 / 137 | `<target>_shrubbery` に ASCII の木を書き込む |
| `RobotomyRequestForm` | 72 / 45 | ドリル音を出して 50% で成功／失敗 (`rand() % 2`) |
| `PresidentialPardonForm` | 25 / 5 | `<target> has been pardoned by Zaphod Beeblebrox.` |

**落とし穴:**
- `AForm` は抽象クラスなので **インスタンス化できない**（`AForm a;` はコンパイルエラー）
- 抽象化するには最低 1 つ純粋仮想関数が必要 → `action() = 0`
- `AForm* f = new ChildForm(...); delete f;` を安全にするには **`virtual ~AForm()`** が必須
- 子の代入演算子は `AForm::operator=(other); _target = other._target;` の順で親から呼ぶ
- `rand()` を使うので `main.cpp` で `srand(time(NULL))` を 1 回だけ実行

### ex03 — Intern（ファクトリ）

**目的:** if/else 連鎖を書かずに文字列→型を分岐

**設計 (関数ポインタテーブル):**
```cpp
namespace {
    template <typename T>
    AForm* create(const std::string& target) { return new T(target); }

    struct FormEntry {
        const char* name;
        AForm*      (*factory)(const std::string&);
    };

    const FormEntry kFormTable[] = {
        {"shrubbery creation",  &create<ShrubberyCreationForm>},
        {"robotomy request",    &create<RobotomyRequestForm>},
        {"presidential pardon", &create<PresidentialPardonForm>}
    };
}
```
- `create<T>` はテンプレート関数。`T` に型を入れると「その型を new して返す関数」が出来上がる
- テーブルを線形検索するだけ。新しい Form 型を追加する時はテーブルに 1 行足すだけ

**なぜ if/else じゃダメ？**
- PDF が「excessive if/elseif/else structure」を明示禁止。「evaluation で受け入れない」と書いてある
- 保守性・拡張性の観点でもテーブル駆動の方が綺麗

**落とし穴:**
- 匿名 namespace は OK。`using namespace std;` は禁止（それとは別物）
- 見つからなかった時は `NULL` を返し、`std::cerr` にエラー
- 返された `AForm*` の所有権は呼び出し側。**必ず `delete` すること**

---

## コンパイル・実行

各 ex ディレクトリで:
```bash
make          # ビルド
make clean    # .o だけ消す
make fclean   # .o と実行ファイル両方消す
make re       # fclean してから all
```

コンパイルフラグ:
```
-Wall -Wextra -Werror -std=c++98 -pedantic
```

## テストとリーク検証

各 ex の `main.cpp` に 9〜15 パターンのテストを埋め込んであります。以下で確認:

```bash
./bureaucrat        # ex00
./form              # ex01
./form_actions      # ex02  (実行後、*_shrubbery ファイル生成)
./intern            # ex03
```

**valgrind でリーク確認:**
```bash
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./bureaucrat
```
全 ex で `0 errors, 0 leaks` を確認済み。

---

## レビュー時に聞かれそうなこと

- **なぜ `throw()` 指定を付ける？** → C++98 の例外仕様（No-throw 保証）。`what()` はメッセージを返すだけなので絶対に例外を投げない、と宣言している
- **`std::exception&` で catch する意味は？** → ポリモーフィズムを使い、`Bureaucrat` と `Form` と `AForm` の全 8 種の例外を 1 箇所で処理できる。参照 (`&`) で受けるのはコピーを避けるため（`std::exception` はコピーコストが低いが、派生クラスの情報が失われる「スライシング」を避ける意味もある）
- **なぜ `AForm` を abstract に？** → base の `AForm` 単体では意味を成さない（実際に何をするか未定義）。純粋仮想関数 `action() = 0` を置くことで「必ず子が実装すべき」を強制
- **テンプレートメソッドパターンの利点は？** → 事前チェック（署名済み・grade 十分）を親クラスで一元管理。子クラスは業務ロジックだけに集中でき、DRY 原則を守れる
- **`Intern` はなぜ関数ポインタテーブル？** → 拡張時に「1 行足す」だけで済む。if/else 連鎖だと分岐が増えるほど読みにくく、条件漏れも起きる

## 参考リンク

- [cppreference: try-catch](https://en.cppreference.com/w/cpp/language/try_catch)
- [cppreference: pure virtual functions](https://en.cppreference.com/w/cpp/language/abstract_class)
- [cppreference: std::exception](https://en.cppreference.com/w/cpp/error/exception)
- テンプレートメソッドパターン: GoF デザインパターン本, もしくは https://en.wikipedia.org/wiki/Template_method_pattern
