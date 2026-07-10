#include "TraceSort.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

// ---- pretty print helpers ----------------------------------------------------
std::string TraceSort::toString(const std::vector<int>& v) {
	std::ostringstream oss;
	oss << "[";
	for (std::size_t i = 0; i < v.size(); ++i) {
		if (i) oss << ", ";
		oss << v[i];
	}
	oss << "]";
	return oss.str();
}

std::string TraceSort::pairsToString(const std::vector<std::pair<int, int> >& p) {
	std::ostringstream oss;
	oss << "[";
	for (std::size_t i = 0; i < p.size(); ++i) {
		if (i) oss << ", ";
		oss << "(" << p[i].first << "," << p[i].second << ")";
	}
	oss << "]";
	return oss.str();
}

std::string TraceSort::indent(int depth) {
	return std::string(static_cast<std::size_t>(depth * 2), ' ');
}

// upper_bound で位置を決めて S に insert。ついでにトレース出力
void TraceSort::binaryInsertWithTrace(std::vector<int>& S, int val, int depth) {
	std::vector<int>::iterator pos = std::upper_bound(S.begin(), S.end(), val);
	std::size_t idx = static_cast<std::size_t>(pos - S.begin());
	std::cout << indent(depth)
			  << "    -> upper_bound(" << val << ") points to index " << idx
			  << std::endl;
	S.insert(pos, val);
	std::cout << indent(depth) << "    After insert: " << toString(S) << std::endl;
}

// ---- Main algorithm ----------------------------------------------------------
void TraceSort::sort(std::vector<int>& v, int depth) {
	const std::string ind = indent(depth);
	std::ostringstream tagOss;
	tagOss << "[depth " << depth << "]";
	const std::string tag = tagOss.str();

	std::cout << ind << tag << " Input: " << toString(v) << std::endl;

	// --- 終端条件 ---
	if (v.size() <= 1) {
		std::cout << ind << tag << " size <= 1, return unchanged" << std::endl;
		return;
	}

	// --- Step 0: straggler 退避 ---
	bool hasStraggler = (v.size() % 2 == 1);
	int straggler = 0;
	if (hasStraggler) {
		straggler = v.back();
		v.pop_back();
		std::cout << ind << tag << " Odd size -> straggler = " << straggler
				  << ", remaining = " << toString(v) << std::endl;
	} else {
		std::cout << ind << tag << " Even size, no straggler" << std::endl;
	}

	// --- Step 1: pair up (larger, smaller) ---
	std::vector<std::pair<int, int> > pairs;
	for (std::size_t i = 0; i < v.size(); i += 2) {
		int a = v[i], b = v[i + 1];
		if (a < b) std::swap(a, b);
		pairs.push_back(std::make_pair(a, b));
	}
	std::cout << ind << tag << " Step 1 - Pairs (larger, smaller): "
			  << pairsToString(pairs) << std::endl;

	// --- Step 2: recursively sort winners ---
	std::vector<int> winners;
	for (std::size_t i = 0; i < pairs.size(); ++i) winners.push_back(pairs[i].first);
	std::cout << ind << tag << " Step 2 - Recursing on winners: "
			  << toString(winners) << std::endl;
	sort(winners, depth + 1);
	std::cout << ind << tag << " Step 2 done - winners sorted: "
			  << toString(winners) << std::endl;

	// --- Step 3: reorder pairs to match sorted winners ---
	std::vector<std::pair<int, int> > sortedPairs;
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
	std::cout << ind << tag << " Step 3 - Reordered pairs: "
			  << pairsToString(pairs) << std::endl;

	// --- Step 4: build main chain S from winners, prepend b1 ---
	std::vector<int> S;
	for (std::size_t i = 0; i < pairs.size(); ++i) S.push_back(pairs[i].first);
	std::cout << ind << tag << " Step 4a - Main chain from winners: "
			  << toString(S) << std::endl;
	S.insert(S.begin(), pairs[0].second);
	std::cout << ind << tag << " Step 4b - Prepend b1=" << pairs[0].second
			  << " (smaller of first pair, trivially min): "
			  << toString(S) << std::endl;

	// --- Step 5: Jacobsthal-guided binary insertion for remaining losers ---
	std::vector<std::size_t> jacob;
	jacob.push_back(1);
	jacob.push_back(3);
	while (jacob.back() < pairs.size() + 2) {
		jacob.push_back(jacob.back() + 2 * jacob[jacob.size() - 2]);
	}
	std::cout << ind << tag << " Step 5a - Jacobsthal seq: ";
	for (std::size_t i = 0; i < jacob.size(); ++i)
		std::cout << jacob[i] << (i + 1 < jacob.size() ? ", " : "");
	std::cout << std::endl;

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
	for (std::size_t i = 1; i < pairs.size(); ++i) {
		if (!inserted[i]) insertOrder.push_back(i);
	}

	std::cout << ind << tag << " Step 5b - Insertion order (indices into pairs): ";
	if (insertOrder.empty()) std::cout << "(none)";
	for (std::size_t i = 0; i < insertOrder.size(); ++i)
		std::cout << "b" << (insertOrder[i] + 1)
				  << "=" << pairs[insertOrder[i]].second
				  << (i + 1 < insertOrder.size() ? ", " : "");
	std::cout << std::endl;

	for (std::size_t k = 0; k < insertOrder.size(); ++k) {
		std::size_t i = insertOrder[k];
		std::cout << ind << tag << " Step 5c - Inserting b" << (i + 1)
				  << "=" << pairs[i].second << " via binary search:" << std::endl;
		binaryInsertWithTrace(S, pairs[i].second, depth);
	}

	// --- Step 6: straggler ---
	if (hasStraggler) {
		std::cout << ind << tag << " Step 6 - Inserting straggler=" << straggler
				  << " via binary search:" << std::endl;
		binaryInsertWithTrace(S, straggler, depth);
	} else {
		std::cout << ind << tag << " Step 6 - No straggler" << std::endl;
	}

	v = S;
	std::cout << ind << tag << " Final at this depth: " << toString(v) << std::endl;
}
