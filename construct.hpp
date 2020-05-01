#ifndef CONSTRUCT_HPP
#define CONSTRUCT_HPP
#include <cstddef>
#include <type_traits>
#include <utility>
#include <new>
#include <cstdlib>
#include <cstring>
#include "mtype_traits.hpp"


namespace sx {

	template<typename T, typename...Args>  	void construct(T *, Args&&... args);
	template<typename T> 			void destroy(T*);
	template<typename ForwardIter>	void destroy_aux(ForwardIter, ForwardIter, std::true_type);
	template<typename ForwardIter>	void destroy_aux(ForwardIter, ForwardIter, std::false_type);
	
	template<typename InputIter, typename ForwardIter>
	ForwardIter uninitialized_copy(InputIter, InputIter, ForwardIter);

	template<typename InputIter, typename ForwardIter>
	ForwardIter uninitialized_copy_aux(InputIter, InputIter, ForwardIter, std::false_type);

	template<typename InputIter, typename ForwardIter>
	ForwardIter uninitialized_copy_aux(InputIter, InputIter, ForwardIter, std::true_type);

	template<typename ForwardIter, typename T>
	void uninitialized_fill(ForwardIter, ForwardIter, T const &);

	template<typename ForwardIter, typename T>
	void uninitialized_fill_aux(ForwardIter, ForwardIter, T const &, std::false_type);

	template<typename ForwardIter, typename T>
	void uninitialized_fill_aux(ForwardIter, ForwardIter, T const &, std::true_type);

	template<typename ForwardIter, typename T>
	void uninitialized_fill_n(ForwardIter, std::size_t, T const &);

	template<typename ForwardIter, typename T>
	void uninitialized_fill_n_aux(ForwardIter, std::size_t, T const &, std::true_type);

	template<typename ForwardIter, typename T>
	void uninitialized_fill_n_aux(ForwardIter, std::size_t, T const &, std::false_type);


	template<typename T, typename... Args> inline
	void construct(T *ptr, Args&&... args)
	{
		new(ptr) T(std::forward<Args>(args)...);
	}

	template<typename T> inline
	void destroy(T *ptr)
	{
		ptr->~T();
	}

	template<typename ForwardIter> inline
	void destroy_aux(ForwardIter first, ForwardIter end, std::true_type)
	{
	}

	template<typename ForwardIter> inline
	void destroy_aux(ForwardIter first, ForwardIter end, std::false_type)
	{
		for (; first != end; ++first)
			destroy(&*first);
	}

	template<typename InputIter, typename ForwardIter> inline
	ForwardIter uninitialized_copy(InputIter first, InputIter end, ForwardIter result)
	{
		using has_traivial = has_traivial_destructor_t<ForwardIter>;
		return uninitialized_copy_aux(first, end, result, has_traivial{});
	}

	template<> inline
	char *uninitialized_copy(char *first, char *last, char *result)
	{
		memmove((void*)result, (void*)first, last - first);
		return result + (last - first);
	}

	template<> inline
	wchar_t *uninitialized_copy(wchar_t *first, wchar_t *last, wchar_t *result)
	{
		memmove((void*)result, (void*)first, last - first);
		return result + (last - first);
	}

	template<typename InputIter, typename ForwardIter> inline
	ForwardIter uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter result, std::false_type)
	{
		InputIter begin = first;
		try {
			for (; first != last; ++first, ++result)
				consruct(&*result, *first);
			return result;
		} catch(...) {
			for (; begin != first; ++first)
				destroy(&*begin);
			throw;
		}
	}

	template<typename InputIter, typename ForwardIter> inline
	ForwardIter uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter result, std::true_type)
	{
		return std::copy(first, last, result);
	}

	template<typename ForwardIter, typename T> inline
	ForwardIter uninitialized_fill(ForwardIter first, ForwardIter last, T const &value)
	{
		using has_traivial = has_traivial_destructor< ForwardIter>;
		return uninitialized_fill_aux(first, last, value, has_traivial{});
	}

	template<typename ForwardIter, typename T> inline
	ForwardIter uninitialized_fill_aux(ForwardIter first, ForwardIter end, T const &value, std::false_type)
	{
		ForwardIter begin = first;
		try {
			for (; first < end; ++first)
				construct(&*first, value);
			return first;
		} catch(...) {
			for (; begin != first; ++begin)
				destroy(&*begin);
			throw;
		}
	}

	template<typename ForwardIter, typename T> inline
	ForwardIter uninitialized_fill_aux(ForwardIter first, ForwardIter last, T const &value, std::true_type)
	{
		return std::fill(first, last, value);
	}

	template<typename ForwardIter, typename T> inline
	ForwardIter uninitialized_fill_n(ForwardIter first, std::size_t n, T const &value)
	{
		using has_traivial = has_traivial_destructor<ForwardIter>;
		return uninitialized_fill_n_aux(first, n, has_traivial{});
	}

	template<typename ForwardIter, typename T> inline
	ForwardIter uninitialized_fill_n_aux(ForwardIter first, std::size_t n, T const &value, std::true_type)
	{
		return std::fill_n(first, n, value);
	}

	template<typename ForwardIter, typename T> inline
	ForwardIter uninitialized_fill_n_aux(ForwardIter first, std::size_t n, T const &value, std::false_type)
	{
		ForwardIter begin = first;
		try {
			for (std::size_t i = 0; i < n; ++i, ++first)
				construct(&*first, value);
			return first;
		} catch(...) {
			for (; begin != first; ++begin)
				destroy(&*begin);
			throw;
		}
	}

	template<typename ForwardIter> inline
	void destroy(ForwardIter first, ForwardIter last)
	{
		using trivial_destructor = typename has_traivial_destructor<ForwardIter>::type;
		destroy_aux(first, last, trivial_destructor{});
	}

	template<> inline
	void destroy(char *, char *)
	{
	}

	template<> inline
	void destroy(wchar_t *, wchar_t *)
	{
	}

}

#endif
