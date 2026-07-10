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
