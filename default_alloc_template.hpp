#ifndef DEFAULT_ALLOC_TEMPLATE_HPP
#define DEFAULT_ALLOC_TEMPLATE_HPP
#include <cstddef>
#include <cstdlib>
#include "malloc_alloc_template.hpp"

namespace sx {

constexpr std::size_t ALIGN = 8;
constexpr std::size_t MAX_BYTES = 128;
constexpr std::size_t NFREELISTS = MAX_BYTES / ALIGN;

template<bool threads> class __default_alloc_template;

union __Obj {
	union __Obj 	*free_list_link;
	char 		 char_client_data[1];
};


template<bool threads>
class __default_alloc_template {
private:
	static __Obj  		* volatile free_list[NFREELISTS];	/* 空闲链表数组 */
	static char 		*start_free;						/* 内存池开始位置 */
	static char 		*end_free;							/* 内存池结束位置 */
	static std::size_t	 heap_size;							/* 空闲块数量 */
private:
	/* 根据字节数获得 free_list 数组中的下标 */
	static std::ptrdiff_t FOUND_INDEX(std::size_t bytes) {
		return ((bytes + ALIGN-1) / ALIGN) - 1;
	}

	/* 将 bytes 上调置 8 字节对齐 */
	static std::size_t ROUND_UP(std::size_t bytes) {
		return (bytes + ALIGN - 1) & ~(ALIGN - 1);
	}

	/* 分配空闲链表 */
	static void *refill(std::size_t bytes) {
		int nobjs = 20;
		char *chunk = chunk_alloc(bytes, nobjs);
		void *result = static_cast<void *>(chunk);

		if (nobjs > 1) {
			__Obj * volatile *my_free_list = free_list + FOUND_INDEX(bytes);
			__Obj *cur_obj = reinterpret_cast<__Obj *>(chunk + bytes);
			__Obj *start_obj = cur_obj;
			__Obj *next_obj;
			for (int i = 1; i < nobjs - 1; ++i) {
				next_obj = (__Obj *)(((char *)cur_obj) + bytes);
				cur_obj->free_list_link = next_obj;
				cur_obj = next_obj;
			}
			cur_obj->free_list_link = *my_free_list;
			*my_free_list = start_obj;
		}
		return result;
	}

	/* 填装内存池 */
	static char *chunk_alloc(std::size_t bytes, int &nobjs) {
		std::size_t total_size = bytes * nobjs;
		std::size_t byte_left  = end_free - start_free;
		char *result = start_free;

		/* 内存池中拥有足够空间 */
		if (byte_left >= total_size) {
			start_free += total_size;
			return result;

		/* 内存池中还足够一块内存 */
		} else if (byte_left >= bytes) {
			start_free += bytes;
			nobjs = 1;
			return result;

		/* 不够一块内存 */
		} else {
			std::size_t byte_to_get = 2 * total_size; //+ (heap_size >> 4);

			/* 如果内存池中还有剩余空间则加入到空闲链表中 */
			if (byte_left > 0) {
				__Obj * volatile *my_free_list = free_list + FOUND_INDEX(byte_left);
				__Obj *obj_ptr = reinterpret_cast<__Obj *>(start_free);
				obj_ptr->free_list_link = *my_free_list;
				*my_free_list = obj_ptr;
			}

			start_free = static_cast<char *>(std::malloc(byte_to_get));
			if (start_free == nullptr) {
				__Obj *volatile *my_free_list;
				__Obj *ptr;
				for (std::size_t i = bytes; i < MAX_BYTES; i += ALIGN) {
					my_free_list = free_list + FOUND_INDEX(i);
					ptr = *my_free_list;
					if (ptr != nullptr) {
						*my_free_list = ptr->free_list_link;
						start_free = reinterpret_cast<char *>(ptr);
						end_free = start_free + i;
						return chunk_alloc(bytes, nobjs);
					}
				}

				start_free = nullptr;
				start_free = static_cast<char *>(malloc_alloc::allocate(byte_to_get));
			}

			end_free = start_free + byte_to_get;
			heap_size += byte_to_get / bytes;
			return chunk_alloc(bytes, nobjs);
		} 
	}
public:
	static void *allocate(std::size_t bytes) {
		if (bytes > MAX_BYTES)
			return malloc_alloc::allocate(bytes);
		
		__Obj * volatile *my_free_list = free_list + FOUND_INDEX(bytes);
		__Obj *result = *my_free_list;
		if (result == nullptr)
			return refill(ROUND_UP(bytes));

		*my_free_list = result->free_list_link;
		return reinterpret_cast<void *>(result);
	}

	static void deallocate(void *ptr, std::size_t bytes) {
		if (ptr == nullptr)
			return;

		if (bytes > MAX_BYTES) {
			malloc_alloc::deallocate(ptr, bytes);
			return;
		}

		__Obj *volatile *my_free_list = free_list + FOUND_INDEX(bytes);
		__Obj *obj_ptr = reinterpret_cast<__Obj *>(ptr);
		obj_ptr->free_list_link = *my_free_list;
		*my_free_list = obj_ptr;
	}
};


template<bool threads>
__Obj * volatile __default_alloc_template<threads>::free_list[NFREELISTS] = { nullptr };

template<bool threads>
char *__default_alloc_template<threads>::start_free = nullptr;

template<bool threads>
char *__default_alloc_template<threads>::end_free = nullptr;

template<bool threads>
std::size_t __default_alloc_template<threads>::heap_size = 0;

} //namespace sx
#endif



