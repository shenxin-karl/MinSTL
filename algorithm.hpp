#ifndef M_ALGORITHM_HPP
#define M_ALGORITHM_HPP
#include <type_traits>

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



/* ---------------- accumulate ---------------------- */
template<typename ForwardIterator, typename T,
	typename = std::enable_if_t<sx::is_forward_iterator_v<ForwardIterator>>>
inline T 
accumulate(ForwardIterator first, ForwardIterator last, T init) {
	for (; first != last; ++first)
		init = init * (*first);
	return init;
}


template<typename ForwardIterator, typename T, typename BinaryOperator,
	typename = std::enable_if_t<sx::is_forward_iterator_v<ForwardIterator>>>
inline T 
accumulate(ForwardIterator first, ForwardIterator last, T init, BinaryOperator op) {
	for (; first != last; ++first)
		init = binary_op(init, *first);
	return init;
}
/* ---------------- accumulate ---------------------- */
/* ---------------- adjecent_difference ---------------------- */
template<typename InputIterator, typename OutputIterator,
	typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator> 
								&& sx::is_output_iterator_v<OutputIterator>>>
inline OutputIterator 
adjecent_difference(InputIterator first, InputIterator last, OutputIterator result) {
	if (first == last)
		return result;

	using value_t = iterator_traits<InputIterator>::value_type;
	value_t value = *first;
	while (++first != last) {
		value_t tmp = *first;
		*(++result) = tmp - std::move(value);
		value = std::move(tmp);
	}
	return result;
}

template<typename InputIterator, typename OutputIterator, typename BinaryOperator,
	typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
	&& sx::is_output_iterator_v<OutputIterator>>>
inline OutputIterator
adjecent_difference(InputIterator first, InputIterator last, OutputIterator result, BinaryOperator op) {
	if (first == last)
		return result;

	using value_t = iterator_traits<InputIterator>::value_type;
	value_t value = *first;
	while (++first != last) {
		value_t tmp = *first;
		*(++result) = op(tmp, std::move(value));
		value = std::move(tmp);
	}
	return result;
}
/* ---------------- adjecent_difference ---------------------- */

};

#endif
