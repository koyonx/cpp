#include "RPN.hpp"
#include <sstream>
#include <cctype>

RPN::RPN() : _stack() {}
RPN::RPN(const RPN& o) : _stack(o._stack) {}
RPN& RPN::operator=(const RPN& o) {
	if (this != &o) _stack = o._stack;
	return *this;
}
RPN::~RPN() {}

// 演算子適用: RPN では「a b op」の順にトークンが並ぶ
// stack: push a, push b, op → pop b, pop a, compute (a op b)
bool RPN::applyOperator(char op) {
	if (_stack.size() < 2) return false;
	int b = _stack.top(); _stack.pop();
	int a = _stack.top(); _stack.pop();
	int result;
	switch (op) {
		case '+': result = a + b; break;
		case '-': result = a - b; break;
		case '*': result = a * b; break;
		case '/':
			if (b == 0) return false;  // 0除算
			result = a / b;
			break;
		default: return false;
	}
	_stack.push(result);
	return true;
}

// expression を空白区切りで token 化して RPN 評価
int RPN::evaluate(const std::string& expression) {
	// evaluate は state を保持しない(呼び出しごとにクリア)
	while (!_stack.empty()) _stack.pop();

	std::istringstream iss(expression);
	std::string token;

	while (iss >> token) {
		if (token.length() == 1 &&
			(token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/')) {
			if (!applyOperator(token[0])) throw RPNException();
		} else if (token.length() == 1 && std::isdigit(static_cast<unsigned char>(token[0]))) {
			// PDF: 数字は 10 未満なので 1 桁のみ
			_stack.push(token[0] - '0');
		} else {
			throw RPNException();
		}
	}

	if (_stack.size() != 1) throw RPNException();
	return _stack.top();
}

std::size_t RPN::stackSize() const { return _stack.size(); }
int RPN::topOfStack() const { return _stack.top(); }

const char* RPN::RPNException::what() const throw() {
	return "Error";
}
