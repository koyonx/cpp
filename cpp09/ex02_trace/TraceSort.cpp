#include "TraceSort.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

std::size_t TraceSort::_comparisons = 0;

bool TraceSort::isLess(int a, int b) {
	++_comparisons;
	return a < b;
}

std::size_t TraceSort::comparisons() { return _comparisons; }

// ---- pretty print helpers ----------------------------------------------------
std::string TraceSort::toString(const std::vector<int>& values,
								const std::vector<std::size_t>& idx) {
	std::ostringstream oss;
	oss << "[";
	for (std::size_t i = 0; i < idx.size(); ++i) {
		if (i) oss << ", ";
		oss << values[idx[i]];
	}
	oss << "]";
	return oss.str();
}

std::string TraceSort::indent(int depth) {
	return std::string(static_cast<std::size_t>(depth * 2), ' ');
}

// ---- Entry point ---------------------------------------------------------------
void TraceSort::sort(std::vector<int>& v) {
	_comparisons = 0;
	std::vector<std::size_t> idx(v.size());
	for (std::size_t i = 0; i < v.size(); ++i) idx[i] = i;
	sortIdx(v, idx, 0);
	std::vector<int> sorted;
	for (std::size_t i = 0; i < idx.size(); ++i) sorted.push_back(v[idx[i]]);
	v = sorted;
}

// ---- Main algorithm (ex02 の mergeInsertVector と同じ) -------------------------
void TraceSort::sortIdx(const std::vector<int>& values,
						std::vector<std::size_t>& idx, int depth) {
	const std::string ind = indent(depth);
	std::ostringstream tagOss;
	tagOss << "[depth " << depth << "]";
	const std::string tag = tagOss.str();

	std::cout << ind << tag << " Input: " << toString(values, idx) << std::endl;

	// --- 終端条件 ---
	if (idx.size() <= 1) {
		std::cout << ind << tag << " size <= 1, return unchanged" << std::endl;
		return;
	}

	// --- Step 0: straggler ---
	bool hasStraggler = (idx.size() % 2 == 1);
	std::size_t straggler = hasStraggler ? idx.back() : 0;
	if (hasStraggler)
		std::cout << ind << tag << " Odd size -> straggler = " << values[straggler]
				  << " (treated as the last b, with no partner a)" << std::endl;
	else
		std::cout << ind << tag << " Even size, no straggler" << std::endl;

	// --- Step 1: pair up ---
	std::size_t m = idx.size() / 2;
	std::vector<std::size_t> winners;
	std::vector<std::size_t> partner(values.size());
	std::cout << ind << tag << " Step 1 - Pairs (a=larger, b=smaller):";
	for (std::size_t i = 0; i < m; ++i) {
		std::size_t x = idx[2 * i];
		std::size_t y = idx[2 * i + 1];
		if (isLess(values[x], values[y])) std::swap(x, y);
		winners.push_back(x);
		partner[x] = y;
		std::cout << " (" << values[x] << "," << values[y] << ")";
	}
	std::cout << "   [" << m << " comparisons]" << std::endl;

	// --- Step 2: recursion ---
	std::cout << ind << tag << " Step 2 - Recursing on a's: "
			  << toString(values, winners) << std::endl;
	sortIdx(values, winners, depth + 1);
	std::cout << ind << tag << " Step 2 done - a's sorted: "
			  << toString(values, winners) << std::endl;

	// --- Step 3: main chain ---
	std::vector<std::size_t> S;
	S.push_back(partner[winners[0]]);
	for (std::size_t i = 0; i < m; ++i) S.push_back(winners[i]);
	std::vector<std::size_t> aPos(m);
	for (std::size_t i = 0; i < m; ++i) aPos[i] = i + 1;
	std::cout << ind << tag << " Step 3 - Main chain [b1, a1..a" << m << "]: "
			  << toString(values, S) << "   (b1=" << values[S[0]]
			  << " < a1, so no comparison needed)" << std::endl;
	std::cout << ind << tag << " Pending b's:";
	for (std::size_t i = 1; i < m; ++i)
		std::cout << " b" << (i + 1) << "=" << values[partner[winners[i]]]
				  << "(<a" << (i + 1) << "=" << values[winners[i]] << ")";
	if (hasStraggler)
		std::cout << " b" << (m + 1) << "=" << values[straggler] << "(straggler)";
	std::cout << std::endl;

	// --- Step 4: Jacobsthal order insertion ---
	std::size_t nb = m + (hasStraggler ? 1 : 0);
	std::size_t prev = 1;
	std::size_t curr = 3;
	while (prev < nb) {
		std::size_t high = (curr < nb) ? curr : nb;
		std::cout << ind << tag << " Step 4 - Jacobsthal group (" << prev << ", "
				  << curr << "]: insert b" << high << " down to b" << (prev + 1)
				  << std::endl;
		for (std::size_t j = high; j > prev; --j) {
			std::size_t i = j - 1;
			bool isStraggler = (i == m);
			std::size_t x = isStraggler ? straggler : partner[winners[i]];
			std::size_t bound = isStraggler ? S.size() : aPos[i];

			std::vector<std::size_t> range(S.begin(), S.begin() + bound);
			std::cout << ind << "    b" << j << "=" << values[x] << ": search "
					  << toString(values, range) << " (" << bound << " elements"
					  << (isStraggler ? ", whole chain" : ", left of a") ;
			if (!isStraggler) std::cout << j;
			std::cout << ")" << std::endl;

			std::size_t before = _comparisons;
			std::size_t lo = 0;
			std::size_t hi = bound;
			while (lo < hi) {
				std::size_t mid = lo + (hi - lo) / 2;
				if (isLess(values[x], values[S[mid]])) hi = mid;
				else lo = mid + 1;
			}
			S.insert(S.begin() + lo, x);
			for (std::size_t k = 0; k < m; ++k)
				if (aPos[k] >= lo) ++aPos[k];

			std::cout << ind << "      -> index " << lo << " ("
					  << (_comparisons - before) << " comparisons): "
					  << toString(values, S) << std::endl;
		}
		std::size_t next = curr + 2 * prev;
		prev = curr;
		curr = next;
	}

	idx = S;
	std::cout << ind << tag << " Final at this depth: " << toString(values, idx)
			  << std::endl;
}
