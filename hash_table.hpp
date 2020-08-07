#ifndef M_HASH_TABLE_HPP
#define M_HASH_TABLE_HPP
#include "allocator.hpp"
#include "utility.hpp"
#include "iterator.hpp"
#include "vector.hpp"
#include <utility>
#include <cstddef>

namespace sx {

template<typename> 
struct hash_table_node;

template<typename, typename, typename, typename, typename, typename>
class hash_iterator;

template<typename, typename, typename, typename, typename, typename>
class hash_const_iterator;

template<typename Value, typename Key, 
    typename HashFunc, typename ExtractKey,
    typename EqualKey, typename Alloc = sx::allocator<Value>>
class hash_table;

template<typename Value, typename Key,
	typename HashFunc, typename ExtractKey,
	typename EqualKey, typename Alloc>
void swap(hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> &,
		hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> &) noexcept;


/* 哈希桶 */
template<typename T>
struct hash_table_node {
    hash_table_node     *next;      /* 下一个结点 */
    T                    data;      /* 数据域 */
public:
    hash_table_node(T const &val) : data(val) {}
    hash_table_node(T &&val) : data(std::move(val)) {}
};

/* 哈希表迭代器 */
template<typename Value, typename Key, typename HashFunc,
    typename ExtractKey, typename EqualKey, typename Alloc>
class hash_iterator {
public:
    using hashtable         = hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using iterator          = hash_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
	using const_iterator	= hash_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using node              = hash_table_node<Value>;
    using iterator_category = sx::forward_iteratpr_tag;
    using value_type        = Value;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t; 
    using pointer           = value_type *;
    using reference         = value_type &;
public:
    node		*curr;      /* 迭代器所指向的结点 */
    hashtable   *table;     /* 保存对容器的连接关系 */
public:
    hash_iterator(node *curr = nullptr, hashtable *table = nullptr) 
		: curr(curr), table(table) {}

    reference operator*() const {
        return curr->data;
    }

    pointer operator->() const {
        return &(this->operator*());
    }

    iterator &operator++() {
        node const *old = curr;
        curr = curr->next;
        if (curr == nullptr) {
            size_type bucket = table->bucket_index(old->data);
            while (curr != nullptr && ++bucket < table->bucket_count())
                curr = table->buckets[bucket];
        }     
        return *this;
    }

    iterator operator++(int) {
        iterator ret = *this;
        ++(*this);
        return ret;
    }

    friend bool operator==(iterator const &first, iterator const &second) noexcept {
        return first.table == second.table && first.curr == second.curr;
    }

    friend bool operator!=(iterator const &first, iterator const &second) noexcept {
        return !(first == second);
    }

	explicit operator const_iterator() const noexcept;
}; 

template<typename Value, typename Key, typename HashFunc, 
    typename ExtractKey, typename EqualKey, typename Alloc>
class hash_const_iterator {
	using iterator			= hash_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
public:
    using hashtable         = hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using node              = hash_table_node<Value>;
    using iterator_category = sx::forward_iteratpr_tag;
    using value_type        = Value;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t; 
    using pointer           = value_type const *;
    using reference         = value_type const &;
public:
    node        const *curr;
    hashtable   const *table;
public:
	hash_const_iterator() : curr(nullptr), table(nullptr) { }

    hash_const_iterator(node const *curr, hashtable const *table) : curr(curr), table(table) { }

    reference operator*() {
        return curr->data;
    }

    pointer operator->() {
        return &(this->operator*());
    }
    
	hash_const_iterator &operator++() {
        node const *old = curr;
        curr = curr->next;
        if (curr == nullptr) {
            unsigned long bucket = table->bucket_index(old->data);
            while (curr != nullptr && bucket < table->bucket_count())
               curr = table->buckets[bucket]; 
        }
        return *this;
    }

	hash_const_iterator &operator++(int) {
		hash_const_iterator ret = *this;
        ++(*this);
        return ret;
    }

	friend bool operator==(hash_const_iterator const &first, hash_const_iterator const &second) noexcept {
		return first.table == second.table && first.curr == second.curr;
	}

	friend bool operator!=(hash_const_iterator const &first, hash_const_iterator const &second) noexcept {
		return !(first == second);
	}

	explicit operator typename hash_const_iterator::iterator () const noexcept;
};

/* 哈希表 */
template<typename Value, typename Key, 
    typename HashFunc, typename ExtractKey,
    typename EqualKey, typename Alloc>
class hash_table {
public:
    using hasher            = HashFunc;
    using key_equal         = EqualKey;
    using size_type         = std::size_t;
    using key_type          = Key;
    using value_type        = Value;
    using pointer           = value_type *;
    using reference         = value_type &;
    using const_pointer     = value_type const *;
    using const_reference   = value_type const &;
    using difference_type   = std::ptrdiff_t;
    using iterator          = hash_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
    using const_iterator    = hash_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;
private:
    using node              = hash_table_node<Value>;
    using Allocator         = decltype(sx::transform_alloator_type<Value, node>(Alloc{}));

    friend class iterator;
    friend class const_iterator;
private:
    static Allocator		allocator;      /* 结点分配器 */
    hasher					hash;           /* 哈希函数 */
    key_equal				equals;         /* 判断 key 是否相等 */
    ExtractKey				get_key;        /* 获取 key 键 */
    vector<node *>			buckets;        /* 哈希桶 */ 
    size_type				num_elements;   /* 元素数量 */
    size_type				first_index;    /* 指向第一个使用的桶 */
protected:
    static constexpr unsigned long prime_list[28] = 
    {
        53,         97,             193,        389,        769,
        1543,       3079,           6151,       12289,      24593,
        49157,      98317,          196613,     393241,     786433,
        1572869,    3145739,        6291469,    12582917,   25165843,
        50331653,   10663319,       20132611,   402653189,  805306457,
        1610612741, 3221225473ul,   4294967291ul
    };

    /* 获得下一个 buckets 的大小 */
    static unsigned long next_prime(unsigned long n) {
        auto first = std::begin(prime_list);
        auto last = std::end(prime_list);
        auto pos = std::lower_bound(first, last, n);
        return pos == last ? *(last - 1) : *pos;
    }

    static node *get_node() noexcept {
        return allocator.allocate(1); 
    }

    static void put_node(node *ptr) noexcept {
        allocator.deallocate(ptr, sizeof(node));
    }

    template<typename... Args>
    static node *create_node(Args&&... args) {
        node *ptr = get_node();
        allocator.construct(ptr, std::forward<Args>(args)...);
        return ptr;
    }

	static void destroy_node(node *ptr) {
        allocator.destroy(ptr);
        put_node(ptr);
    }

	unsigned long hash_index(key_type const &key, unsigned long buckets_size) const {
		return hash(key) % buckets_size;
	}

	unsigned long hash_index(key_type const &key) const {
		return hash_index(key, bucket_count());
	}

    unsigned long bucket_index(value_type const &val, unsigned long buckets_size) const {
		return hash_index(ExtractKey()(val), buckets_size);
    }

    unsigned long bucket_index(value_type const &val) const {
        return bucket_index(val, bucket_count()); 
    }
private:

    template<bool IsUnique>
    std::pair<iterator, bool> __insert(value_type const &val) {
        unsigned long index = bucket_index(val);
        if (index < first_index)
            first_index = index;

		node *curr = buckets[index];
		node *prev = nullptr;
		while (curr != nullptr) {
			if (equals(ExtractKey()(curr->data), ExtractKey()(val)))
				break;

			prev = curr;
			curr = curr->next;
		}
        
		if (curr != nullptr && IsUnique)
			return { iterator(curr, this), false };

        node *new_node = create_node(val);
		new_node->next = curr;
        ++num_elements;

		if (prev != nullptr)
			prev->next = new_node;
		else
			buckets[index] = new_node;

        return std::pair<iterator, bool>(iterator(new_node, this), true);
    }

	size_type remove(key_type const &key) {
		unsigned long index = hash_index(key);
		
		node *curr = buckets[index];
		node *prev = nullptr;
		node *del_list = nullptr;
		while (curr != nullptr) {
			if (equals(get_key(curr->data), key)) {
				del_list = curr;
				node *right_prev = nullptr;
				while (curr != nullptr && equals(get_key(curr->data), key)) {
					right_prev = curr;
					curr = curr->next;
				}
				right_prev->next = nullptr;
				prev->next = curr;
				break;
			}
			prev = curr;
			curr = curr->next;
		}
		
		size_type del_size = 0;
		while (del_list != nullptr) {
			node *del_node = del_list;
			del_list = del_list->next;
			destroy_node(del_node);
			++del_size;
		}

		num_elements -= del_size;
		return del_size;
	}
public:
    hash_table(HashFunc const &hash_func, EqualKey const &euqal_func)
        : hash(hash_func), equals(euqal_func), num_elements(0), first_index(0) { 
        first_index = bucket_count() - 1;
    }

    hash_table(size_type n, HashFunc const &hash_func, EqualKey const &equal_func)
        : hash(hash_func),  equals(equal_func), num_elements(0), buckets(n, nullptr) { 
        first_index = bucket_count() - 1;
    }

    hash_table(hash_table const &other) 
        : hash_table(other.hash, other.equals, other.begin(), other.end()) {
    }

    hash_table(hash_table &&other) noexcept
    : hash(std::move(other.hash)), equals(std::move(other.equals)), num_elements(other.num_elements),
        first_index(other.first_index), buckets(std::move(other.buckets)) {
        std::fill(other.buckets.begin(), other.buckets.end(), nullptr);
        other.first_index = other.bucket_count() - 1;
        other.num_elements = 0;
    }

    hash_table &operator=(hash_table const &other) {
        if (this != &other) {
            hash_table tmp = other;
            sx::swap(tmp, *this);
        }
        return *this;
    }

    hash_table &operator=(hash_table &&other) {
        hash_table tmp(hash, equals);
		sx::swap(tmp, other);
        sx::swap(*this, tmp);
        return *this;
    }

    template<typename InputIterator, 
        typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
    hash_table(HashFunc const &hash_func, EqualKey const &equal_func,
        InputIterator first, InputIterator last) : hash_table(hash_func, equal_func) {
        for ( ; first != last; ++first)
            insert_unique(*first);
    }

    ~hash_table() {
        clear();
    }
public:

    size_type size() const noexcept {
        return num_elements;
    }

    bool empty() const noexcept {
        return num_elements == 0;
    }


    static size_type max_size() noexcept {
		return std::numeric_limits<size_type>::max();
    }

    void clear() {
        for (size_type i = 0; i < bucket_count(); ++i) {
            if (buckets[i] == nullptr)
                continue;
            
            while (buckets[i] != nullptr) {
                node *first = buckets[i];
                buckets[i] = first->next;
                destroy_node(first);
            }
        }

        num_elements = 0;
        first_index = bucket_count() - 1;
    }

    iterator begin() noexcept { 
        return iterator(buckets[first_index], this);
    }

    iterator end() noexcept {
        return iterator(nullptr, this);
    }

    const_iterator begin() const noexcept {
        return cbegin();
    }

    const_iterator end() const noexcept {
        return cend();
    }

    const_iterator cbegin() const noexcept {
		return const_iterator(buckets[first_index], this);
    }

    const_iterator cend() const noexcept {
		return const_iterator(nullptr, this);
    }

	template<typename T,
		typename = std::enable_if_t<std::is_convertible_v<T, value_type>>>
	iterator insert_unique(T const &val) {
		resize(num_elements + 1);
		return __insert<true>(val);
	}

    std::pair<iterator, bool> insert_unique(value_type const &val) {
        resize(num_elements + 1);
        return __insert<true>(val);
    }

    template<typename InputIterator, 
        typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
    void insert_unique(InputIterator first, InputIterator last) {
		for ( ; first != last; ++first)
			__insert<true>(*first);
    }

	template<typename T,
		typename = std::enable_if_t<std::is_convertible_v<T, value_type>>>
	iterator insert_equal(T const &val) {
		resize(num_elements + 1);
		return __insert<false>(val);
	}

    iterator insert_equal(value_type const &val) {
        resize(num_elements + 1);
        return __insert<false>(val).first;
    }

    template<typename InpuIterator,
        typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator> 
        && sx::is_convertible_iter_type_v<InputIterator, value_type>>>
    iterator insert_equal(InpuIterator first, InpuIterator last) {
        iterator ret;
        for ( ; first != last; ++first)
            ret = insert_equal(*first);
        return ret;
    }

    template<typename... Args> 
    std::pair<iterator, bool> emplace_unique(Args&&... args) {
        resize(num_elements + 1);
        return __insert<true>(std::forward<Args>(args)...);
    }

    template<typename... Args>
    iterator emplace_equal(Args&&... args) {
        resize(num_elements + 1);
        return __insert<false>(std::forward<Args>(args)...).first; 
    }

    size_type erase(key_type const &key) {
        return remove(key);
    }

	iterator erase(iterator pos) {
		if (pos == end())
			return pos;

		size_type index = bucket_index(pos.curr->data);
		node *curr = buckets[index];
		node *prev = nullptr;
		while (curr != pos.curr) {
			prev = curr;
			curr = curr->next;
		}

		if (prev == nullptr)
			buckets[index] = curr->next;
		else
			prev->next = curr->next;

		destroy_node(curr);
		--num_elements;
		return ++iterator(prev, this);
	}

	iterator erase(iterator first, iterator last) {
		if (first == end() || first == last)
			return end();

		vector<node *> carry;
		size_type first_idx = bucket_index(first.curr->data);
		size_type last_idx = last.curr != nullptr ? bucket_index(last.curr->data) : bucket_count();
		
		node *curr = buckets[first_idx];
		node *prev = nullptr;
		node *first_curr = first.curr;
		while (curr != first_curr) {
			prev = curr;
			curr = curr->next;
		}

		carry.push_back(curr);
		if (prev == nullptr)
			buckets[first_idx] = nullptr;
		else
			prev->next = nullptr;

		for (size_type i = first_idx; i != last_idx; ++i) {
			carry.push_back(buckets[i]);
			buckets[i] = nullptr;
		}

		for (auto ptr : carry) {
			while (ptr != nullptr) {
				node *del_node = ptr;
				ptr = ptr->next;
				destroy_node(del_node);
				--num_elements;
			}
		}

		if (last_idx != bucket_count()) {
			node *left_ptr = buckets[last_idx];
			node *last_curr = last.curr;
			buckets[last_idx] = last_curr;
			while (left_ptr != last_curr) {
				node *del_node = left_ptr;
				left_ptr = left_ptr->next;
				destroy_node(del_node);
				--num_elements;
			}
		}

		return iterator(buckets[last_idx], this);
	}

	size_type count(key_type const &key) const {
		node *first = buckets[hash_index(key)];
		size_type distance = 0;
		while (first != nullptr) {
			if (equals(get_key(first->data), key)) {
				first = first->next;
				++distance;
			}
		}
		return distance;
	}

	iterator find(key_type const &key) const {
		node *first = buckets[hash_index(key)];
		while (first != nullptr) {
			if (equals(get_key(first->data), key))
				break;
		}

		return iterator(first, const_cast<hash_table *>(this));
	}

	std::pair<iterator, iterator> equal_range(key_type const &key) const {
		iterator first = find(key);
		iterator last = first;

		while (last != static_cast<iterator>(end()) && equals(get_key(*last), key))
			++last;
		
		return { first, last };
	}

	void resize(size_type num_elements_hint) {
		const size_type old = bucket_count();
		if (num_elements < old)
			return;

		const size_type new_size = next_prime(old);
		if (num_elements == old)
			return;

		vector<node *> carry(new_size, nullptr);
		first_index = new_size;
		for (unsigned long bucket = 0; bucket < old; ++bucket) {
			node *head = buckets[bucket];
			if (head == nullptr)
				continue;

			while (head != nullptr) {
				node *element = head;
				head = head->next;

				unsigned long index = bucket_index(element->data, new_size);
				if (index < first_index)
					first_index = index;

				node *new_bucket_head = carry[index];
				element->next = new_bucket_head;
				new_bucket_head = element;
			}
		}
		sx::swap(carry, buckets);
	}

	size_type bucket_count() const noexcept {
		return buckets.size();
	}

	static size_type max_bucket_count() noexcept {
		auto end = std::end(prime_list);
		return *(--end);
	}

	void swap(hash_table &other) noexcept {
		using std::swap;
		swap(hash, other.hash);
		swap(equals, other.equals);
		swap(get_key, other.get_key);
		swap(buckets, other.buckets);
		swap(num_elements, other.num_elements);
		swap(first_index, other.first_index);
	}

	friend void swap(hash_table &lhs, hash_table &rhs) noexcept;
};


template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline hash_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::
operator typename hash_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::const_iterator() const noexcept {
	return const_iterator(curr, table);
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
inline hash_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::
operator typename hash_const_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator() const noexcept {
	return iterator(const_cast<node *>(curr), const_cast<hashtable *>(table));
}


template<typename Value, typename Key,
	typename HashFunc, typename ExtractKey,
	typename EqualKey, typename Alloc>
inline void swap(hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> &lhs, hash_table<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> &rhs) noexcept {
	lhs.swap(rhs);
}


template<typename Value, typename Key,
	typename ExtractKey, typename HashFunc,
	typename EqualKey, typename Alloc>
typename hash_table< Value, Key, ExtractKey, HashFunc, EqualKey, Alloc>::Allocator
hash_table< Value, Key, ExtractKey, HashFunc, EqualKey, Alloc>::allocator;

}

#endif