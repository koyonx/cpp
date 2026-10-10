#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <string>
#include <vector>
#include <deque>
#include <exception>
#include <cstddef>

// PmergeMe: Ford-Johnson (merge-insertion) sort を 2 種類の container で実装
// PDF 制約: ex00 (std::map) と ex01 (std::stack) で使ったコンテナは使えない
//          -> 本 ex では std::vector と std::deque を使用
//
// Ford-Johnson (merge-insertion) sort とは:
//  1. 要素をペアにし、各ペア内で大きい方を a、小さい方を b とする (b_i < a_i)
//  2. a だけを取り出し、再帰的にソート (main chain 完成)
//  3. a_1 とペアだった b_1 は main chain の全要素より小さい -> 比較なしで先頭に置く
//  4. 残りの b を Jacobsthal 番号順 (b3,b2, b5,b4, b11..b6, ...) で binary insertion
//     b_i の探索範囲は「a_i より前」だけ (b_i < a_i が既知なので)
//     Jacobsthal 順にすると探索範囲が常に 2^k - 1 個以下になり、比較 k 回で済む
//  -> n 要素の最悪比較回数 = Σ_{k=1..n} ⌈log2(3k/4)⌉ (n <= 11 では比較回数最適)
//
// 実装メモ: 再帰の後に「どの a とどの b がペアだったか」を値で探すと
// 重複値で曖昧になる上に余計な比較が発生するので、要素は元配列の添字で持ち回る。
class PmergeMe {
private:
	std::vector<int>	_vecData;
	std::deque<int>		_deqData;

	// 比較回数カウンタ (要素同士の比較はすべて isLess を通す)
	static std::size_t	_comparisons;
	static bool			isLess(int a, int b);

	// Vector 用実装 (idx: values の添字列を values の昇順に並べ替える)
	static void			fordJohnsonVector(std::vector<int>& v);
	static void			mergeInsertVector(const std::vector<int>& values,
							std::vector<std::size_t>& idx);
	static std::size_t	binaryInsertVector(const std::vector<int>& values,
							std::vector<std::size_t>& S, std::size_t x, std::size_t bound);

	// Deque 用実装
	static void			fordJohnsonDeque(std::deque<int>& d);
	static void			mergeInsertDeque(const std::deque<int>& values,
							std::deque<std::size_t>& idx);
	static std::size_t	binaryInsertDeque(const std::deque<int>& values,
							std::deque<std::size_t>& S, std::size_t x, std::size_t bound);

public:
	PmergeMe();
	PmergeMe(const PmergeMe& other);
	PmergeMe& operator=(const PmergeMe& other);
	~PmergeMe();

	// argv[1..argc-1] を parse & 両 container に格納
	// 不正入力 (負・非整数・overflow) は例外送出
	void	parseInput(int argc, char** argv);

	// 両 container を Ford-Johnson でソート、時間計測
	void	sortAndReport();

	// テスト用ヘルパ
	const std::vector<int>&	vec() const;
	const std::deque<int>&	deq() const;

	// Direct sort API (テスト可用)
	static void			sortVector(std::vector<int>& v);
	static void			sortDeque(std::deque<int>& d);
	static std::size_t	comparisons();
	static void			resetComparisons();

	class InputException : public std::exception {
	public:
		virtual const char* what() const throw();
	};
};

#endif
