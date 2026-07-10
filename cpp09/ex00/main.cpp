#include "BitcoinExchange.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstddef>

// ==== テストハーネス (---test オプション時実行) ==============================

static int g_pass = 0;
static int g_fail = 0;

static void section(const std::string& t) {
	std::cout << "\n=== " << t << " ===" << std::endl;
}
static void expect(bool cond, const std::string& what) {
	if (cond) { ++g_pass; std::cout << "  [PASS] " << what << std::endl; }
	else      { ++g_fail; std::cout << "  [FAIL] " << what << std::endl; }
}

// stdout / stderr を一時的に捕捉
class StreamCapture {
	std::streambuf* _saved_out;
	std::streambuf* _saved_err;
	std::ostringstream _oss_out;
	std::ostringstream _oss_err;
public:
	StreamCapture() {
		_saved_out = std::cout.rdbuf(_oss_out.rdbuf());
		_saved_err = std::cerr.rdbuf(_oss_err.rdbuf());
	}
	~StreamCapture() {
		std::cout.rdbuf(_saved_out);
		std::cerr.rdbuf(_saved_err);
	}
	std::string out() const { return _oss_out.str(); }
	std::string err() const { return _oss_err.str(); }
};

// テスト用の一時ファイル書き出し
static void writeFile(const std::string& path, const std::string& content) {
	std::ofstream ofs(path.c_str());
	ofs << content;
}

static void removeFile(const std::string& path) {
	std::remove(path.c_str());
}

static bool contains(const std::string& hay, const std::string& needle) {
	return hay.find(needle) != std::string::npos;
}

static int runTests() {
	// === 1. isValidDate: 正常な日付 ===
	section("1. isValidDate: valid formats");
	expect(BitcoinExchange::isValidDate("2011-01-03"), "2011-01-03 valid");
	expect(BitcoinExchange::isValidDate("2020-02-29"), "2020-02-29 leap year valid");
	expect(BitcoinExchange::isValidDate("2000-02-29"), "2000-02-29 (400-year) valid");
	expect(BitcoinExchange::isValidDate("2009-12-31"), "year end valid");
	expect(BitcoinExchange::isValidDate("2009-01-01"), "year start valid");

	// === 2. isValidDate: 不正な日付 ===
	section("2. isValidDate: invalid formats");
	expect(!BitcoinExchange::isValidDate(""), "empty");
	expect(!BitcoinExchange::isValidDate("2011-1-3"), "no zero-padding");
	expect(!BitcoinExchange::isValidDate("2011/01/03"), "wrong separator");
	expect(!BitcoinExchange::isValidDate("2001-42-42"), "invalid month/day (PDF ex)");
	expect(!BitcoinExchange::isValidDate("2011-00-01"), "month 0");
	expect(!BitcoinExchange::isValidDate("2011-13-01"), "month 13");
	expect(!BitcoinExchange::isValidDate("2011-01-00"), "day 0");
	expect(!BitcoinExchange::isValidDate("2011-01-32"), "day 32");
	expect(!BitcoinExchange::isValidDate("2011-02-29"), "2011 is not leap year");
	expect(!BitcoinExchange::isValidDate("1900-02-29"), "1900 not leap (100 but not 400)");
	expect(!BitcoinExchange::isValidDate("2011-01-XX"), "non-digit day");
	expect(!BitcoinExchange::isValidDate("abcd-01-01"), "non-digit year");
	expect(!BitcoinExchange::isValidDate("2011-01-030"), "11 chars too long");
	expect(!BitcoinExchange::isValidDate("2011-01"), "too short");

	// === 3. parseValue ===
	section("3. parseValue");
	{
		double v;
		expect(BitcoinExchange::parseValue("42", v) && v == 42, "int 42");
		expect(BitcoinExchange::parseValue("3.14", v) && v == 3.14, "float 3.14");
		expect(BitcoinExchange::parseValue("0", v) && v == 0, "0");
		expect(BitcoinExchange::parseValue("1000", v) && v == 1000, "1000 boundary");
		expect(BitcoinExchange::parseValue("-1", v) && v == -1, "negative (returned, caller checks range)");
		expect(BitcoinExchange::parseValue("1.0e2", v) && v == 100, "scientific notation");
		expect(!BitcoinExchange::parseValue("", v), "empty rejected");
		expect(!BitcoinExchange::parseValue("abc", v), "letters rejected");
		expect(!BitcoinExchange::parseValue("42x", v), "trailing garbage rejected");
		expect(!BitcoinExchange::parseValue(" 42", v), "leading space rejected");
	}

	// === 4. loadDatabase: 実 data.csv ===
	section("4. loadDatabase: load actual data.csv");
	{
		BitcoinExchange btc;
		bool ok = btc.loadDatabase("data.csv");
		expect(ok, "data.csv loaded");
		expect(btc.dbSize() > 100, "more than 100 entries");
	}

	// === 5. loadDatabase: 不在ファイル ===
	section("5. loadDatabase: missing file");
	{
		BitcoinExchange btc;
		bool ok = btc.loadDatabase("nonexistent_file_12345.csv");
		expect(!ok, "returns false");
		expect(!btc.hasDatabase(), "no database");
	}

	// === 6. loadDatabase: 空データベース ===
	section("6. loadDatabase: mock CSV in temp file");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,100.0\n"
			"2020-06-15,200.5\n"
			"2021-01-01,50000.0\n");
		BitcoinExchange btc;
		expect(btc.loadDatabase("/tmp/mock_db.csv"), "loaded");
		expect(btc.dbSize() == 3, "3 entries");
		removeFile("/tmp/mock_db.csv");
	}

	// === 7. getRate: 完全一致 ===
	section("7. getRate: exact date match");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,100.0\n"
			"2020-06-15,200.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		expect(btc.getRate("2020-01-01") == 100.0, "exact -> 100");
		expect(btc.getRate("2020-06-15") == 200.0, "exact -> 200");
		removeFile("/tmp/mock_db.csv");
	}

	// === 8. getRate: 範囲内で closest lower date ===
	section("8. getRate: closest lower date");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,100.0\n"
			"2020-06-15,200.0\n"
			"2021-01-01,300.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		expect(btc.getRate("2020-03-01") == 100.0, "between -> use lower");
		expect(btc.getRate("2020-12-31") == 200.0, "just before next -> use lower");
		expect(btc.getRate("2020-06-14") == 100.0, "just before exact -> use lower");
		expect(btc.getRate("2020-06-16") == 200.0, "just after exact -> use that");
		removeFile("/tmp/mock_db.csv");
	}

	// === 9. getRate: 範囲外 (全 DB より新しい) ===
	section("9. getRate: after all entries");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,100.0\n"
			"2020-06-15,200.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		expect(btc.getRate("2099-12-31") == 200.0, "future date -> last known");
		removeFile("/tmp/mock_db.csv");
	}

	// === 10. getRate: 範囲外 (全 DB より古い) ===
	section("10. getRate: before all entries");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,100.0\n"
			"2020-06-15,200.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		// PDF は明示していないが、DB より古い日付は最古の rate を返す実装を選択
		expect(btc.getRate("1999-01-01") == 100.0, "pre-DB -> first entry");
		removeFile("/tmp/mock_db.csv");
	}

	// === 11. processInput: PDF 例の入力 ===
	section("11. processInput: PDF example");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2009-01-02,0\n"
			"2011-01-03,0.3\n"
			"2011-01-09,0.32\n"
			"2012-01-11,7.1\n");
		writeFile("/tmp/mock_input.txt",
			"date | value\n"
			"2011-01-03 | 3\n"
			"2011-01-03 | 2\n"
			"2011-01-03 | 1\n"
			"2011-01-03 | 1.2\n"
			"2011-01-09 | 1\n"
			"2012-01-11 | -1\n"
			"2001-42-42\n"
			"2012-01-11 | 1\n"
			"2012-01-11 | 2147483648\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		StreamCapture cap;
		btc.processInput("/tmp/mock_input.txt");
		std::string out = cap.out();
		std::string err = cap.err();
		expect(contains(out, "2011-01-03 => 3 = 0.9"), "3 * 0.3 = 0.9");
		expect(contains(out, "2011-01-03 => 2 = 0.6"), "2 * 0.3 = 0.6");
		expect(contains(out, "2011-01-03 => 1 = 0.3"), "1 * 0.3 = 0.3");
		expect(contains(out, "2011-01-09 => 1 = 0.32"), "1 * 0.32 = 0.32");
		expect(contains(out, "2012-01-11 => 1 = 7.1"), "1 * 7.1 = 7.1");
		expect(contains(err, "not a positive number"), "-1 -> error");
		expect(contains(err, "bad input => 2001-42-42"), "malformed date");
		expect(contains(err, "too large a number"), "2147483648 -> too large");
		removeFile("/tmp/mock_db.csv");
		removeFile("/tmp/mock_input.txt");
	}

	// === 12. processInput: 存在しないファイル ===
	section("12. processInput: file missing");
	{
		BitcoinExchange btc;
		StreamCapture cap;
		btc.processInput("/tmp/does_not_exist_98765.txt");
		expect(contains(cap.err(), "could not open file"), "error message");
	}

	// === 13. processInput: 空ファイル ===
	section("13. processInput: empty file");
	{
		writeFile("/tmp/empty.txt", "");
		BitcoinExchange btc;
		StreamCapture cap;
		btc.processInput("/tmp/empty.txt");
		expect(cap.out().empty() && cap.err().empty(), "no output, no error");
		removeFile("/tmp/empty.txt");
	}

	// === 14. processInput: 各エラーの分岐 ===
	section("14. processInput: all error branches");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,1.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");

		// negative
		writeFile("/tmp/tin.txt", "date | value\n2020-01-01 | -5\n");
		{ StreamCapture cap; btc.processInput("/tmp/tin.txt");
		  expect(contains(cap.err(), "not a positive number"), "-5"); }

		// too large
		writeFile("/tmp/tin.txt", "date | value\n2020-01-01 | 1001\n");
		{ StreamCapture cap; btc.processInput("/tmp/tin.txt");
		  expect(contains(cap.err(), "too large a number"), "1001"); }

		// 1000 は境界内
		writeFile("/tmp/tin.txt", "date | value\n2020-01-01 | 1000\n");
		{ StreamCapture cap; btc.processInput("/tmp/tin.txt");
		  expect(!contains(cap.err(), "too large"), "1000 valid"); }

		// bad format (no separator)
		writeFile("/tmp/tin.txt", "date | value\n2020-01-01 100\n");
		{ StreamCapture cap; btc.processInput("/tmp/tin.txt");
		  expect(contains(cap.err(), "bad input"), "no separator"); }

		// bad date
		writeFile("/tmp/tin.txt", "date | value\n1999-13-40 | 5\n");
		{ StreamCapture cap; btc.processInput("/tmp/tin.txt");
		  expect(contains(cap.err(), "bad input"), "bad date"); }

		// bad value
		writeFile("/tmp/tin.txt", "date | value\n2020-01-01 | abc\n");
		{ StreamCapture cap; btc.processInput("/tmp/tin.txt");
		  expect(contains(cap.err(), "bad input"), "bad value"); }

		removeFile("/tmp/tin.txt");
		removeFile("/tmp/mock_db.csv");
	}

	// === 15. OCF: copy ctor / operator= / self-assign ===
	section("15. OCF");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,100.0\n");
		BitcoinExchange a;
		a.loadDatabase("/tmp/mock_db.csv");
		BitcoinExchange b(a);
		expect(b.hasDatabase() && b.getRate("2020-01-01") == 100.0, "copy ctor");
		BitcoinExchange c;
		c = a;
		expect(c.getRate("2020-01-01") == 100.0, "operator=");
		c = c;
		expect(c.getRate("2020-01-01") == 100.0, "self-assign safe");
		removeFile("/tmp/mock_db.csv");
	}

	// === 16. Leak safety: 100 processInput cycles ===
	section("16. 100 processInput cycles (leak safety)");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,50.0\n");
		writeFile("/tmp/tin.txt", "date | value\n2020-01-01 | 5\n");
		for (int i = 0; i < 100; ++i) {
			BitcoinExchange btc;
			btc.loadDatabase("/tmp/mock_db.csv");
			StreamCapture cap;
			btc.processInput("/tmp/tin.txt");
		}
		expect(true, "100 cycles no crash");
		removeFile("/tmp/mock_db.csv");
		removeFile("/tmp/tin.txt");
	}

	// === 17. 大量エントリ ===
	section("17. large database (1000+ entries in mock)");
	{
		std::ostringstream oss;
		oss << "date,exchange_rate\n";
		for (int y = 2000; y <= 2019; ++y)
			for (int m = 1; m <= 12; ++m)
				for (int d = 1; d <= 28; d += 2)
					oss << y << "-" << (m < 10 ? "0" : "") << m
					    << "-" << (d < 10 ? "0" : "") << d
					    << "," << (y - 1990 + m * 0.1) << "\n";
		writeFile("/tmp/big_db.csv", oss.str());
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/big_db.csv");
		expect(btc.dbSize() > 3000, "loaded > 3000 entries");
		double r = btc.getRate("2010-06-15");
		expect(r > 0, "closest lookup works on big DB");
		removeFile("/tmp/big_db.csv");
	}

	// === 18. 全月境界日付 (各月の 31日/30日/28日) ===
	section("18. all-months boundary days");
	{
		// 1月31日 有効
		expect(BitcoinExchange::isValidDate("2020-01-31"), "Jan 31 valid");
		// 4月30日 有効, 31 無効
		expect(BitcoinExchange::isValidDate("2020-04-30"), "Apr 30 valid");
		expect(!BitcoinExchange::isValidDate("2020-04-31"), "Apr 31 invalid");
		// 6月30日 有効
		expect(BitcoinExchange::isValidDate("2020-06-30"), "Jun 30 valid");
		expect(!BitcoinExchange::isValidDate("2020-06-31"), "Jun 31 invalid");
		// 9月30日 有効
		expect(BitcoinExchange::isValidDate("2020-09-30"), "Sep 30 valid");
		expect(!BitcoinExchange::isValidDate("2020-09-31"), "Sep 31 invalid");
		// 11月30日 有効
		expect(BitcoinExchange::isValidDate("2020-11-30"), "Nov 30 valid");
		expect(!BitcoinExchange::isValidDate("2020-11-31"), "Nov 31 invalid");
	}

	// === 19. 2月 leap year 完全網羅 ===
	section("19. February leap year comprehensive");
	{
		// 4年周期のうるう年
		expect(BitcoinExchange::isValidDate("2004-02-29"), "2004 leap");
		expect(BitcoinExchange::isValidDate("2008-02-29"), "2008 leap");
		// 100 の倍数は普通の年
		expect(!BitcoinExchange::isValidDate("2100-02-29"), "2100 not leap (100 rule)");
		expect(!BitcoinExchange::isValidDate("2200-02-29"), "2200 not leap");
		// 400 の倍数はうるう年
		expect(BitcoinExchange::isValidDate("2000-02-29"), "2000 leap (400 rule)");
		expect(BitcoinExchange::isValidDate("2400-02-29"), "2400 leap");
		// 通常年
		expect(!BitcoinExchange::isValidDate("2001-02-29"), "2001 not leap");
		expect(!BitcoinExchange::isValidDate("2019-02-29"), "2019 not leap");
	}

	// === 20. year=0 系 ===
	section("20. year edge cases");
	{
		expect(!BitcoinExchange::isValidDate("0000-01-01"), "year 0000 rejected");
		expect(BitcoinExchange::isValidDate("0001-01-01"), "year 0001 valid");
		expect(BitcoinExchange::isValidDate("9999-12-31"), "year 9999 valid");
	}

	// === 21. parseValue: leading zeros / +符号 ===
	section("21. parseValue: leading zeros, plus sign");
	{
		double v;
		expect(BitcoinExchange::parseValue("007", v) && v == 7, "007 parsed as 7");
		expect(BitcoinExchange::parseValue("+42", v) && v == 42, "+42 parsed");
		expect(BitcoinExchange::parseValue("0.5", v) && v == 0.5, "0.5");
		expect(BitcoinExchange::parseValue(".5", v) && v == 0.5, ".5 valid (strtod accepts)");
	}

	// === 22. parseValue: 特殊数値 ===
	section("22. parseValue: special numeric forms");
	{
		double v;
		expect(BitcoinExchange::parseValue("1e-2", v) && v > 0.009 && v < 0.011, "1e-2");
		expect(BitcoinExchange::parseValue("1.5E10", v), "1.5E10 (uppercase E)");
	}

	// === 23. processInput: value == 0 は valid (境界) ===
	section("23. value 0 is valid");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,100.0\n");
		writeFile("/tmp/tin.txt", "date | value\n2020-01-01 | 0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		StreamCapture cap;
		btc.processInput("/tmp/tin.txt");
		expect(contains(cap.out(), "2020-01-01 => 0 = 0"), "0 * 100 = 0");
		removeFile("/tmp/mock_db.csv");
		removeFile("/tmp/tin.txt");
	}

	// === 24. processInput: value = 1000 exact 境界 valid ===
	section("24. value 1000 exact boundary valid");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,2.0\n");
		writeFile("/tmp/tin.txt", "date | value\n2020-01-01 | 1000\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		StreamCapture cap;
		btc.processInput("/tmp/tin.txt");
		expect(contains(cap.out(), "2020-01-01 => 1000 = 2000"), "1000 * 2 = 2000");
		removeFile("/tmp/mock_db.csv");
		removeFile("/tmp/tin.txt");
	}

	// === 25. processInput: 小数値 ===
	section("25. fractional values");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,10.0\n");
		writeFile("/tmp/tin.txt",
			"date | value\n"
			"2020-01-01 | 0.5\n"
			"2020-01-01 | 3.14\n"
			"2020-01-01 | 999.99\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		StreamCapture cap;
		btc.processInput("/tmp/tin.txt");
		std::string out = cap.out();
		expect(contains(out, "0.5"), "0.5 processed");
		expect(contains(out, "3.14"), "3.14 processed");
		expect(contains(out, "999.99"), "999.99 processed");
		removeFile("/tmp/mock_db.csv");
		removeFile("/tmp/tin.txt");
	}

	// === 26. 複数のクエリで DB が変わらない (immutability) ===
	section("26. DB immutable across queries");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,50.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		std::size_t sz0 = btc.dbSize();
		for (int i = 0; i < 100; ++i) (void)btc.getRate("2020-01-01");
		expect(btc.dbSize() == sz0, "size unchanged");
		removeFile("/tmp/mock_db.csv");
	}

	// === 27. Header 判定: "date | value" 完全一致のみ header 扱い ===
	section("27. header detection: 'date | value' only");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,10.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		// header 行がない場合、1行目もデータとして処理される
		writeFile("/tmp/tin.txt", "2020-01-01 | 5\n");
		StreamCapture cap;
		btc.processInput("/tmp/tin.txt");
		expect(contains(cap.out(), "2020-01-01 => 5 = 50"), "1st line as data");
		removeFile("/tmp/mock_db.csv");
		removeFile("/tmp/tin.txt");
	}

	// === 28. 誤ったフォーマット (separator の類似) ===
	section("28. wrong separators");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,10.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		writeFile("/tmp/tin.txt",
			"date | value\n"
			"2020-01-01|5\n"       // 空白なし
			"2020-01-01 |5\n"      // 前空白のみ
			"2020-01-01| 5\n");    // 後空白のみ
		StreamCapture cap;
		btc.processInput("/tmp/tin.txt");
		std::string err = cap.err();
		expect(err.find("bad input") != std::string::npos, "wrong separator format");
		removeFile("/tmp/mock_db.csv");
		removeFile("/tmp/tin.txt");
	}

	// === 29. 極端に近い日付 (連続日) ===
	section("29. consecutive dates in DB");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2020-01-01,1.0\n"
			"2020-01-02,2.0\n"
			"2020-01-03,3.0\n"
			"2020-01-04,4.0\n"
			"2020-01-05,5.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		expect(btc.getRate("2020-01-01") == 1.0, "day 1");
		expect(btc.getRate("2020-01-02") == 2.0, "day 2");
		expect(btc.getRate("2020-01-03") == 3.0, "day 3");
		expect(btc.getRate("2020-01-04") == 4.0, "day 4");
		expect(btc.getRate("2020-01-05") == 5.0, "day 5");
		removeFile("/tmp/mock_db.csv");
	}

	// === 30. 月境界を跨ぐ日付 ===
	section("30. month/year boundary crossing");
	{
		writeFile("/tmp/mock_db.csv",
			"date,exchange_rate\n"
			"2019-12-31,100.0\n"
			"2020-01-01,200.0\n");
		BitcoinExchange btc;
		btc.loadDatabase("/tmp/mock_db.csv");
		expect(btc.getRate("2019-12-31") == 100.0, "year end");
		expect(btc.getRate("2020-01-01") == 200.0, "year start");
		// 中間日はない
		removeFile("/tmp/mock_db.csv");
	}

	// SUMMARY
	std::cout << "\n=====================================\n";
	std::cout << "RESULT: " << g_pass << " passed, " << g_fail << " failed." << std::endl;
	std::cout << "=====================================" << std::endl;
	return g_fail == 0 ? 0 : 1;
}

// ==== main =================================================================

int main(int argc, char** argv) {
	if (argc == 2 && std::string(argv[1]) == "--test") {
		return runTests();
	}
	if (argc != 2) {
		std::cerr << "Error: could not open file." << std::endl;
		return 1;
	}
	BitcoinExchange btc;
	if (!btc.loadDatabase("data.csv")) {
		std::cerr << "Error: could not open file." << std::endl;
		return 1;
	}
	btc.processInput(argv[1]);
	return 0;
}
