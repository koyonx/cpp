#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <string>
#include <vector>
#include <deque>
#include <exception>

// PmergeMe: Ford-Johnson (merge-insertion) sort を 2 種類の container で実装
// PDF 制約: ex00 (std::map) と ex01 (std::stack) で使ったコンテナは使えない
//          -> 本 ex では std::vector と std::deque を使用
//
// Ford-Johnson (merge-insertion) sort とは:
//  1. 要素をペアにし、各ペア内で (larger, smaller) に整理
//  2. larger 要素だけを取り出し、再帰的にソート (main chain 完成)
//  3. main chain の先頭要素とペアだった smaller は自明に最小 -> 先頭に挿入
//  4. 残りの smaller 要素を Jacobsthal 番号順で binary insertion
//     Jacobsthal 順は「探索範囲が常に 2^k-1 のサイズになる」ように選ばれる
//     → 総比較回数 = ⌈log2(3n/4)⌉ 前後 (n=15 くらいまでは comparison-optimal)
class PmergeMe {
private:
	// Vector 版 & Deque 版
	std::vector<int>	_vecData;
	std::deque<int>		_deqData;

	// Vector 用実装
	void	fordJohnsonVector(std::vector<int>& v);
	void	binaryInsertVector(std::vector<int>& S, int val, std::size_t maxIdx);

	// Deque 用実装
	void	fordJohnsonDeque(std::deque<int>& d);
	void	binaryInsertDeque(std::deque<int>& S, int val, std::size_t maxIdx);

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
	static void	sortVector(std::vector<int>& v);
	static void	sortDeque(std::deque<int>& d);

	class InputException : public std::exception {
	public:
		virtual const char* what() const throw();
	};
};

#endif
