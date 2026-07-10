#include "PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <climits>
#include <algorithm>
#include <sys/time.h>

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
	std::cout << "After: ";
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
// アルゴリズム:
// 1. Pair up: v を 2 個ずつ組み、各 pair 内で (larger, smaller) に並べる
// 2. Recursive: larger 要素 (winners) だけを取り出し、再帰的にソート
// 3. Main chain 構築: 再帰終了後、winner 列は既ソート
// 4. Prepend b1: main chain 先頭 winner とペアだった smaller は自明に最小
// 5. Jacobsthal 挿入: 残り smaller を Jacobsthal 順で main chain に binary insertion
// 6. Straggler: 元が奇数個なら余った要素も binary insertion
// ========================================================================
void PmergeMe::fordJohnsonVector(std::vector<int>& v) {
	if (v.size() <= 1) return;

	// Step 0: 奇数個なら straggler を退避
	bool hasStraggler = (v.size() % 2 == 1);
	int straggler = 0;
	if (hasStraggler) {
		straggler = v.back();
		v.pop_back();
	}

	// Step 1: (larger, smaller) の pair 列を作る
	std::vector<std::pair<int, int> > pairs;
	pairs.reserve(v.size() / 2);
	for (std::size_t i = 0; i < v.size(); i += 2) {
		int a = v[i], b = v[i + 1];
		if (a < b) std::swap(a, b);
		pairs.push_back(std::make_pair(a, b));
	}

	// Step 2: 一時的に larger 要素のみ取り出し、再帰的にソート
	std::vector<int> winners;
	winners.reserve(pairs.size());
	for (std::size_t i = 0; i < pairs.size(); ++i) winners.push_back(pairs[i].first);
	fordJohnsonVector(winners);

	// Step 3: sorted winners に合わせて pairs を並べ替え (安定化)
	std::vector<std::pair<int, int> > sortedPairs;
	sortedPairs.reserve(pairs.size());
	std::vector<bool> used(pairs.size(), false);
	for (std::size_t i = 0; i < winners.size(); ++i) {
		for (std::size_t j = 0; j < pairs.size(); ++j) {
			if (!used[j] && pairs[j].first == winners[i]) {
				sortedPairs.push_back(pairs[j]);
				used[j] = true;
				break;
			}
		}
	}
	pairs = sortedPairs;

	// Step 4: main chain S を winners から作り、b1 (先頭 pair の loser) を prepend
	std::vector<int> S;
	S.reserve(v.size() + 1);
	for (std::size_t i = 0; i < pairs.size(); ++i) S.push_back(pairs[i].first);
	S.insert(S.begin(), pairs[0].second);

	// Step 5: Jacobsthal 順で残り loser を binary insertion
	// Jacobsthal: J_0=0, J_1=1, J_n = J_{n-1} + 2*J_{n-2} -> 1, 3, 5, 11, 21, 43...
	std::vector<std::size_t> jacob;
	jacob.push_back(1);
	jacob.push_back(3);
	while (jacob.back() < pairs.size() + 2) {
		jacob.push_back(jacob.back() + 2 * jacob[jacob.size() - 2]);
	}

	std::vector<std::size_t> insertOrder;
	std::vector<bool> inserted(pairs.size(), false);
	inserted[0] = true;  // b1 already prepended
	for (std::size_t k = 2; k < jacob.size(); ++k) {
		std::size_t high = jacob[k] - 1;
		std::size_t low = jacob[k - 1];
		if (high >= pairs.size()) high = pairs.size() - 1;
		if (low > high) continue;
		for (std::size_t i = high + 1; i > low; --i) {
			std::size_t idx = i - 1;
			if (!inserted[idx]) {
				insertOrder.push_back(idx);
				inserted[idx] = true;
			}
		}
	}
	// 残り (Jacobsthal 到達外の tail) を昇順で追加
	for (std::size_t i = 1; i < pairs.size(); ++i) {
		if (!inserted[i]) insertOrder.push_back(i);
	}

	for (std::size_t k = 0; k < insertOrder.size(); ++k) {
		std::size_t i = insertOrder[k];
		binaryInsertVector(S, pairs[i].second, S.size());
	}

	// Step 6: straggler があれば binary insertion
	if (hasStraggler) {
		binaryInsertVector(S, straggler, S.size());
	}

	v = S;
}

// std::upper_bound で位置決定し、std::vector::insert で挿入
void PmergeMe::binaryInsertVector(std::vector<int>& S, int val, std::size_t maxIdx) {
	if (maxIdx > S.size()) maxIdx = S.size();
	std::vector<int>::iterator lo = S.begin();
	std::vector<int>::iterator hi = S.begin() + maxIdx;
	std::vector<int>::iterator pos = std::upper_bound(lo, hi, val);
	S.insert(pos, val);
}

// ========================================================================
// Ford-Johnson for std::deque (実装は vector 版とほぼ同じロジック)
// PDF: 「container ごとに実装せよ」なので同じ template で回さず個別に書く
// ========================================================================
void PmergeMe::fordJohnsonDeque(std::deque<int>& d) {
	if (d.size() <= 1) return;

	bool hasStraggler = (d.size() % 2 == 1);
	int straggler = 0;
	if (hasStraggler) {
		straggler = d.back();
		d.pop_back();
	}

	std::deque<std::pair<int, int> > pairs;
	for (std::size_t i = 0; i < d.size(); i += 2) {
		int a = d[i], b = d[i + 1];
		if (a < b) std::swap(a, b);
		pairs.push_back(std::make_pair(a, b));
	}

	std::deque<int> winners;
	for (std::size_t i = 0; i < pairs.size(); ++i) winners.push_back(pairs[i].first);
	fordJohnsonDeque(winners);

	std::deque<std::pair<int, int> > sortedPairs;
	std::deque<bool> used(pairs.size(), false);
	for (std::size_t i = 0; i < winners.size(); ++i) {
		for (std::size_t j = 0; j < pairs.size(); ++j) {
			if (!used[j] && pairs[j].first == winners[i]) {
				sortedPairs.push_back(pairs[j]);
				used[j] = true;
				break;
			}
		}
	}
	pairs = sortedPairs;

	std::deque<int> S;
	for (std::size_t i = 0; i < pairs.size(); ++i) S.push_back(pairs[i].first);
	S.push_front(pairs[0].second);

	std::deque<std::size_t> jacob;
	jacob.push_back(1);
	jacob.push_back(3);
	while (jacob.back() < pairs.size() + 2) {
		jacob.push_back(jacob.back() + 2 * jacob[jacob.size() - 2]);
	}

	std::deque<std::size_t> insertOrder;
	std::deque<bool> inserted(pairs.size(), false);
	inserted[0] = true;
	for (std::size_t k = 2; k < jacob.size(); ++k) {
		std::size_t high = jacob[k] - 1;
		std::size_t low = jacob[k - 1];
		if (high >= pairs.size()) high = pairs.size() - 1;
		if (low > high) continue;
		for (std::size_t i = high + 1; i > low; --i) {
			std::size_t idx = i - 1;
			if (!inserted[idx]) {
				insertOrder.push_back(idx);
				inserted[idx] = true;
			}
		}
	}
	for (std::size_t i = 1; i < pairs.size(); ++i) {
		if (!inserted[i]) insertOrder.push_back(i);
	}

	for (std::size_t k = 0; k < insertOrder.size(); ++k) {
		std::size_t i = insertOrder[k];
		binaryInsertDeque(S, pairs[i].second, S.size());
	}

	if (hasStraggler) {
		binaryInsertDeque(S, straggler, S.size());
	}

	d = S;
}

void PmergeMe::binaryInsertDeque(std::deque<int>& S, int val, std::size_t maxIdx) {
	if (maxIdx > S.size()) maxIdx = S.size();
	std::deque<int>::iterator lo = S.begin();
	std::deque<int>::iterator hi = S.begin() + static_cast<std::ptrdiff_t>(maxIdx);
	std::deque<int>::iterator pos = std::upper_bound(lo, hi, val);
	S.insert(pos, val);
}

// ========================================================================
// Static direct API (テスト用)
// ========================================================================
void PmergeMe::sortVector(std::vector<int>& v) {
	PmergeMe p;
	p.fordJohnsonVector(v);
}

void PmergeMe::sortDeque(std::deque<int>& d) {
	PmergeMe p;
	p.fordJohnsonDeque(d);
}
