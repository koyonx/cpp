#include "Span.hpp"
#include <climits>

Span::Span() : _capacity(0), _data() {}

Span::Span(unsigned int n) : _capacity(n), _data() {
	_data.reserve(n);
}

Span::Span(const Span& other) : _capacity(other._capacity), _data(other._data) {}

Span& Span::operator=(const Span& other) {
	if (this != &other) {
		_capacity = other._capacity;
		_data = other._data;
	}
	return *this;
}

Span::~Span() {}

void Span::addNumber(int value) {
	if (_data.size() >= _capacity)
		throw SpanFullException();
	_data.push_back(value);
}

int Span::shortestSpan() const {
	if (_data.size() < 2)
		throw NoSpanException();
	std::vector<int> sorted(_data);
	std::sort(sorted.begin(), sorted.end());
	// 隣接ペアの min diff. long でオーバーフロー回避
	long shortest = static_cast<long>(sorted[1]) - static_cast<long>(sorted[0]);
	for (size_t i = 2; i < sorted.size(); ++i) {
		long diff = static_cast<long>(sorted[i]) - static_cast<long>(sorted[i - 1]);
		if (diff < shortest) shortest = diff;
	}
	// clamp INT_MAX
	if (shortest > INT_MAX) shortest = INT_MAX;
	return static_cast<int>(shortest);
}

int Span::longestSpan() const {
	if (_data.size() < 2)
		throw NoSpanException();
	std::vector<int>::const_iterator mn = std::min_element(_data.begin(), _data.end());
	std::vector<int>::const_iterator mx = std::max_element(_data.begin(), _data.end());
	long diff = static_cast<long>(*mx) - static_cast<long>(*mn);
	if (diff > INT_MAX) diff = INT_MAX;
	return static_cast<int>(diff);
}

unsigned int Span::size() const { return static_cast<unsigned int>(_data.size()); }
unsigned int Span::capacity() const { return _capacity; }

const char* Span::SpanFullException::what() const throw() {
	return "Span: capacity full";
}
const char* Span::NoSpanException::what() const throw() {
	return "Span: not enough numbers to compute span";
}
