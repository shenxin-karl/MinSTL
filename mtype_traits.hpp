#ifndef M_TYPE_TRAITS_HPP
#define M_TYPE_TRAITS_HPP
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


} 	// !nampscace sx

#endif