#ifndef ITER_HPP
#define ITER_HPP

#include <cstddef>

// T が const 型として推論されると T* は const T*, T& は const T& になる
// これにより同じ template で const/非const array と const/非const 引数関数の両方に対応
template <typename T, typename F>
void iter(T* array, size_t const length, F func) {
	for (size_t i = 0; i < length; ++i)
		func(array[i]);
}

#endif
