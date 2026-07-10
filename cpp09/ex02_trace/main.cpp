#include "TraceSort.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <climits>

static void printBar(const std::string& title) {
	std::cout << "\n============================================================\n";
	std::cout << " " << title << "\n";
	std::cout << "============================================================\n";
}

static bool parseArgs(int argc, char** argv, std::vector<int>& out) {
	for (int i = 1; i < argc; ++i) {
		std::string s(argv[i]);
		if (s.empty()) return false;
		for (std::size_t j = 0; j < s.length(); ++j) {
			if (s[j] < '0' || s[j] > '9') return false;
		}
		char* endp = 0;
		long v = std::strtol(s.c_str(), &endp, 10);
		if (*endp != '\0' || v < 0 || v > INT_MAX) return false;
		out.push_back(static_cast<int>(v));
	}
	return true;
}

int main(int argc, char** argv) {
	std::vector<int> v;

	if (argc == 1) {
		// デフォルト: PDF 例
		int def[] = {3, 5, 9, 7, 4};
		for (int i = 0; i < 5; ++i) v.push_back(def[i]);
		std::cout << "(no args: using PDF example)\n" << std::endl;
	} else {
		if (!parseArgs(argc, argv, v)) {
			std::cerr << "Error: invalid input" << std::endl;
			return 1;
		}
	}

	printBar("Ford-Johnson (merge-insertion) sort - step by step trace");
	std::cout << "Initial input: [";
	for (std::size_t i = 0; i < v.size(); ++i) {
		if (i) std::cout << ", ";
		std::cout << v[i];
	}
	std::cout << "]" << std::endl;

	printBar("Trace");
	TraceSort::sort(v, 0);

	printBar("Result");
	std::cout << "Sorted output: [";
	for (std::size_t i = 0; i < v.size(); ++i) {
		if (i) std::cout << ", ";
		std::cout << v[i];
	}
	std::cout << "]" << std::endl;

	return 0;
}
