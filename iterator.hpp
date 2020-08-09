#ifndef M_ITERATOR_HPP
#define M_ITERATOR_HPP
#include <cstddef>
#include <type_traits>
#include <iostream>
#include "type_traits.hpp"

namespace sx {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iteratpr_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iteratpr_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};


template<typename Category,  typename T, typename Distance = std::ptrdiff_t,
			typename Pointer = T*, typename Reference = T&>
struct iterator {
	using iterator_category = Category;
	using value_type 		= T;
	using difference_type 	= Distance;
	using pointer 			= Pointer;
	using reference 		= Reference;
	using const_pointer		= T const *;
	using const_reference   = T const &;
};

template<typename Iterator>
struct iterator_traits {
	using iterator_category = typename Iterator::iterator_category;
	using value_type = typename Iterator::value_type;
	using difference_type = typename Iterator::difference_type;
	using pointer = typename Iterator::pointer;
	using reference = typename Iterator::reference;
};

template<typename T>
struct iterator_traits<T *> {
	using iterator_category = random_access_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T * ;
	using reference = T &;
};

template<typename T>
struct iterator_traits<T const *> {
	using iterator_category = random_access_iterator_tag;
	using value_type = const T;
	using difference_type = std::ptrdiff_t;
	using pointer = T const * ;
	using reference = T const & ;
};

template<typename Iterator> inline
typename iterator_traits<Iterator>::iterator_category
iterator_category(Iterator const &);

template<typename Iterator> inline
typename iterator_traits<Iterator>::diffeence_type
distance_type(Iterator const &);

template<typename Iterator> inline
typename iterator_traits<Iterator>::value_type
iterator_value_type(Iterator const &);

template<typename InputIterator> inline
typename iterator_traits<InputIterator>::difference_type
__distance_aux(InputIterator first, InputIterator last, input_iterator_tag)
{
	using diff_t = typename iterator_traits<InputIterator>::difference_type;
	diff_t n = 0;
	while (first != last) {
		++n;
		++first;
	}

	return n;
}

template<typename RandomAccessIterator> inline
typename iterator_traits<RandomAccessIterator>::difference_type
__distance_aux(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
{
	return last - first;
}

template<typename Iterator> inline
typename iterator_traits<Iterator>::difference_type
distance(Iterator first, Iterator end) {
	using category = typename iterator_traits<Iterator>::iterator_category;
	return __distance_aux(first, end, category{});
}

template<typename InputIterator> inline
void __advance_aux(InputIterator &iter, int n, input_iterator_tag)
{
	while (n--)
		++iter;
}

template<typename BidirectionalIterator> inline 
void __advance_aux(BidirectionalIterator &iter, int n, bidirectional_iterator_tag)
{
	if (n > 0) {
		while (n--)
			++iter;
	} else {
		while (++n)
			--iter;
	}
}

template<typename RandomIterator> inline 
void advance_aux(RandomIterator &iter, int n, random_access_iterator_tag)
{
	iter += n;
}

template<typename Iterator> inline 
void advance(Iterator &iter, int n)
{
	using category = typename iterator_traits<Iterator>::iterator_category;
	__advance_aux(iter, n, category{});
}


#if 0
template<typename Iterator, 
		 typename = std::enable_if_t<sx::is_iterator_v<Iterator>>>
class move_iterator {
	Iterator iterator;
public:
	move_iterator(Iterator iter) : iterator(iter) { }
	move_iterator(move_iterator const &other) = default;
	move_iterator &operator=(move_iterator const &other) = default;
	move_iterator() = delete;
	~move_iterator() = default;

	template<typename = std::enable_if_t<decltype(Iterator()++, std::true_type)>>
	move_iterator operator++(int) {
		return move_iterator(iterator++);
	}

	template<typename = std::enable_if_t<decltype(++Iterator(), std::true_type)>>
	move_iterator &operator++() {
		return move_iterator(++iterator);
	}

	template<typename = std::enable_if_t<decltype(Iterator()++, std::true_type::value)>>
	move_iterator &operator--(int) {
		return move_iterator(iterator--);
	}

	template<typename = std::enable_if_t<decltype(Iterator()++, std::true_type::value)>>
	move_iterator operator--() {
		return move_iterator(--iterator);
	}

	template<typename = std::enable_if_t<decltype(Iterator() += 5, std::true_type::value)>>
	move_iterator &operator+=(int n) {
		return move_iterator(iterator += n);
	}

	template<typename = std::enable_if_t<decltype(Iterator() -= 5, std::true_type::value)>>
	move_iterator &operator-=(int n) {
		return move_iterator(iterator -= n);
	}

	template<typename = std::enable_if_t<decltype(Iterator() + 5, std::true_type::value)>>
	move_iterator operator+(int n) {
		return move_iterator(iterator + n);
	}

	template<typename = std::enable_if_t<decltype(Iterator() - 5, std::true_type::value)>>
	move_iterator operator-(int n) {
		return move_iterator(iterator - n);
	}

	template<typename = std::enable_if_t<decltype(std::declval<Iterator>().operator*(), std::true_type::value)>>
	typename iterator_traits<Iterator>::value_type &&
	operator*() {
		using move_type = typename iterator_traits<Iterator>::value_type &&;
		return static_cast<move_type>(*iterator);
	}

	template<typename = std::enable_if_t<decltype(std::declval<Iterator>()[1], std::true_type::value)>>
	typename iterator_traits<Iterator>::value_type &&
	operator[](int n) {
		using move_type = typename iterator_traits<Iterator>::value_type &&;
		return static_cast<move_type>(iterator[n]);
	}

	template<typename = std::enable_if_t<decltype(std::declval<Iterator>().operator->(), std::true_type::value)>>
	typename iterator_traits<Iterator>::value_type *
	operator->() {
		return &(*iterator);
	}
};
#endif


template<typename T, std::size_t N> constexpr
T *begin(T (&arr)[N]) {
	return arr;
}

template<typename T, std::size_t N> constexpr
T *end(T (&arr)[N]) {
	return arr + N;
}


template<typename Iterator>
class __reverse_iterator;

template<typename Iterator>
class __reverse_iterator {
	using self 				= __reverse_iterator<Iterator>;
public:
	using iterator_category = typename iterator_traits<Iterator>::iterator_category;
	using pointer 			= typename iterator_traits<Iterator>::pointer;
	using reference 		= typename iterator_traits<Iterator>::reference;
	using difference_type 	= typename iterator_traits<Iterator>::difference_type;
private:
	Iterator  curr;
public:
	__reverse_iterator() {}
	explicit __reverse_iterator(Iterator iter) : curr(iter) {}
	__reverse_iterator(__reverse_iterator const &) = default;
	__reverse_iterator(__reverse_iterator &&) = default;
	__reverse_iterator &operator=(__reverse_iterator const &) = default;
	__reverse_iterator &operator=(__reverse_iterator &&) = default;
	~__reverse_iterator() = default;
public:
	reference operator*() {
		Iterator tmp = curr;
		return *(--tmp);
	}

	pointer operator->() {
		return &(this->operator*());
	}

	self &operator++() {
		--curr;
		return *this;
	}

	self operator++(int) {
		self tmp = curr;
		--curr;
		return tmp;
	}

	self &operator--() {
		++curr;
		return *this;
	}

	self operator--(int) {
		self tmp = curr;
		++curr;
		return tmp;
	}

	self operator+(int n) {
		self tmp = curr;
		return (tmp -= n);
	}

	self &operator+=(int n) {
		return *(*this -= n);
	}

	self operator-(int n) {
		self tmp = curr;
		return (tmp -= n);
	}

	self &operator-=(int n) {
		return *(*this += n);
	}

	template<typename = std::enable_if_t<sx::has_operator_equal_v<Iterator>>>
	friend bool operator==(self const &first, self const &second) noexcept {
		return first.curr == second.curr;
	}

	template<typename = std::enable_if_t<sx::has_operator_not_equal_v<Iterator>>>
	friend bool operator!=(self const &first, self const &second) noexcept {
		return !(first == second);
	}
};

}






#endif
