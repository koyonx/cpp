#include "PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <climits>
#include <algorithm>
#include <sys/time.h>

std::size_t PmergeMe::_comparisons = 0;

PmergeMe::PmergeMe() : _vecData(), _deqData() {}
PmergeMe::PmergeMe(const PmergeMe& o) : _vecData(o._vecData), _deqData(o._deqData) {}
PmergeMe& PmergeMe::operator=(const PmergeMe& o) {
	if (this != &o) { _vecData = o._vecData; _deqData = o._deqData; }
	return *this;
}
PmergeMe::~PmergeMe() {}

const std::vector<int>& PmergeMe::vec() const { return _vecData; }
const std::deque<int>&  PmergeMe::deq() const { return _deqData; }

const char* PmergeMe::InputException::what() const throw() {
	return "Error";
}

bool PmergeMe::isLess(int a, int b) {
	++_comparisons;
	return a < b;
}

std::size_t PmergeMe::comparisons() { return _comparisons; }
void PmergeMe::resetComparisons() { _comparisons = 0; }

// ========================================================================
// 入力パース: argv[1..argc-1] を正の整数として読み取り
// ========================================================================
void PmergeMe::parseInput(int argc, char** argv) {
	_vecData.clear();
	_deqData.clear();
	if (argc < 2) throw InputException();

	for (int i = 1; i < argc; ++i) {
		std::string s(argv[i]);
		if (s.empty()) throw InputException();
		// 数字のみ許容 (負・小数・記号すべて拒否)
		for (std::size_t j = 0; j < s.length(); ++j) {
			if (s[j] < '0' || s[j] > '9') throw InputException();
		}
		// long でパースし INT 範囲チェック
		char* endp = 0;
		long v = std::strtol(s.c_str(), &endp, 10);
		if (*endp != '\0' || v < 0 || v > INT_MAX) throw InputException();
		_vecData.push_back(static_cast<int>(v));
		_deqData.push_back(static_cast<int>(v));
	}
}

// ========================================================================
// 現在時刻をマイクロ秒で取得 (gettimeofday: POSIX 標準、C++98 互換)
// ========================================================================
static double nowMicroseconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1e6 + tv.tv_usec;
}

// ========================================================================
// PDF 形式で before/after/時間出力
// ========================================================================
void PmergeMe::sortAndReport() {
	// Before line
	std::cout << "Before:";
	for (std::size_t i = 0; i < _vecData.size(); ++i) std::cout << " " << _vecData[i];
	std::cout << std::endl;

	// Vector sort with timing
	double t0v = nowMicroseconds();
	fordJohnsonVector(_vecData);
	double t1v = nowMicroseconds();

	// Deque sort with timing
	double t0d = nowMicroseconds();
	fordJohnsonDeque(_deqData);
	double t1d = nowMicroseconds();

	// After line (vector の結果)
	std::cout << "After:";
	for (std::size_t i = 0; i < _vecData.size(); ++i) std::cout << " " << _vecData[i];
	std::cout << std::endl;

	std::cout.precision(5);
	std::cout << "Time to process a range of " << _vecData.size()
			  << " elements with std::vector : " << std::fixed << (t1v - t0v) << " us"
			  << std::endl;
	std::cout << "Time to process a range of " << _deqData.size()
			  << " elements with std::deque  : " << std::fixed << (t1d - t0d) << " us"
			  << std::endl;
}

// ========================================================================
// Ford-Johnson (merge-insertion) for std::vector
//
// 要素は「元配列 values の添字」で扱う。値の比較は values[添字] で行う。
// こうすると再帰後も a と b のペア関係を partner[a] で O(1) で引ける。
// ========================================================================
void PmergeMe::fordJohnsonVector(std::vector<int>& v) {
	std::vector<std::size_t> idx(v.size());
	for (std::size_t i = 0; i < v.size(); ++i) idx[i] = i;
	mergeInsertVector(v, idx);

	std::vector<int> sorted;
	sorted.reserve(v.size());
	for (std::size_t i = 0; i < idx.size(); ++i) sorted.push_back(v[idx[i]]);
	v = sorted;
}

void PmergeMe::mergeInsertVector(const std::vector<int>& values,
								 std::vector<std::size_t>& idx) {
	if (idx.size() <= 1) return;

	// Step 0: 奇数個なら最後の 1 個 (straggler) はペアを組まない
	bool hasStraggler = (idx.size() % 2 == 1);
	std::size_t straggler = hasStraggler ? idx.back() : 0;

	// Step 1: 2 個ずつペアにして 1 回比較。大きい方 a を winners へ、
	//         小さい方 b は partner[a] に覚えておく
	std::size_t m = idx.size() / 2;
	std::vector<std::size_t> winners;
	winners.reserve(m);
	std::vector<std::size_t> partner(values.size());
	for (std::size_t i = 0; i < m; ++i) {
		std::size_t x = idx[2 * i];
		std::size_t y = idx[2 * i + 1];
		if (isLess(values[x], values[y])) std::swap(x, y);  // x が大きい方
		winners.push_back(x);
		partner[x] = y;
	}

	// Step 2: a (winners) だけを再帰的にソート -> a1 <= a2 <= ... <= am
	mergeInsertVector(values, winners);

	// Step 3: main chain S = [b1, a1, a2, ..., am]
	//         b1 < a1 <= 他の a なので b1 は比較なしで先頭に置ける
	//         aPos[i] = S の中で a_(i+1) が今どこにいるか
	std::vector<std::size_t> S;
	S.reserve(idx.size());
	S.push_back(partner[winners[0]]);
	for (std::size_t i = 0; i < m; ++i) S.push_back(winners[i]);
	std::vector<std::size_t> aPos(m);
	for (std::size_t i = 0; i < m; ++i) aPos[i] = i + 1;

	// Step 4: 残りの b を Jacobsthal 順に挿入
	//   Jacobsthal 数 1, 3, 5, 11, 21, 43 ... (J_n = J_{n-1} + 2 * J_{n-2})
	//   グループ (prev, curr] を大きい番号から: b3 b2 | b5 b4 | b11 ... b6 | ...
	//   straggler は「相方の a がいない b_(m+1)」として同じ順序に乗せる
	std::size_t nb = m + (hasStraggler ? 1 : 0);
	std::size_t prev = 1;
	std::size_t curr = 3;
	while (prev < nb) {
		std::size_t high = (curr < nb) ? curr : nb;
		for (std::size_t j = high; j > prev; --j) {
			std::size_t i = j - 1;  // b_j の 0-origin
			bool isStraggler = (i == m);
			std::size_t x = isStraggler ? straggler : partner[winners[i]];
			// b_j < a_j は既知なので a_j より前だけを探す (straggler は全体)
			std::size_t bound = isStraggler ? S.size() : aPos[i];
			std::size_t p = binaryInsertVector(values, S, x, bound);
			// 挿入位置以降にいた a は 1 つ後ろにずれる
			for (std::size_t k = 0; k < m; ++k)
				if (aPos[k] >= p) ++aPos[k];
		}
		std::size_t next = curr + 2 * prev;
		prev = curr;
		curr = next;
	}
	idx = S;
}

// S[0, bound) の中で values[x] を入れる位置を二分探索し、挿入する。
// 等しい値の後ろに入れる (upper_bound と同じ)。挿入位置を返す。
// 探索範囲が 2^k - 1 個ならちょうど k 回の比較で決まる。
std::size_t PmergeMe::binaryInsertVector(const std::vector<int>& values,
		std::vector<std::size_t>& S, std::size_t x, std::size_t bound) {
	std::size_t lo = 0;
	std::size_t hi = bound;
	while (lo < hi) {
		std::size_t mid = lo + (hi - lo) / 2;
		if (isLess(values[x], values[S[mid]])) hi = mid;
		else lo = mid + 1;
	}
	S.insert(S.begin() + lo, x);
	return lo;
}

// ========================================================================
// Ford-Johnson for std::deque (ロジックは vector 版と同じ)
// PDF: 「container ごとに実装せよ」なので template で共通化せず個別に書く
// ========================================================================
void PmergeMe::fordJohnsonDeque(std::deque<int>& d) {
	std::deque<std::size_t> idx(d.size());
	for (std::size_t i = 0; i < d.size(); ++i) idx[i] = i;
	mergeInsertDeque(d, idx);

	std::deque<int> sorted;
	for (std::size_t i = 0; i < idx.size(); ++i) sorted.push_back(d[idx[i]]);
	d = sorted;
}

void PmergeMe::mergeInsertDeque(const std::deque<int>& values,
								std::deque<std::size_t>& idx) {
	if (idx.size() <= 1) return;

	bool hasStraggler = (idx.size() % 2 == 1);
	std::size_t straggler = hasStraggler ? idx.back() : 0;

	std::size_t m = idx.size() / 2;
	std::deque<std::size_t> winners;
	std::deque<std::size_t> partner(values.size());
	for (std::size_t i = 0; i < m; ++i) {
		std::size_t x = idx[2 * i];
		std::size_t y = idx[2 * i + 1];
		if (isLess(values[x], values[y])) std::swap(x, y);
		winners.push_back(x);
		partner[x] = y;
	}

	mergeInsertDeque(values, winners);

	std::deque<std::size_t> S;
	for (std::size_t i = 0; i < m; ++i) S.push_back(winners[i]);
	S.push_front(partner[winners[0]]);
	std::deque<std::size_t> aPos(m);
	for (std::size_t i = 0; i < m; ++i) aPos[i] = i + 1;

	std::size_t nb = m + (hasStraggler ? 1 : 0);
	std::size_t prev = 1;
	std::size_t curr = 3;
	while (prev < nb) {
		std::size_t high = (curr < nb) ? curr : nb;
		for (std::size_t j = high; j > prev; --j) {
			std::size_t i = j - 1;
			bool isStraggler = (i == m);
			std::size_t x = isStraggler ? straggler : partner[winners[i]];
			std::size_t bound = isStraggler ? S.size() : aPos[i];
			std::size_t p = binaryInsertDeque(values, S, x, bound);
			for (std::size_t k = 0; k < m; ++k)
				if (aPos[k] >= p) ++aPos[k];
		}
		std::size_t next = curr + 2 * prev;
		prev = curr;
		curr = next;
	}
	idx = S;
}

std::size_t PmergeMe::binaryInsertDeque(const std::deque<int>& values,
		std::deque<std::size_t>& S, std::size_t x, std::size_t bound) {
	std::size_t lo = 0;
	std::size_t hi = bound;
	while (lo < hi) {
		std::size_t mid = lo + (hi - lo) / 2;
		if (isLess(values[x], values[S[mid]])) hi = mid;
		else lo = mid + 1;
	}
	S.insert(S.begin() + static_cast<std::ptrdiff_t>(lo), x);
	return lo;
}

// ========================================================================
// Static direct API (テスト用)
// ========================================================================
void PmergeMe::sortVector(std::vector<int>& v) {
	fordJohnsonVector(v);
}

void PmergeMe::sortDeque(std::deque<int>& d) {
	fordJohnsonDeque(d);
}
