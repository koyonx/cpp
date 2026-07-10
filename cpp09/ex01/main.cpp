#include "RPN.hpp"
#include <iostream>
#include <sstream>
#include <string>

// ==== テストハーネス =========================================================

static int g_pass = 0;
static int g_fail = 0;

static void section(const std::string& t) {
	std::cout << "\n=== " << t << " ===" << std::endl;
}
static void expect(bool cond, const std::string& what) {
	if (cond) { ++g_pass; std::cout << "  [PASS] " << what << std::endl; }
	else      { ++g_fail; std::cout << "  [FAIL] " << what << std::endl; }
}

static int runTests() {
	// === 1. PDF 例そのまま ===
	section("1. PDF examples");
	{
		RPN rpn;
		expect(rpn.evaluate("8 9 * 9 - 9 - 9 - 4 - 1 +") == 42, "8 9 * 9 - 9 - 9 - 4 - 1 + = 42");
	}
	{
		RPN rpn;
		expect(rpn.evaluate("7 7 * 7 -") == 42, "7 7 * 7 - = 42");
	}
	{
		RPN rpn;
		expect(rpn.evaluate("1 2 * 2 / 2 * 2 4 - +") == 0, "= 0");
	}

	// === 2. PDF error case: "(1 + 1)" ===
	section("2. PDF error: parentheses");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("(1 + 1)"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "throws");
	}

	// === 3. 基本 4 演算 ===
	section("3. basic 4 operators");
	{
		RPN rpn;
		expect(rpn.evaluate("1 2 +") == 3, "1+2=3");
		expect(rpn.evaluate("5 3 -") == 2, "5-3=2");
		expect(rpn.evaluate("4 2 *") == 8, "4*2=8");
		expect(rpn.evaluate("8 2 /") == 4, "8/2=4");
	}

	// === 4. 単一トークン (1 数字) ===
	section("4. single-digit expressions");
	{
		RPN rpn;
		expect(rpn.evaluate("5") == 5, "single 5");
		expect(rpn.evaluate("0") == 0, "single 0");
		expect(rpn.evaluate("9") == 9, "single 9");
	}

	// === 5. 負の結果 ===
	section("5. negative intermediate results");
	{
		RPN rpn;
		expect(rpn.evaluate("3 5 -") == -2, "3-5=-2");
		expect(rpn.evaluate("0 5 -") == -5, "0-5=-5");
	}

	// === 6. 整数除算 (小数切り捨て) ===
	section("6. integer division truncation");
	{
		RPN rpn;
		expect(rpn.evaluate("7 2 /") == 3, "7/2=3 (truncated)");
		expect(rpn.evaluate("9 4 /") == 2, "9/4=2");
	}

	// === 7. 0 除算 → error ===
	section("7. division by zero");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("5 0 /"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "5/0 throws");
	}

	// === 8. 不足オペランド → error ===
	section("8. insufficient operands");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("+"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "'+' alone throws");
	}
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("5 +"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "'5 +' throws (needs 2)");
	}

	// === 9. 過剰オペランド → error (stack size != 1 at end) ===
	section("9. leftover operands");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("1 2 3"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "'1 2 3' throws (stack size 3)");
	}

	// === 10. 空文字列 → error ===
	section("10. empty expression");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate(""); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "empty throws");
	}

	// === 11. 空白のみ → error ===
	section("11. whitespace-only");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("   "); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "whitespace throws");
	}

	// === 12. 2桁以上の数字 → error (PDF: less than 10) ===
	section("12. multi-digit number");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("10 2 +"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "'10' rejected (multi-digit)");
	}

	// === 13. 無効な演算子 → error ===
	section("13. unsupported operators");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("5 3 %"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "'%' not supported");
	}
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("5 3 ^"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "'^' not supported");
	}

	// === 14. 文字が混じる → error ===
	section("14. non-numeric non-operator tokens");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("a b +"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "letters rejected");
	}

	// === 15. 長い正当式 ===
	section("15. long valid expression");
	{
		RPN rpn;
		// 5+3+2+1 = 11 as "5 3 + 2 + 1 +"
		expect(rpn.evaluate("5 3 + 2 + 1 +") == 11, "5+3+2+1=11");
	}

	// === 16. 中間結果が大きい (PDF: rule doesn't apply to intermediate) ===
	section("16. intermediate results can exceed 9");
	{
		RPN rpn;
		// 9 * 9 = 81, - 8 = 73
		expect(rpn.evaluate("9 9 * 8 -") == 73, "intermediate 81 OK");
	}

	// === 17. 負の中間結果からの追い計算 ===
	section("17. negative intermediate carries forward");
	{
		RPN rpn;
		// 3 - 5 = -2, -2 * 3 = -6
		expect(rpn.evaluate("3 5 - 3 *") == -6, "3-5=-2, *3=-6");
	}

	// === 18. evaluate は state を保持しない (呼び出しごとリセット) ===
	section("18. evaluate resets state");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("1 2 3"); }  // 失敗するが stack が汚れる可能性
		catch (const std::exception&) { caught = true; }
		expect(caught, "first eval throws");
		// 次の evaluate が clean state で動くか
		expect(rpn.evaluate("2 3 +") == 5, "second eval OK");
	}

	// === 19. OCF ===
	section("19. OCF");
	{
		RPN a;
		a.evaluate("2 3 +");
		RPN b(a);
		expect(b.evaluate("4 5 +") == 9, "copy-ctor'd RPN works");
		RPN c;
		c = a;
		expect(c.evaluate("6 7 +") == 13, "assigned RPN works");
		c = c;
		expect(c.evaluate("1 1 +") == 2, "self-assign safe");
	}

	// === 20. 10,000 evaluations (leak / stress) ===
	section("20. 10,000 evaluations stress");
	{
		RPN rpn;
		for (int i = 0; i < 10000; ++i) {
			int r = rpn.evaluate("8 9 * 9 - 9 - 9 - 4 - 1 +");
			if (r != 42) { expect(false, "iteration failed"); break; }
		}
		expect(true, "10000 evaluations OK");
	}

	// === 21. すべての演算子を含む長式 ===
	section("21. all operators in one expression");
	{
		RPN rpn;
		// ((5 + 3) * 2 - 4) / 2 = (8*2-4)/2 = 12/2 = 6
		expect(rpn.evaluate("5 3 + 2 * 4 - 2 /") == 6, "((5+3)*2-4)/2 = 6");
	}

	// === 22. 単項マイナスは非サポート (PDF: less than 10, positive) ===
	section("22. unary minus not supported");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("-5"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "-5 is not a single-digit token");
	}

	// === 23. 演算子連続 → error (stack が足りない) ===
	section("23. consecutive operators fail");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("+ +"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "'+ +' throws");
	}

	// === 24. 0 のオペランド (境界的) ===
	section("24. zero operand");
	{
		RPN rpn;
		expect(rpn.evaluate("5 0 +") == 5, "5+0=5");
		expect(rpn.evaluate("0 5 -") == -5, "0-5=-5");
		expect(rpn.evaluate("5 0 *") == 0, "5*0=0");
		expect(rpn.evaluate("0 5 /") == 0, "0/5=0");
	}

	// === 25. what() message ===
	section("25. exception message");
	{
		RPN rpn;
		try { rpn.evaluate("(1 + 1)"); }
		catch (const std::exception& e) {
			expect(std::string(e.what()) == "Error", "what() == 'Error'");
		}
	}

	// === 26. 演算子の非可換性を確認 (a - b ≠ b - a) ===
	section("26. non-commutative operators: order matters");
	{
		RPN rpn;
		expect(rpn.evaluate("7 3 -") == 4, "7-3=4");
		expect(rpn.evaluate("3 7 -") == -4, "3-7=-4");
		expect(rpn.evaluate("8 2 /") == 4, "8/2=4");
		expect(rpn.evaluate("2 8 /") == 0, "2/8=0 (integer)");
	}

	// === 27. 左結合性の RPN 表現 (a - b - c は (a-b)-c) ===
	section("27. left-associative subtraction (single-digit)");
	{
		RPN rpn;
		// (9 - 3) - 2 = 4
		expect(rpn.evaluate("9 3 - 2 -") == 4, "(9-3)-2=4");
		// 9 - (3 - 2) = 8 (RPN で「9 3 2 - -」)
		expect(rpn.evaluate("9 3 2 - -") == 8, "9-(3-2)=8");
	}

	// === 28. 単一ペア operation の全組み合わせ (a, b in 0..9) ===
	section("28. all (a, b) op combinations for + and *");
	{
		RPN rpn;
		bool all_ok = true;
		for (int a = 0; a < 10; ++a) {
			for (int b = 0; b < 10; ++b) {
				std::ostringstream expr; expr << a << " " << b << " +";
				if (rpn.evaluate(expr.str()) != a + b) { all_ok = false; break; }
				std::ostringstream expr2; expr2 << a << " " << b << " *";
				if (rpn.evaluate(expr2.str()) != a * b) { all_ok = false; break; }
			}
			if (!all_ok) break;
		}
		expect(all_ok, "100 + and * combinations all correct");
	}

	// === 29. 減算 全組合せ ===
	section("29. all subtractions (a-b for a,b in 0..9)");
	{
		RPN rpn;
		bool ok = true;
		for (int a = 0; a < 10; ++a) {
			for (int b = 0; b < 10; ++b) {
				std::ostringstream e; e << a << " " << b << " -";
				if (rpn.evaluate(e.str()) != a - b) { ok = false; break; }
			}
			if (!ok) break;
		}
		expect(ok, "100 subtractions all correct");
	}

	// === 30. 除算 (b != 0) 全組合せ ===
	section("30. all divisions with non-zero b");
	{
		RPN rpn;
		bool ok = true;
		for (int a = 0; a < 10; ++a) {
			for (int b = 1; b < 10; ++b) {
				std::ostringstream e; e << a << " " << b << " /";
				if (rpn.evaluate(e.str()) != a / b) { ok = false; break; }
			}
			if (!ok) break;
		}
		expect(ok, "90 divisions all correct");
	}

	// === 31. 深い stack 演算 (10 要素 +) ===
	section("31. deep stack: 9+9+9+9 = 36");
	{
		RPN rpn;
		expect(rpn.evaluate("9 9 + 9 + 9 +") == 36, "9+9+9+9=36");
	}

	// === 32. 空白の扱い: tab / 複数空白 ===
	section("32. whitespace variations tolerated by istringstream");
	{
		RPN rpn;
		expect(rpn.evaluate("2  3 +") == 5, "double space OK");
		expect(rpn.evaluate("2\t3 +") == 5, "tab OK");
		expect(rpn.evaluate("  2 3 +  ") == 5, "leading/trailing whitespace OK");
	}

	// === 33. 特定演算子の連続適用 (階乗風) ===
	section("33. staircase multiplications: 2*3*4=24");
	{
		RPN rpn;
		expect(rpn.evaluate("2 3 * 4 *") == 24, "2*3*4=24");
	}

	// === 34. PDF 例をトークン単位で分解して検証 ===
	section("34. PDF example partial evaluations");
	{
		RPN rpn;
		// 8 9 * = 72
		expect(rpn.evaluate("8 9 *") == 72, "8*9=72");
		// 8 9 * 9 - = 63
		expect(rpn.evaluate("8 9 * 9 -") == 63, "72-9=63");
		// PDF full: 8 9 * 9 - 9 - 9 - 4 - 1 + = 42
		expect(rpn.evaluate("8 9 * 9 - 9 - 9 - 4 - 1 +") == 42, "PDF result 42");
	}

	// === 35. 中間結果が INT の範囲を大きく使う ===
	section("35. large intermediate results");
	{
		RPN rpn;
		// 9 * 9 * 9 * 9 = 6561
		expect(rpn.evaluate("9 9 * 9 * 9 *") == 6561, "9^4 = 6561");
		// 9 * 9 * 9 * 9 * 9 = 59049
		expect(rpn.evaluate("9 9 * 9 * 9 * 9 *") == 59049, "9^5 = 59049");
	}

	// === 36. Multiple RPN objects independent ===
	section("36. multiple RPN objects don't interfere");
	{
		RPN a, b, c;
		expect(a.evaluate("1 2 +") == 3, "a: 3");
		expect(b.evaluate("4 5 +") == 9, "b: 9");
		expect(c.evaluate("6 7 +") == 13, "c: 13");
		expect(a.evaluate("2 3 +") == 5, "a again: 5");
	}

	// === 37. 1桁数字だけの token (10 は multi-digit で reject) ===
	section("37. multi-digit vs single-digit tokens");
	{
		RPN rpn;
		expect(rpn.evaluate("9 9 +") == 18, "9+9=18 single-digit OK");
		bool caught = false;
		try { rpn.evaluate("99 1 +"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "99 rejected");
	}

	// === 38. 特殊 token: '.' / '_' なども reject ===
	section("38. special tokens rejected");
	{
		RPN rpn;
		bool ok = true;
		const char* bad[] = { "1 . +", "_ 1 +", "1 ! 2", "1 = 2", "1 2 &" };
		for (int i = 0; i < 5; ++i) {
			try { rpn.evaluate(bad[i]); ok = false; break; }
			catch (const std::exception&) {}
		}
		expect(ok, "5 special-token expressions all throw");
	}

	// === 39. 単一演算子のみの token (`+` alone) ===
	section("39. single-operator-only rejected");
	{
		RPN rpn;
		bool caught = false;
		try { rpn.evaluate("*"); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "single '*' throws");
	}

	// === 40. 過去の PDF 例エラー: 括弧 ===
	section("40. parentheses rejected");
	{
		RPN rpn;
		bool ok = true;
		const char* bad[] = { "(1)", "(1 + 1)", "1 + (1)", "((", "))" };
		for (int i = 0; i < 5; ++i) {
			try { rpn.evaluate(bad[i]); ok = false; break; }
			catch (const std::exception&) {}
		}
		expect(ok, "5 parenthesis expressions all throw");
	}

	// === 41. Long expression の stress test ===
	section("41. deeply nested expression: (((1+1)+1)+1)...");
	{
		RPN rpn;
		// "1" 初期、続けて " 1 +" を 16 回 → 1 + 16 = 17
		std::ostringstream oss;
		oss << "1";
		for (int i = 0; i < 16; ++i) oss << " 1 +";
		expect(rpn.evaluate(oss.str()) == 17, "1 + 16 x 1 = 17");
	}

	// === 42. 0除算のバリエーション ===
	section("42. division by zero variations");
	{
		RPN rpn;
		bool ok = true;
		const char* bad[] = { "0 0 /", "5 0 /", "9 0 /" };
		for (int i = 0; i < 3; ++i) {
			try { rpn.evaluate(bad[i]); ok = false; break; }
			catch (const std::exception&) {}
		}
		expect(ok, "3 div-by-zero variants throw");
	}

	// === 43. 順序保持と非破壊性 ===
	section("43. successful evaluate leaves state clean");
	{
		RPN rpn;
		rpn.evaluate("1 2 +");
		// evaluate 内で stack をクリアしているので次の evaluate に影響なし
		expect(rpn.evaluate("3 4 +") == 7, "second eval clean");
	}

	// === 44. 負中間結果からの正確な計算 ===
	section("44. negative intermediates carry correctly");
	{
		RPN rpn;
		// (1-9) * (1-9) = -8 * -8 = 64
		expect(rpn.evaluate("1 9 - 1 9 - *") == 64, "(-8)*(-8)=64");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}

// ==== main ==================================================================

int main(int argc, char** argv) {
	if (argc == 2 && std::string(argv[1]) == "--test") {
		return runTests();
	}
	if (argc != 2) {
		std::cerr << "Error" << std::endl;
		return 1;
	}
	try {
		RPN rpn;
		std::cout << rpn.evaluate(argv[1]) << std::endl;
	} catch (const std::exception&) {
		std::cerr << "Error" << std::endl;
		return 1;
	}
	return 0;
}
