#ifndef RPN_HPP
#define RPN_HPP

#include <string>
#include <stack>
#include <list>

// PDF ルール: ex00 で使った std::map は使用禁止。
// ex01 では std::stack を使用 (RPN 評価にネイティブに合う LIFO 構造)
// std::stack のデフォルトの中身は std::deque で、ex02 で deque を使うため
// 「同じコンテナでは?」と言われないよう中身を std::list に明示している
class RPN {
private:
	std::stack<int, std::list<int> >	_stack;

	// 演算子適用 (b op a) の順に注意: RPN では 「a b op」 なので pop 順は b, a
	bool	applyOperator(char op);

public:
	RPN();
	RPN(const RPN& other);
	RPN& operator=(const RPN& other);
	~RPN();

	// expression を parse & 評価。エラーなら例外を投げる (main で catch)
	int		evaluate(const std::string& expression);

	// テスト用ヘルパ
	std::size_t	stackSize() const;
	int			topOfStack() const;

	// エラー例外クラス
	class RPNException : public std::exception {
	public:
		virtual const char* what() const throw();
	};
};

#endif
