#ifndef UTILS_HH
#define UTILS_HH

# include <cstddef>

template <typename T, size_t N>
constexpr auto array_size(T (&arr)[N]) {
	(void) arr;
	return N;
}

#endif /* !UTILS_HH */
