#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cctype>

BitcoinExchange::BitcoinExchange() : _database() {}
BitcoinExchange::BitcoinExchange(const BitcoinExchange& o) : _database(o._database) {}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& o) {
	if (this != &o) _database = o._database;
	return *this;
}

BitcoinExchange::~BitcoinExchange() {}

// data.csv 読み込み: 1行目 header 「date,exchange_rate」、以降 「YYYY-MM-DD,double」
bool BitcoinExchange::loadDatabase(const std::string& filename) {
	std::ifstream ifs(filename.c_str());
	if (!ifs.is_open()) return false;

	std::string line;
	std::getline(ifs, line);  // header skip

	while (std::getline(ifs, line)) {
		if (line.empty()) continue;
		std::string::size_type comma = line.find(',');
		if (comma == std::string::npos) continue;
		std::string date = line.substr(0, comma);
		std::string rate_str = line.substr(comma + 1);
		char* endp = 0;
		double rate = std::strtod(rate_str.c_str(), &endp);
		if (endp == rate_str.c_str()) continue;
		_database[date] = rate;
	}
	return !_database.empty();
}

// 「入力日以下で最も近い日付」の rate を返す。
// std::map::lower_bound(date) は「date 以上の最初」を返すので:
//   - 完全一致 → そのまま
//   - end() → 全 DB より新しい → 最後の要素 (--end)
//   - begin() → 全 DB より古い → 見つからないケース (0.0 or 最初の値)
//   - それ以外 → 1 個前 (--it) が「date より前の最新」
double BitcoinExchange::getRate(const std::string& date) const {
	if (_database.empty()) return 0.0;
	std::map<std::string, double>::const_iterator it = _database.lower_bound(date);
	if (it != _database.end() && it->first == date) return it->second;
	if (it == _database.begin()) return _database.begin()->second;
	--it;
	return it->second;
}

// YYYY-MM-DD の厳密チェック + 実在する日付か (うるう年考慮)
bool BitcoinExchange::isValidDate(const std::string& s) {
	if (s.length() != 10) return false;
	if (s[4] != '-' || s[7] != '-') return false;
	for (int i = 0; i < 10; ++i) {
		if (i == 4 || i == 7) continue;
		if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
	}
	int y = std::atoi(s.substr(0, 4).c_str());
	int m = std::atoi(s.substr(5, 2).c_str());
	int d = std::atoi(s.substr(8, 2).c_str());
	if (y < 1 || m < 1 || m > 12 || d < 1) return false;
	int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	// うるう年: 4で割れる && 100で割れない、または 400で割れる
	if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) days[1] = 29;
	if (d > days[m - 1]) return false;
	return true;
}

// value 文字列を double にパース。空文字列や末尾ゴミは reject
bool BitcoinExchange::parseValue(const std::string& s, double& out) {
	if (s.empty()) return false;
	// 先頭に空白があってはならない (PDF format 「date | value」)
	if (std::isspace(static_cast<unsigned char>(s[0]))) return false;
	char* endp = 0;
	double v = std::strtod(s.c_str(), &endp);
	if (endp == s.c_str()) return false;
	// 末尾は空白 or NUL のみ許容
	while (*endp != '\0') {
		if (!std::isspace(static_cast<unsigned char>(*endp))) return false;
		++endp;
	}
	out = v;
	return true;
}

// 入力ファイルを処理: format 「date | value」
void BitcoinExchange::processInput(const std::string& filename) {
	std::ifstream ifs(filename.c_str());
	if (!ifs.is_open()) {
		std::cerr << "Error: could not open file." << std::endl;
		return;
	}

	std::string line;
	// 最初の1行が header 「date | value」の場合はスキップ (中身が実データなら処理)
	if (std::getline(ifs, line)) {
		if (line != "date | value") {
			// 通常の行として処理
			processLine(line);
		}
	}

	while (std::getline(ifs, line)) {
		processLine(line);
	}
}

// 1行を処理: 内部ヘルパ (BitcoinExchange.cpp 内 static 相当だがメンバ化してテスト可能に)
void BitcoinExchange::processLine(const std::string& line) {
	if (line.empty()) return;

	// separator は " | " (3文字)
	std::string::size_type sep = line.find(" | ");
	if (sep == std::string::npos) {
		std::cerr << "Error: bad input => " << line << std::endl;
		return;
	}

	std::string date = line.substr(0, sep);
	std::string value_str = line.substr(sep + 3);

	if (!isValidDate(date)) {
		std::cerr << "Error: bad input => " << line << std::endl;
		return;
	}

	double value;
	if (!parseValue(value_str, value)) {
		std::cerr << "Error: bad input => " << line << std::endl;
		return;
	}

	if (value < 0) {
		std::cerr << "Error: not a positive number." << std::endl;
		return;
	}
	if (value > 1000) {
		std::cerr << "Error: too large a number." << std::endl;
		return;
	}

	double rate = getRate(date);
	std::cout << date << " => " << value << " = " << (value * rate) << std::endl;
}

bool BitcoinExchange::hasDatabase() const { return !_database.empty(); }
std::size_t BitcoinExchange::dbSize() const { return _database.size(); }
