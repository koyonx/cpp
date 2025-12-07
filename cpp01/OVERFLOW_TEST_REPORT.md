# オーバーフロー脆弱性テストレポート

## テスト日時
2025-12-04

## 対象プロジェクト
/Users/kkz/Downloads/cpp/cpp01

---

## 発見された重大な脆弱性

### 1. メモリオーバーフロー（ex01/zombieHorde.cpp）

**深刻度**: 🔴 CRITICAL

**場所**: `ex01/zombieHorde.cpp:7`

**脆弱なコード**:
```cpp
Zombie* zombieHorde( int N, std::string name ) {
    if (N <= 0) {
        return NULL;
    }
    Zombie* horde = new Zombie[N];  // ← 脆弱性
    for (int i = 0; i < N; ++i) {
        horde[i].setName(name);
    }
    return horde;
}
```

**問題点**:
1. **上限チェックなし**: N の最大値チェックが存在しない
2. **メモリ枯渇**: 大きなN値（例：INT_MAX = 2,147,483,647）でシステムメモリを枯渇させる
3. **例外処理なし**: std::bad_alloc 例外を処理していない
4. **DoS攻撃の可能性**: 悪意のあるユーザーがシステムをクラッシュさせることが可能

**テスト結果**:
- ✓ N = -10: 正常にNULLを返す
- ✓ N = 0: 正常にNULLを返す
- ✓ N = 5: 正常に動作
- ✓ N = 10,000: 正常に動作
- ⚠️ N = 1,000,000: システムによっては動作（メモリ依存）
- ❌ N = INT_MAX: **プロセスが強制終了（exit code 137 / SIGKILL）**

**影響**:
- システムメモリの完全な枯渇
- プロセスのクラッシュ
- サービス拒否（DoS）攻撃の可能性

---

## 推奨される修正方法

### 修正案1: 上限チェックの追加

```cpp
#define MAX_ZOMBIE_HORDE 100000  // 合理的な最大値

Zombie* zombieHorde( int N, std::string name ) {
    if (N <= 0 || N > MAX_ZOMBIE_HORDE) {
        return NULL;
    }
    Zombie* horde = new Zombie[N];
    for (int i = 0; i < N; ++i) {
        horde[i].setName(name);
    }
    return horde;
}
```

### 修正案2: 例外処理の追加（C++98互換）

```cpp
Zombie* zombieHorde( int N, std::string name ) {
    if (N <= 0) {
        return NULL;
    }

    try {
        Zombie* horde = new Zombie[N];
        for (int i = 0; i < N; ++i) {
            horde[i].setName(name);
        }
        return horde;
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        return NULL;
    }
}
```

### 修正案3: 両方を組み合わせ（推奨）

```cpp
#define MAX_ZOMBIE_HORDE 100000

Zombie* zombieHorde( int N, std::string name ) {
    if (N <= 0 || N > MAX_ZOMBIE_HORDE) {
        return NULL;
    }

    try {
        Zombie* horde = new Zombie[N];
        for (int i = 0; i < N; ++i) {
            horde[i].setName(name);
        }
        return horde;
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        return NULL;
    }
}
```

---

## その他のチェック項目

### ex04/sed.cpp
**状態**: ⚠️ 潜在的な問題あり

**潜在的な問題**:
1. **大きなファイル**: 非常に大きなファイルを一度にメモリに読み込む（sed.cpp:11-14）
2. **無限ループの可能性**: replace()関数で s1 が s2 に含まれる場合

**現在の実装**:
```cpp
while ((pos = fileContent.find(s1, pos)) != std::string::npos)
{
    fileContent.erase(pos, s1.length());
    fileContent.insert(pos, s2);
    pos += s2.length();  // ← これで無限ループを回避
}
```

**推奨**:
- ファイルサイズの上限チェックを追加
- ストリームベースの処理を検討（メモリ効率向上）

---

## テストファイル

以下のテストファイルを作成しました：

1. `test_overflow.cpp` - 包括的テスト（INT_MAXテストを含む）
2. `test_overflow_safe.cpp` - 安全版テスト（INT_MAXテストをスキップ）

### テスト実行方法

```bash
# 安全版テストの実行（推奨）
c++ -Wall -Wextra -Werror -std=c++98 -pedantic \
    -o test_overflow_safe test_overflow_safe.cpp \
    ex01/Zombie.cpp ex01/zombieHorde.cpp -I.
./test_overflow_safe

# 個別テスト実行
./test_overflow_safe basic      # 基本テストのみ
./test_overflow_safe large      # 大規模割り当てテスト
./test_overflow_safe overflow   # オーバーフロー分析
```

---

## まとめ

**重要度: 高**

`zombieHorde` 関数には深刻なメモリオーバーフロー脆弱性が存在します。
悪意のあるユーザーまたはバグのあるコードが大きな N 値を渡すと、
システム全体がクラッシュする可能性があります。

**即座に修正が必要です。**

修正案3（上限チェック + 例外処理）の実装を強く推奨します。
