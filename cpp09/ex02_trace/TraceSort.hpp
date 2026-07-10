#ifndef TRACESORT_HPP
#define TRACESORT_HPP

#include <vector>
#include <string>
#include <cstddef>

// TraceSort: ex02 の Ford-Johnson を「教育用」に step-by-step 出力するバージョン
// - 再帰の深さで indent (2 空白 x depth)
// - 各ステップ (pair up / recurse / reorder / prepend / jacobsthal / insert / straggler / final)
//   を [depth N] のラベル付きで printed
// - ex02 (実装本体) とロジックは同じ、差分は「トレース出力」だけ
class TraceSort {
private:
	// helper: ベクトルを "[a, b, c]" 形式で print
	static std::string	toString(const std::vector<int>& v);
	static std::string	pairsToString(const std::vector<std::pair<int, int> >& p);
	static std::string	indent(int depth);
	static void		binaryInsertWithTrace(std::vector<int>& S, int val, int depth);

public:
	// 唯一のエントリポイント
	static void	sort(std::vector<int>& v, int depth = 0);
};

#endif
