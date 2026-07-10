#include "PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>
#include <algorithm>
#include <cstdlib>
#include <ctime>
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

// sort 結果が正しく昇順か
static bool isSorted(const std::vector<int>& v) {
	for (std::size_t i = 1; i < v.size(); ++i)
		if (v[i] < v[i-1]) return false;
	return true;
}
static bool isSorted(const std::deque<int>& d) {
	for (std::size_t i = 1; i < d.size(); ++i)
		if (d[i] < d[i-1]) return false;
	return true;
}

// std::sort と同じ結果を返すか (multiset 相当)
static bool sameAsStdSort(std::vector<int> v) {
	std::vector<int> sorted(v);
	std::sort(sorted.begin(), sorted.end());
	std::vector<int> mine(v);
	PmergeMe::sortVector(mine);
	return mine == sorted;
}

static int runTests() {
	std::srand(42);

	// === 1. 空 vector ===
	section("1. sort empty vector");
	{
		std::vector<int> v;
		PmergeMe::sortVector(v);
		expect(v.empty(), "empty stays empty");
	}

	// === 2. 単一要素 ===
	section("2. single element");
	{
		std::vector<int> v; v.push_back(42);
		PmergeMe::sortVector(v);
		expect(v.size() == 1 && v[0] == 42, "single unchanged");
	}

	// === 3. 2要素 ===
	section("3. two elements");
	{
		std::vector<int> v; v.push_back(2); v.push_back(1);
		PmergeMe::sortVector(v);
		expect(v[0] == 1 && v[1] == 2, "2 elements sorted");
	}

	// === 4. PDF 例: 3 5 9 7 4 → 3 4 5 7 9 ===
	section("4. PDF example: 3 5 9 7 4");
	{
		std::vector<int> v;
		v.push_back(3); v.push_back(5); v.push_back(9); v.push_back(7); v.push_back(4);
		PmergeMe::sortVector(v);
		expect(v.size() == 5 && v[0]==3 && v[1]==4 && v[2]==5 && v[3]==7 && v[4]==9,
		       "3 4 5 7 9");
	}

	// === 5. 既ソート ===
	section("5. already sorted input");
	{
		std::vector<int> v;
		for (int i = 1; i <= 10; ++i) v.push_back(i);
		PmergeMe::sortVector(v);
		expect(isSorted(v) && v[0] == 1 && v[9] == 10, "already sorted preserved");
	}

	// === 6. 逆順 ===
	section("6. reverse-sorted input");
	{
		std::vector<int> v;
		for (int i = 10; i >= 1; --i) v.push_back(i);
		PmergeMe::sortVector(v);
		expect(isSorted(v) && v[0] == 1 && v[9] == 10, "reversed to sorted");
	}

	// === 7. 全同じ値 ===
	section("7. all-equal values");
	{
		std::vector<int> v(10, 7);
		PmergeMe::sortVector(v);
		bool all_seven = true;
		for (std::size_t i = 0; i < 10; ++i) if (v[i] != 7) { all_seven = false; break; }
		expect(all_seven && isSorted(v), "10 sevens preserved");
	}

	// === 8. duplicates 混在 ===
	section("8. mixed duplicates");
	{
		std::vector<int> v;
		int arr[] = {5, 3, 5, 1, 3, 5, 2, 1};
		for (std::size_t i = 0; i < 8; ++i) v.push_back(arr[i]);
		PmergeMe::sortVector(v);
		expect(isSorted(v), "sorted");
		expect(v.size() == 8, "size preserved");
		expect(v[0] == 1 && v[7] == 5, "min/max correct");
	}

	// === 9. 奇数個 (straggler test) ===
	section("9. odd count (straggler)");
	{
		for (int n = 1; n <= 11; n += 2) {
			std::vector<int> v;
			for (int i = 0; i < n; ++i) v.push_back(std::rand() % 100);
			std::vector<int> expected(v);
			std::sort(expected.begin(), expected.end());
			PmergeMe::sortVector(v);
			if (v != expected) {
				std::ostringstream m; m << "odd n=" << n << " mismatch";
				expect(false, m.str());
				break;
			}
		}
		expect(true, "1,3,5,7,9,11 elements all sort correctly");
	}

	// === 10. 偶数個 ===
	section("10. even count");
	{
		for (int n = 2; n <= 12; n += 2) {
			std::vector<int> v;
			for (int i = 0; i < n; ++i) v.push_back(std::rand() % 100);
			std::vector<int> expected(v);
			std::sort(expected.begin(), expected.end());
			PmergeMe::sortVector(v);
			if (v != expected) {
				std::ostringstream m; m << "even n=" << n << " mismatch";
				expect(false, m.str());
				break;
			}
		}
		expect(true, "even 2..12 sort correctly");
	}

	// === 11. deque でも同じ結果 ===
	section("11. deque sort works");
	{
		std::deque<int> d;
		int arr[] = {3, 5, 9, 7, 4};
		for (std::size_t i = 0; i < 5; ++i) d.push_back(arr[i]);
		PmergeMe::sortDeque(d);
		expect(isSorted(d) && d[0]==3 && d[4]==9, "deque PDF example");
	}

	// === 12. vector と deque が同じ結果 ===
	section("12. vector and deque produce identical output");
	{
		std::vector<int> v;
		std::deque<int> d;
		for (int i = 0; i < 100; ++i) {
			int r = std::rand() % 1000;
			v.push_back(r);
			d.push_back(r);
		}
		PmergeMe::sortVector(v);
		PmergeMe::sortDeque(d);
		bool eq = (v.size() == d.size());
		for (std::size_t i = 0; eq && i < v.size(); ++i)
			if (v[i] != d[i]) eq = false;
		expect(eq, "100-random vector == deque output");
	}

	// === 13. std::sort との一致 (25 要素) ===
	section("13. matches std::sort output (25 random)");
	{
		std::vector<int> v;
		for (int i = 0; i < 25; ++i) v.push_back(std::rand() % 1000);
		expect(sameAsStdSort(v), "matches std::sort");
	}

	// === 14. 100 要素、10 回反復 ===
	section("14. 100 elements x 10 iterations, all match std::sort");
	{
		bool all_ok = true;
		for (int iter = 0; iter < 10; ++iter) {
			std::vector<int> v;
			for (int i = 0; i < 100; ++i) v.push_back(std::rand() % 10000);
			if (!sameAsStdSort(v)) { all_ok = false; break; }
		}
		expect(all_ok, "10 iterations all match");
	}

	// === 15. 3000 要素 (PDF: 3000+ 対応必須) ===
	section("15. 3000 elements (PDF requirement)");
	{
		std::vector<int> v;
		for (int i = 0; i < 3000; ++i) v.push_back(std::rand() % 100000);
		std::vector<int> expected(v);
		std::sort(expected.begin(), expected.end());
		std::vector<int> mine(v);
		PmergeMe::sortVector(mine);
		expect(mine == expected, "3000 elements sorted correctly");
	}

	// === 16. 3000 要素 deque ===
	section("16. 3000 elements deque");
	{
		std::deque<int> d;
		for (int i = 0; i < 3000; ++i) d.push_back(std::rand() % 100000);
		std::deque<int> expected(d);
		std::sort(expected.begin(), expected.end());
		PmergeMe::sortDeque(d);
		expect(d == expected, "3000 elements deque sorted");
	}

	// === 17. 10,000 要素 (extreme) ===
	section("17. 10,000 elements extreme");
	{
		std::vector<int> v;
		for (int i = 0; i < 10000; ++i) v.push_back(std::rand());
		std::vector<int> expected(v);
		std::sort(expected.begin(), expected.end());
		PmergeMe::sortVector(v);
		expect(v == expected, "10,000 elements match std::sort");
	}

	// === 18. parseInput: 有効入力 ===
	section("18. parseInput: valid args");
	{
		char* argv[] = { (char*)"prog", (char*)"3", (char*)"5", (char*)"9", (char*)"7", (char*)"4" };
		PmergeMe p;
		p.parseInput(6, argv);
		expect(p.vec().size() == 5 && p.deq().size() == 5, "5 elements loaded");
		expect(p.vec()[0] == 3 && p.vec()[4] == 4, "vec contents");
	}

	// === 19. parseInput: 負の値 → error ===
	section("19. parseInput: negative rejected");
	{
		char* argv[] = { (char*)"prog", (char*)"-1", (char*)"2" };
		PmergeMe p;
		bool caught = false;
		try { p.parseInput(3, argv); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "negative throws");
	}

	// === 20. parseInput: 非数値 → error ===
	section("20. parseInput: non-numeric rejected");
	{
		char* argv[] = { (char*)"prog", (char*)"abc" };
		PmergeMe p;
		bool caught = false;
		try { p.parseInput(2, argv); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "letters throw");
	}

	// === 21. parseInput: 空文字 ===
	section("21. parseInput: empty string rejected");
	{
		char* argv[] = { (char*)"prog", (char*)"" };
		PmergeMe p;
		bool caught = false;
		try { p.parseInput(2, argv); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "empty throws");
	}

	// === 22. parseInput: INT_MAX 超え → error ===
	section("22. parseInput: overflow rejected");
	{
		char* argv[] = { (char*)"prog", (char*)"99999999999" };
		PmergeMe p;
		bool caught = false;
		try { p.parseInput(2, argv); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "overflow throws");
	}

	// === 23. parseInput: 引数なし ===
	section("23. parseInput: no args");
	{
		char* argv[] = { (char*)"prog" };
		PmergeMe p;
		bool caught = false;
		try { p.parseInput(1, argv); }
		catch (const std::exception&) { caught = true; }
		expect(caught, "no args throws");
	}

	// === 24. parseInput: 0 は valid (positive integer includes 0) ===
	section("24. parseInput: 0 is valid");
	{
		char* argv[] = { (char*)"prog", (char*)"0", (char*)"1" };
		PmergeMe p;
		p.parseInput(3, argv);
		expect(p.vec().size() == 2 && p.vec()[0] == 0, "0 accepted");
	}

	// === 25. OCF ===
	section("25. OCF");
	{
		char* argv[] = { (char*)"prog", (char*)"1", (char*)"2" };
		PmergeMe a;
		a.parseInput(3, argv);
		PmergeMe b(a);
		expect(b.vec() == a.vec(), "copy ctor");
		PmergeMe c;
		c = a;
		expect(c.vec() == a.vec(), "operator=");
		c = c;
		expect(c.vec().size() == 2, "self-assign safe");
	}

	// === 26. sort 冪等性 (2回ソートしても結果同じ) ===
	section("26. sorting an already-sorted array is idempotent");
	{
		std::vector<int> v;
		for (int i = 0; i < 20; ++i) v.push_back(std::rand() % 50);
		PmergeMe::sortVector(v);
		std::vector<int> once(v);
		PmergeMe::sortVector(v);
		expect(v == once, "second sort no-op");
	}

	// === 27. 大量 duplicate ===
	section("27. many duplicates (100 elements only 3 distinct)");
	{
		std::vector<int> v;
		for (int i = 0; i < 100; ++i) v.push_back(std::rand() % 3);
		std::vector<int> expected(v);
		std::sort(expected.begin(), expected.end());
		PmergeMe::sortVector(v);
		expect(v == expected, "duplicates preserved and sorted");
	}

	// === 28. INT_MAX / 0 混在 ===
	section("28. boundary values (0 and INT_MAX)");
	{
		std::vector<int> v;
		v.push_back(2147483647); v.push_back(0); v.push_back(1);
		v.push_back(2147483646); v.push_back(2147483647);
		PmergeMe::sortVector(v);
		expect(isSorted(v) && v[0] == 0 && v[4] == 2147483647, "INT_MAX handled");
	}

	// === 29. n=15 くらいの Ford-Johnson optimal 域 ===
	section("29. various sizes 1..20 all produce sorted output");
	{
		bool all_ok = true;
		for (int n = 1; n <= 20; ++n) {
			std::vector<int> v;
			for (int i = 0; i < n; ++i) v.push_back(std::rand() % 1000);
			std::vector<int> expected(v);
			std::sort(expected.begin(), expected.end());
			PmergeMe::sortVector(v);
			if (v != expected) { all_ok = false; break; }
		}
		expect(all_ok, "n=1..20 all correct");
	}

	// === 30. sortAndReport: PDF format ===
	section("30. sortAndReport output format");
	{
		char* argv[] = { (char*)"prog", (char*)"3", (char*)"5", (char*)"9", (char*)"7", (char*)"4" };
		PmergeMe p;
		p.parseInput(6, argv);
		std::ostringstream oss;
		std::streambuf* saved = std::cout.rdbuf(oss.rdbuf());
		p.sortAndReport();
		std::cout.rdbuf(saved);
		std::string out = oss.str();
		expect(out.find("Before:") != std::string::npos, "'Before:' present");
		expect(out.find("After:") != std::string::npos, "'After:' present");
		expect(out.find("Time to process") != std::string::npos, "'Time to process' present");
		expect(out.find("std::vector") != std::string::npos, "std::vector named");
		expect(out.find("std::deque") != std::string::npos, "std::deque named");
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
	try {
		PmergeMe p;
		p.parseInput(argc, argv);
		p.sortAndReport();
	} catch (const std::exception&) {
		std::cerr << "Error" << std::endl;
		return 1;
	}
	return 0;
}
