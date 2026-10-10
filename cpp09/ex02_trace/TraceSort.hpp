#ifndef TRACESORT_HPP
#define TRACESORT_HPP

#include <vector>
#include <string>
#include <cstddef>

// TraceSort: ex02 の Ford-Johnson を「教育用」に step-by-step 出力するバージョン
// - 再帰の深さで indent (2 空白 x depth)
// - 各ステップ (pair up / recurse / main chain / jacobsthal / insert / final)
//   を [depth N] のラベル付きで printed
// - ex02 (実装本体) とロジックは同じ (添字で持ち回る)、差分は「トレース出力」と比較回数の表示
class TraceSort {
private:
	static std::size_t	_comparisons;

	static bool			isLess(int a, int b);
	static std::string	toString(const std::vector<int>& values,
							const std::vector<std::size_t>& idx);
	static std::string	indent(int depth);
	static void			sortIdx(const std::vector<int>& values,
							std::vector<std::size_t>& idx, int depth);

public:
	// 唯一のエントリポイント
	static void			sort(std::vector<int>& v);
	static std::size_t	comparisons();
};

#endif
