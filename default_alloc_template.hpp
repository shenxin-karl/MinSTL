#ifndef DEFAULT_ALLOC_TEMPLATE
#define DEFAULT_ALLOC_TEMPLATE
#include <new>
#include <cstdlib>
#include "malloc_alloc_template.hpp"

#if 0
enum : std::size_t { ALIGN = 8 };
enum : std::size_t { MAX_BYTES = 128 };
enum : std::size_t { NFREELISTS = MAX_BYTES / ALIGN };

template<bool threads, int inst>
class default_alloc_template {
private:
    /* 向上填充 8 的倍数 */
    static std::size_t ROUND_UP(std::size_t byte) {
        return (((byte) + ALIGN-1) & ~(ALIGN - 1));
    }
private:
    /* free_list 结构 */
    union Obj {
        union Obj   *free_list_link;
        char         client_data[1];
    };
private:
    /* 16个 free_lists */
    static Obj *volatile free_list[NFREELISTS];

    /* 根据 bytes， 决定使用 n 号 free_lists */
    static std::size_t FREELIST_INDEX(std::size_t bytes) {
        return (((bytes) + ALIGN-1) / ALIGN-1);
    }

    /* 返回一个大小为 n 的对象， 并可能加入大小为 n 的其他区块到 free list */
    static void *refill(std::size_t n);

    /* 配置一大块空间， 可容纳 nobjs 个大小为 size 的区块 */    
    static char *chunk_alloc(std::size_t size, int &nobjs);

    static char         *start_free;         /* 内存池开始位置， 只在 chunk_alloc() 中变化 */
    static char         *end_free;           /* 内存池结束位置， 只在 chunk_alloc() 中变化 */
    static std::size_t   heap_size;         
public:
    static void *allocate(std::size_t n);
    static void  deallocate(void *ptr, std::size_t n);
    static void *reallocate(void *ptr, std::size_t old_sz, std::size_t new_sz);
};

template<bool threads, int inst>
char *default_alloc_template<threads, inst>::start_free = nullptr;

template<bool threads, int inst>
char *default_alloc_template<threads, inst>::end_free = nullptr;

template<bool threads, int inst>
std::size_t default_alloc_template<threads, inst>::heap_size = 0;

template<bool threads, int inst>
typename default_alloc_template<threads, inst>::Obj * volatile
default_alloc_template<threads, inst>::free_list[NFREELISTS] = { 0 };


template<bool threads, int inst>
void *default_alloc_template<threads, inst>::allocate(std::size_t n)
{
    Obj * volatile *my_free_list;
    Obj *result;

    if (n > MAX_BYTES) 
        return malloc_alloc::allocate(n);

    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if (result == nullptr) {
        void *r = refill(ROUND_UP(n));
        return r;
    }

    *my_free_list = result->free_list_link;
    return result;
}

template<bool threads, int inst>
void default_alloc_template<threads, inst>::deallocate(void *ptr, std::size_t n)
{
	Obj *q = static_cast<Obj *>(ptr);
	Obj * volatile * my_free_list;

	if (n > MAX_BYTES) {
		malloc_alloc::deallocate(ptr, n);
		return;
	}

	my_free_list = FREELIST_INDEX(n);
	q->free_list_link = *my_free_list;
	*my_free_list = q;
}

template<bool threads, int inst>
void *default_alloc_template<threads, inst>::refill(std::size_t n)
{
	int nobjs = 20;
	Obj *result;
	Obj *current_obj;
	Obj *next_obj;
	char *chunk = chunk_alloc(n, nobjs);
	Obj * volatile *my_free_list;

	if (nobjs == 1)
		return chunk;

	my_free_list = free_list + FREELIST_INDEX(n);
	result = static_cast<Obj *>chunk;
	*my_free_list = next_obj = static_cast<Obj *>(chunk + n);
	for (int i = 1; ; ++i) {
		current_obj = next_obj;
		next_obj = static_cast<Obj *>(static_cast<char *>(next_obj + n));
		if (nobjs - 1 == i) {
			current_obj->free_list_link = nullptr;
			break;
		} else
			current_obj->free_list_link = next_obj;
	}

	return result;
}


template<bool threads, int inst>
char *default_alloc_template<threads, inst>::chunk_alloc(std::size_t n, int &nobjs)
{
	char *result;
	std::size_t total_bytes = size * nobjs;
	std::size_t byte_left = end_free - start_free;

	if (byte_left >= total_bytes) {
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else if (byte_left >= size) {
		nobjs = byte_left / size;
		total_bytes = size * nobjs;
		result = start_free;
		start_free += total_bytes;
		return result;
	}
	else {
		std::size_t	byte_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
		if (byte_left > 0) {
			Obj * volatile *my_free_list = free_list + FREELIST_INDEX(byte_left);
			static_cast<Obj *>(start_free)->free_list_link = *my_free_list;
			*my_free_list = static_cast<Obj *>(start_free);
		}

		start_free = static_cast<char *>(malloc(byte_to_get));
		if (start_free == nullptr) {
			Obj * volatile *my_free_list;
			Obj *p;
			for (int i = size; i < MAX_BYTES; i += ALIGN) {
				p = *my_free_list;
				if (p != nullptr) {
					*my_free_list = p->free_list_link;
					start_free = static_cast<char *>(p);
					end_free = start_free + i;
					return chunk_alloc(size, nobjs);
				}
			}
			end_free = nullptr;
			start_free = static_cast<char *>(malloc_alloc::allocate(byte_to_get));
		}

		heap_size += byte_to_get;
		end_free = start_free + byte_to_get;
		return chunk_alloc(size, nobjs);
	}
} 

#endif
#endif


#ifndef DEFAULT_ALLOC_TEMPLATE_HPP
#define DEFAULT_ALLOC_TEMPLATE_HPP
#include <cstddef>
#include <cstdlib>

namespace sx {

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
			}
			else if (byte_left >= size) {
				nobjs = byte_left / size;
				total_size = size * nobjs;
				result = start_free;
				start_free += total_size;
				return result;

				/* 内存池中没有足够的空间分配一个对象 */
			}
			else {
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
				malloc_alloc::dealloca(ptr, bytes);
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
} //namespace sx
#endif
