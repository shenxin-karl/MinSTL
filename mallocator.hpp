#ifndef ALLOCA_HPP
#define ALLOCA_HPP
#include <cstddef>
#include <utility>
#include "construct.hpp"
#include "default_alloc_template.hpp"


namespace sx {
	template<typename T>
	class allocator {
		using alloc_template = default_alloc_template<false, 0>;
	public:
		static T *allocate() {
			return reinterpret_cast<T *>(alloc_template::allocate(sizeof(T)));
		}

		static T *allocate(std::size_t n) {
			return reinterpret_cast<T *>(alloc_template::allocate(sizeof(T) * n));
		}

		static void deallocate(T *ptr) {
			return alloc_template::deallocate(static_cast<void *>(ptr), sizeof(T));
		}

		static void deallocate(T *ptr, std::size_t n) {
			return alloc_template::deallocate(static_cast<void *>(ptr), n);
		}

		template<typename... Args>
		static void construct(T *ptr, Args&&... args) {
			sx::construct(ptr, std::forward<Args>(args)...);
		}

		template<typename T>
		static void destroy(T *ptr) {
			sx::destroy(ptr);
		}

		template<typename ForwardIter>
		static void destroy(ForwardIter first, ForwardIter last) {
			sx::destroy(first, last);
		}
	};
}

#endif