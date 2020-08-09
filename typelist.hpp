#ifndef M_TYPE_LIST_HPP
#include "type_traits.hpp"
#include <cstdint>

namespace sx {

template<typename...>
struct type_list;

/* -----------------------------------	is_type_list  --------------------------------------------- */

template<typename X>
struct is_type_list : public std::false_type {
};

template<typename... Args>
struct is_type_list<type_list<Args...>> : public std::true_type {
};

template<typename X>
static constexpr bool is_type_list_v = is_type_list<X>::value;

/* -----------------------------------	is_type_list  --------------------------------------------- */


/* -----------------------------------	empty  --------------------------------------------- */

template<typename X, bool = is_type_list_v<X>>
struct type_list_empty;

template<typename... Args>
struct type_list_empty<type_list<Args...>, true> {
	static constexpr bool value = sizeof...(Args) == 0;
};

template<typename X>
static constexpr bool type_list_empty_v = type_list_empty<C>::value;

/* -----------------------------------	empty  --------------------------------------------- */

/* -----------------------------------	size  --------------------------------------------- */

template<typename X, bool = sx::is_type_list_v<X>>
struct type_list_size;

template<typename... Args>
struct type_list_size<type_list<Args...>, true> {
	static constexpr std::size_t value = sizeof...(Args);
};

template<typename X>
static constexpr std::size_t type_list_size_v = type_list_size<X>::value;

/* -----------------------------------	size  --------------------------------------------- */

/* -----------------------------------	front  --------------------------------------------- */

template<typename X, bool = sx::type_list_empty_v<X>>
struct type_list_front;

template<typename Head, typename... Args>
struct type_list_front<type_list<Head, Args...>, false> {
	using type = Head;
};

template<typename X>
using type_list_front_t = typename type_list_front<X>::type;

/* -----------------------------------	front  --------------------------------------------- */

/* -----------------------------------	push_front  --------------------------------------------- */

template<typename X, typename Element, bool = sx::is_type_list_v<X>>
struct type_list_push_front;

template<typename... Args, typename Element>
struct type_list_push_front<type_list<Args...>, Element, true> {
	using type = type_list<Element, Args...>;
};

template<typename X, typename Element>
using type_list_push_front_t = typename type_list_push_front<X, Element>::type;

/* -----------------------------------	push_front  --------------------------------------------- */


/* -----------------------------------	pop_front  --------------------------------------------- */

template<typename X, bool = sx::type_list_empty_v<X>>
struct type_list_pop_front;

template<typename Head, typename... Args>
struct type_list_pop_front<type_list<Head, Args...>, false> {
	using type = type_list<Args...>;
};

template<typename X>
using type_list_pop_front_t = typename type_list_pop_front<X>::type;

/* -----------------------------------	pop_front  --------------------------------------------- */

/* -----------------------------------	back  --------------------------------------------- */

template<typename X, bool = sx::type_list_empty_v<X>>
struct type_list_back;

template<typename X>
struct type_list_back<X, false> : type_list_back<type_list_pop_front_t<X>> {
};

template<typename Head>
struct type_list_back<type_list<Head>, false> {
	using type = Head;
};


template<typename X>
using type_list_back_t = typename type_list_back<X>::type;

/* -----------------------------------	back  --------------------------------------------- */
/* -----------------------------------	pop_back  --------------------------------------------- */

template<typename X, bool = sx::type_list_empty_v<X>>
struct type_list_pop_back;

template<typename X>
struct type_list_pop_back<X, false> {
	using type = 
};

template<typename Head>
struct type_list_pop_back<type_list<Head>, false> {
	using type = type_list<>;
};


template<typename X>
using type_list_back_t = typename type_list_back<X>::type;

/* -----------------------------------	pop_back  --------------------------------------------- */


}

#endif // !M_TYPE_LIST_HPP

