#ifndef M_ALGORITHM_HPP
#define M_ALGORITHM_HPP

namespace sx {

template<typename BidirIterator1, typename BidirIterator2>
BidirIterator2 copy_backward(BidirIterator1 first, BidirIterator1 end, BidirIterator2 result) {
	while (first != end)
		*(--result) = *(--end);

	return result;
}

template<typename InputIterator1, typename InputIterator2>
InputIterator2 copy(InputIterator1 first, InputIterator1 end, InputIterator2 result) {
	for (; first != end; ++first, ++result)
		*first = *result;

	return result;
}

template<typename InputIterator, typename Size, typename Value>
InputIterator fill_n(InputIterator first, Size size, Value const &value) {
	for (Size index = 0; index < size; ++index, ++first)
		*first = value;

	return first;
}

};

#endif
