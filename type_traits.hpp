#ifndef M_TYPE_TRAITS_HPP
#define M_TYPE_TRAITS_HPP
#include "iterator.hpp"
#include <type_traits>


namespace sx {

template<typename T>
class value_type {
	template<typename U, typename U1 = typename U::value_type>
	static std::true_type match(std::nullptr_t);

	template<typename U>
	static std::false_type match(...);
public:
	using type = std::conditional_t<decltype(match<T>(nullptr))::value,
									typename T::value_type,
									std::remove_pointer_t<T>>;
};

template<typename T>
using value_type_t = typename value_type<T>::type;


template<typename T, bool = std::is_enum<T>::value				||
							std::is_pointer<T>::value			||
							std::is_member_pointer<T>::value	||
							std::is_arithmetic<T>::value		||
							std::is_null_pointer<T>::value>
class has_traivial_destructor {
public:
	using type = std::false_type;
};

template<typename T>
class has_traivial_destructor<T, true> {
public:
	using type = std::true_type;
};

template<>
class has_traivial_destructor<nullptr_t> {
public:
	using type = std::true_type;
};

template<typename T>
using has_traivial_destructor_t = typename has_traivial_destructor<T>::type;


template<typename T>
class has_noexcept_move_construct {
	template<typename U, typename = std::enable_if_t<noexcept(U(std::declval<U>()))>>
	static std::true_type match(std::nullptr_t);

	template<typename U>
	static std::false_type match(...);
public:
	using type = decltype(match<T>(nullptr));
};


template<typename T>
using has_noexcept_move_construct_t = typename has_noexcept_move_construct<T>::type;


/* 转换分配器的类型 */
template<typename T, typename R,
		 template<typename, typename...> class Alloc,
		 typename... Args>
Alloc<R, Args...> transform_alloator_type(Alloc<T, Args...>);


template<typename T, typename R,
		 template<typename> class Alloc>
Alloc<R> transform_alloator_type(Alloc<T>);



template<typename T>
struct identity {
	using type = T;
	type const & operator()(type const &val) const noexcept {
		return val;
	}
};

template<typename T>
struct __is_pointer_iterator {
	using type = std::conditional_t<std::is_pointer_v<T>, std::true_type, std::false_type>;
};

template<typename T>
using __is_pointer_iterator_t = typename __is_pointer_iterator<T>::type;

template<typename T>
struct __is_class_iterator {
	using type = std::conditional_t<std::is_class_v<T>, std::true_type, std::false_type>;
};

template<typename T>
using __is_class_iterator_t = typename __is_class_iterator<T>::type;

template<typename Iterator>
struct __has_iterator_tag {
	template<typename U, typename = typename U::iterator_category>
	static auto match(std::nullptr_t)->std::true_type;

	template<typename U>
	static auto match(...)->std::false_type;
public:
	using type = decltype(match<Iterator>(nullptr));
};

template<typename Iterator>
using has_iterator_tag_t = typename __has_iterator_tag<Iterator>::type;


template<typename Iterator>
struct is_iterator {
	using type = std::conditional_t<__is_pointer_iterator_t<Iterator>::value, 
					std::true_type,
					std::conditional_t<__is_class_iterator_t<Iterator>::value,
						has_iterator_tag_t<Iterator>,
						std::false_type>>;
};

template<typename Iterator>
using is_iterator_t = typename is_iterator<Iterator>::type;

template<typename Iterator>
constexpr static bool is_iterator_v = is_iterator_t<Iterator>::value;


template<typename Iterator, typename IteratorCategory>
struct __is_same_iterator_tag {
	using type = std::is_base_of<IteratorCategory, typename Iterator::iterator_category>;
};


template<typename Iterator, typename IteratorCategory>
struct __is_iterator_type {
	using type = typename std::conditional_t<std::is_pointer_v<Iterator>, 
												sx::identity<std::true_type>,
												std::conditional_t<sx::is_iterator_v<Iterator>,
													sx::__is_same_iterator_tag<Iterator, IteratorCategory>,
													sx::identity<std::false_type>>>::type;
};


template<typename Iterator>
using is_random_iterator = __is_iterator_type<Iterator, sx::random_access_iterator_tag>;

template<typename Iterator>
using is_random_iterator_t = typename is_random_iterator<Iterator>::type;


template<typename Iterator>
static constexpr bool is_random_iterator_v = is_random_iterator_t<Iterator>::value;


template<typename Iterator>
using is_input_iterator = __is_iterator_type<Iterator, sx::input_iterator_tag>;

template<typename Iterator>
using is_input_iterator_t = typename is_input_iterator<Iterator>::type;

template<typename Iterator>
static constexpr bool is_input_iterator_v = is_input_iterator_t<Iterator>::value;


template<typename Iterator>
using is_output_iterator = __is_iterator_type<Iterator, sx::output_iterator_tag>;

template<typename Iterator>
using is_output_iterator_t = typename is_output_iterator<Iterator>::type;

template<typename Iterator>
static constexpr bool is_output_iterator_v = is_output_iterator_t<Iterator>::value;


template<typename Iterator>
using is_forward_iterator = __is_iterator_type<Iterator, sx::forward_iteratpr_tag>;


template<typename Iterator>
using is_forward_iterator_t = typename is_forward_iterator<Iterator>::type;

template<typename Iterator>
static constexpr bool is_forward_iterator_v = is_forward_iterator_t<Iterator>::value;

template<typename Iterator>
using is_bidirectional_iterator = __is_iterator_type<Iterator, sx::forward_iteratpr_tag>;


template<typename Iterator>
using is_bidirectional_iterator_t = typename is_bidirectional_iterator<Iterator>::type;

template<typename Iterator>
static constexpr bool is_bidirectional_iterator_v = is_bidirectional_iterator_t<Iterator>::value;



template<typename Iter, typename To>
class is_convertible_iter_type {
	static void aux(To);

	template<typename I, typename T,
		typename = decltype(aux(std::declval<decltype(*std::declval<I>())>()))>
	static auto match(nullptr_t) -> std::true_type;

	template<typename, typename>
	static auto match(...) -> std::false_type;
public:
	using type = decltype(match<Iter, To>(nullptr));
};

template<typename Iter, typename To>
using is_convertible_iter_type_t = typename is_convertible_iter_type<Iter, To>::type;

template<typename Iter, typename To>
static constexpr bool is_convertible_iter_type_v = is_convertible_iter_type_t<Iter, To>::value;

} 	// !nampscace sx

#endif