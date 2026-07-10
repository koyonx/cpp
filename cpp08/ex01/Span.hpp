#ifndef SPAN_HPP
#define SPAN_HPP

#include <vector>
#include <exception>
#include <algorithm>
#include <iterator>

class Span {
private:
	unsigned int		_capacity;
	std::vector<int>	_data;

public:
	Span();
	Span(unsigned int n);
	Span(const Span& other);
	Span& operator=(const Span& other);
	~Span();

	void			addNumber(int value);
	int				shortestSpan() const;
	int				longestSpan() const;
	unsigned int	size() const;
	unsigned int	capacity() const;

	// Range-based add (iterator 対応): PDF hint に対応
	template <typename Iter>
	void addNumbers(Iter first, Iter last) {
		typename std::iterator_traits<Iter>::difference_type extra =
			std::distance(first, last);
		if (extra < 0 || _data.size() + static_cast<size_t>(extra) > _capacity)
			throw SpanFullException();
		_data.insert(_data.end(), first, last);
	}

	class SpanFullException : public std::exception {
	public:
		virtual const char* what() const throw();
	};
	class NoSpanException : public std::exception {
	public:
		virtual const char* what() const throw();
	};
};

#endif
