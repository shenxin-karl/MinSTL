#ifndef DEFAULT_ALLOC_TEMPLATE_HPP
#define DEFAULT_ALLOC_TEMPLATE_HPP
#include <cstddef>
#include <cstdlib>
#include "malloc_alloc_template.hpp"

namespace sx {

#if 0
constexpr std::size_t ALIGN = 8;
constexpr std::size_t MAX_BYTES = 128;
constexpr std::size_t NFREELISTS = MAX_BYTES / ALIGN;

template<bool threads, int inst>
class default_alloc_template {
private:
	/* 自由链表结构 */
	union Obj {
		union Obj 	*free_list_link;
		char         char_client_data[1];
	};
private:
	static Obj 			* volatile free_list[NFREELISTS];	/* 自由链表 */
	static char 		*start_free;						/* 内存池起始位置 */
	static char 		*end_free;							/* 内存池结束位置 */
	static std::size_t 	 heap_size;							/* 当前拥有的内存字节 */
private:
	/* 根据 byte 的大小, 获得在 free_list 中的位置 */
	static std::size_t FREELIST_INDEX(std::size_t byte) {
		return ((byte + ALIGN - 1) / ALIGN) - 1;
	}

	static std::size_t ROUND_UP(std::size_t bytes) {
		return (bytes + ALIGN - 1) & (~ALIGN - 1);
	}

	/* 填装 free_list 中的空闲链表 */
	static void *refill(std::size_t n) {
		int nobjs = 20;
		char *chunk = chunk_alloc(n, nobjs);
		Obj *volatile *my_free_list;
		Obj *result;
		Obj *current_obj, *next_obj;

		/* 如果 chunk_alloc 只获得 1 块内存 */
		if (nobjs == 1)
			return chunk;

		/* 否则调整 free_list, 插入新获得的结点 */
		result = reinterpret_cast<Obj*>(chunk);
		my_free_list = free_list + FREELIST_INDEX(n);
		*my_free_list = next_obj = reinterpret_cast<Obj*>(chunk + n);
		for (int i = 1; ; ++i) {
			current_obj = next_obj;
			next_obj = reinterpret_cast<Obj*>(reinterpret_cast<char*>(next_obj) + n);
			if (nobjs - 1 == i) {
				current_obj->free_list_link = nullptr;
				break;
			}
			else
				current_obj->free_list_link = next_obj;
		}
		return result;
	}

	static char *chunk_alloc(std::size_t size, int &nobjs) {
		std::size_t total_size = size * nobjs;
		std::size_t byte_left = end_free - start_free;
		char *result;

		/* 内存池中还拥有足够的内存 */
		if (byte_left >= total_size) {
			result = start_free;
			start_free += total_size;
			return result;

		/* 空间不够需求, 但是还足够一个对象大小 */
		} else if (byte_left >= size) {
			nobjs = byte_left / size;
			total_size = size * nobjs;
			result = start_free;
			start_free += total_size;
			return result;

		/* 内存池中没有足够的空间分配一个对象 */
		} else {
			std::size_t byte_to_get = 2 * total_size + ROUND_UP(heap_size >> 4);

			/* 如果内存池中还有点剩余空间, 则加入到合适的空闲链表 */
			if (byte_left > 0) {
				Obj * volatile *my_free_list = free_list + FREELIST_INDEX(byte_left);
				reinterpret_cast<Obj*>(start_free)->free_list_link = *my_free_list;
				*my_free_list = reinterpret_cast<Obj*>(start_free);
			}

			start_free = static_cast<char *>(malloc(byte_to_get));
			if (start_free == nullptr) {
				Obj * volatile *my_free_list;
				Obj *p;

				for (int i = size; i < MAX_BYTES; i += ALIGN) {
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != nullptr) {
						*my_free_list = p->free_list_link;
						start_free = reinterpret_cast<char *>(p);
						return chunk_alloc(size, nobjs);		/* 再次调整, 返回内存 */
					}
				}

				end_free = nullptr;
				start_free = reinterpret_cast<char *>(malloc_alloc::allocate(byte_to_get));
			}

			heap_size += byte_to_get;
			end_free = start_free + byte_to_get;
			return chunk_alloc(size, nobjs);
		}
	}
public:
	static void *allocate(std::size_t bytes) {
		if (bytes > MAX_BYTES)
			return malloc_alloc::allocate(bytes);

		Obj * volatile *my_free_list;
		Obj * result;

		my_free_list = free_list + FREELIST_INDEX(bytes);
		result = *my_free_list;
		if (result == nullptr) {
			void *r = refill(ROUND_UP(bytes));
			return r;
		}

		*free_list = result->free_list_link;
		return reinterpret_cast<void*>(result);
	}

	static void deallocate(void *ptr, std::size_t bytes) {
		if (bytes > MAX_BYTES) {
			malloc_alloc::deallocate(ptr, bytes);
			return;
		}

		Obj * volatile *my_free_list;
		Obj *p;

		my_free_list = free_list + FREELIST_INDEX(bytes);
		p = reinterpret_cast<Obj*>(ptr);
		p->free_list_link = *my_free_list;
		*my_free_list = p;
	}
};

template<bool threads, int inst>
typename default_alloc_template<threads, inst>::Obj * volatile 
default_alloc_template<threads, inst>::free_list[NFREELISTS] = { nullptr };

template<bool threads, int inst>
char *default_alloc_template<threads, inst>::start_free = nullptr;

template<bool threads, int inst>
char *default_alloc_template<threads, inst>::end_free = nullptr;

template<bool threads, int inst>
std::size_t default_alloc_template<threads, inst>::heap_size = 0;

#endif



constexpr std::size_t ALIGN = 8;
constexpr std::size_t MAX_BYTES = 128;
constexpr std::size_t NFREELISTS = MAX_BYTES / ALIGN;

template<bool threads> class default_alloc_template;

union Obj {
	union Obj 	*free_list_link;
	char 		 char_client_data[1];
};


template<bool threads>
class default_alloc_template {
private:
	static Obj  		* volatile free_list[NFREELISTS];	/* 空闲链表数组 */
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
			Obj * volatile *my_free_list = free_list + FOUND_INDEX(bytes);
			Obj *cur_obj = reinterpret_cast<Obj *>(chunk + bytes);
			Obj *next_obj;
			for (int i = 1; i < nobjs; ++i) {
				next_obj = cur_obj + 1;
				cur_obj->free_list_link = next_obj;
				cur_obj = next_obj;
			}
			cur_obj->free_list_link = *my_free_list;
			*my_free_list = cur_obj;
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
		} else if (byte_left > bytes) {
			start_free += bytes;
			nobjs = 1;
			return result;

		/* 不够一块内存 */
		} else {
			std::size_t byte_to_get = 2 * total_size + (heap_size >> 4);

			/* 如果内存池中还有剩余空间则加入到空闲链表中 */
			if (byte_left > 0) {
				Obj * volatile *my_free_list = free_list + FOUND_INDEX(byte_left);
				Obj *obj_ptr = reinterpret_cast<Obj *>(start_free);
				obj_ptr->free_list_link = *my_free_list;
				*my_free_list = obj_ptr;
			}

			start_free = static_cast<char *>(std::malloc(byte_to_get));
			if (start_free == nullptr) {
				Obj *volatile *my_free_list;
				Obj *ptr;
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
		
		Obj * volatile *my_free_list = free_list + FOUND_INDEX(bytes);
		Obj *result = *my_free_list;
		if (result == nullptr)
			return refill(ROUND_UP(bytes));

		*my_free_list = result->free_list_link;
		return reinterpret_cast<void *>(result);
	}

	static void deallocate(void *ptr, std::size_t bytes) {
		if (bytes > MAX_BYTES)
			malloc_alloc::deallocate(ptr, bytes);
		
		Obj *volatile *my_free_list = free_list + FOUND_INDEX(bytes);
		Obj *obj_ptr = reinterpret_cast<Obj *>(ptr);
		obj_ptr->free_list_link = *free_list;
		*free_list = obj_ptr;
	}
};


template<bool threads>
Obj * volatile default_alloc_template<threads>::free_list[NFREELISTS] = { nullptr };

template<bool threads>
char *default_alloc_template<threads>::start_free = nullptr;

template<bool threads>
char *default_alloc_template<threads>::end_free = nullptr;

template<bool threads>
std::size_t default_alloc_template<threads>::heap_size = 0;

} //namespace sx
#endif



