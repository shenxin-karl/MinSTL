#ifndef MALLOC_ALLOC_TEMPLATE_HPP
#define MALLOC_ALLOC_TEMPLATE_HPP
#include <cstdlib>
#include <cstddef>
#include <exception>

namespace sx {

	class BadAlloca : public std::exception {
	};

	template<int inst>
	class malloc_alloc_template {
	private:
		static void *oom_malloc(std::size_t);
		static void *oom_realloc(void *, std::size_t);
		static void(*malloc_alloc_oom_handler)();          /* 内存分配失败处理程序指针 */
	public:
		static void *allocate(std::size_t n) {
			void *result = malloc(n);
			if (result == nullptr)
				result = oom_malloc(n);

			return result;
		}

		static void deallocate(void *ptr, std::size_t) {
			free(ptr);
		}

		static void *reallocate(void *ptr, std::size_t/* old_size */, std::size_t new_sz) {
			void *result = realloc(ptr, new_sz);
			if (result == nullptr)
				result = oom_realloc(ptr, new_sz);

			return result;
		}

		static auto set_malloc_handler(void(*handler)()) -> void(*)() {
			void(*old)() = malloc_alloc_oom_handler;
			malloc_alloc_oom_handler = handler;
			return old;
		}
	};

	void(*malloc_alloc_template<0>::malloc_alloc_oom_handler)() = nullptr;

	template<int inst>
	void *malloc_alloc_template<inst>::oom_malloc(std::size_t n)
	{
		void(*my_malloc_handler)();
		void *result;

		for (; ; ) {
			my_malloc_handler = malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr)
				throw BadAlloca();

			my_malloc_handler();
			result = malloc(n);
			if (result)
				return result;
		}
	}

	template<int inst>
	void *malloc_alloc_template<inst>::oom_realloc(void *ptr, size_t n)
	{
		void(*my_malloc_handler)();
		void *result;

		for (; ; ) {
			my_malloc_handler = malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr)
				throw BadAlloca();

			my_malloc_handler();
			result = realloc(ptr, n);
			if (result)
				return result;
		}
	}

	using malloc_alloc = malloc_alloc_template<0>;
}

#endif