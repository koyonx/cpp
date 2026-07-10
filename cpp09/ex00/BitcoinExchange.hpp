#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <string>
#include <map>

class BitcoinExchange {
private:
	// std::map: date をキーに sorted 保存
	// -> 「入力日以下で最も近い日付」を lower_bound() で O(log n) で見つけられる
	std::map<std::string, double>	_database;

	void	processLine(const std::string& line);

public:
	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange& other);
	BitcoinExchange& operator=(const BitcoinExchange& other);
	~BitcoinExchange();

	// Public API
	bool	loadDatabase(const std::string& filename);
	void	processInput(const std::string& filename);

	// Helpers (テスト可用)
	double		getRate(const std::string& date) const;
	bool		hasDatabase() const;
	std::size_t	dbSize() const;

	// Static validators (テスト可用)
	static bool	isValidDate(const std::string& s);
	static bool	parseValue(const std::string& s, double& out);
};

#endif
