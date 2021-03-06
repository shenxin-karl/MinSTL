﻿
#ifndef M_LIST_HPP
#define M_LIST_HPP
#include "allocator.hpp"
#include "iterator.hpp"
#include "utility.hpp"
#include <exception>

namespace sx {

class list_empty : public std::exception {
};

template<typename T>
struct __link_node;

template<typename T, typename Alloc = sx::allocator<T>>
class list;


template<typename T>
struct __link_node {
    __link_node   *prev;
    __link_node   *next;
    T			   data;
public:
    __link_node(T const &element) : data(element) {}
    __link_node(T &&element) : data(std::move(element)) {}
};

template<typename T, typename Ptr, typename Ref>
class __list_iterator {
public:
	template<typename T, typename Alloc>
	friend class list;
    using value_type			= T;
	using pointer				= Ptr;
    using reference				= Ref;
    using difference_type		= std::ptrdiff_t;
    using link_node_ptr			= __link_node<T> *;
    using iterator_category		= sx::bidirectional_iterator_tag;
private:
    link_node_ptr   node_ptr;
public:
    __list_iterator() : node_ptr(nullptr) {}
    __list_iterator(link_node_ptr ptr) : node_ptr(ptr) { }
    __list_iterator(__list_iterator const &other) = default;
    __list_iterator &operator=(__list_iterator const &other) = default;
    ~__list_iterator() = default;
public:
    friend bool operator==(__list_iterator const &first, __list_iterator const &second) {
        return first.node_ptr == second.node_ptr;
    }

    friend bool operator!=(__list_iterator const &first, __list_iterator const &second) {
        return !(first == second);
    }

    pointer operator->() {
        return &node_ptr->data;
    }

    reference operator*() {
        return node_ptr->data;
    }

    __list_iterator &operator++() {
        node_ptr = node_ptr->next;
        return *this;
    }

    __list_iterator operator++(int) {
        __list_iterator ret = *this;
        ++(*this);
        return ret;
    }

    __list_iterator &operator--() {
        node_ptr = node_ptr->prev;
        return *this;
    }

    __list_iterator operator--(int) {
        __list_iterator ret = *this;
        --(*this);
        return ret;
    }
};

template<typename T, typename Alloc>
class list : public sx::container_helpful<list<T, Alloc>> {
	using Allocator = decltype(sx::transform_alloator_type<T, __link_node<T>>(Alloc{}));
public: 
    using value_type             = T;
    using pointer                = T *;
    using reference              = T &;
    using const_pointer          = T const *;
    using const_reference        = T const &;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using link_node_ptr          = __link_node<T> *;
    using iterator               = __list_iterator<T, T *, T &>;
	using const_iterator		 = __list_iterator<T, T const *, T const &>;
	using reverse_iterator		 = sx::__reverse_iterator<iterator>;
	using const_reverse_iterator = sx::__reverse_iterator<const_iterator>;
protected:
    static Allocator        allocator;          /* 数据分配器 */
    link_node_ptr           head_node;          /* 头结点 */
    size_type               node_size;          /* 数量 */
private:
    static link_node_ptr get_node() {
        return allocator.allocate(1);
    }

    static void put_node(link_node_ptr node_ptr) {
        allocator.deallocate(node_ptr, 1);
    }

    template<typename... Args>
    static link_node_ptr create_node(Args&&... args) {
        link_node_ptr node_ptr = get_node();
        allocator.construct(node_ptr, std::forward<Args>(args)...);
        return node_ptr;
    }

    static void destroy_node(link_node_ptr node_ptr) {
        allocator.destroy(node_ptr);
        allocator.deallocate(node_ptr, sizeof(__link_node<T>));
    }

	void empty_initialized() noexcept {
		head_node = get_node();
		head_node->next = head_node->prev = head_node;
		node_size = 0;
	}

	template<typename InputIterator>
	void alloc_and_fill(InputIterator begin, InputIterator end) {
        while (begin != end) {
            push_back(*begin);
			++begin;
        }
	}

	void destroy() {
		link_node_ptr node = head_node->next;
        link_node_ptr next;
        while (node != head_node) {
            next = node->next;
            destroy_node(node);
            node = next;
        }
        put_node(head_node);
	}

    template<typename... Args>
    void insert_aux(iterator position, Args&&... args) {
        link_node_ptr node_ptr = position.node_ptr;
        link_node_ptr new_node = create_node(std::forward<Args>(args)...);
        new_node->prev = node_ptr->prev;
        new_node->prev->next = new_node;
        new_node->next = node_ptr;
        node_ptr->prev = new_node;
    }

    /* 将 begin ~ end 区间的结点, 移动到 position 前面 */
    static void transfer(iterator position, iterator begin, iterator end) {
        iterator last = end;
        --last;
        link_node_ptr pos_node = position.node_ptr;
        link_node_ptr beg_node = begin.node_ptr;
        link_node_ptr last_node = last.node_ptr;

        beg_node->prev->next = last_node->next;
        last_node->next->prev = beg_node->prev;

        beg_node->prev = pos_node->prev;
        beg_node->prev->next = beg_node;
        last_node->next = pos_node;
        pos_node->prev = last_node;
    }
public:
	list() { empty_initialized(); }

    list(list const &other) {
		empty_initialized();
		alloc_and_fill(other.begin(), other.end());
    }

	list(list &&other) : head_node(other.head_node), node_size(other.node_size) {
		other.empty_initialized();
	}

	list &operator=(list const &other) {
		if (this == &other)
			return *this;
		list tmp = other;
		swap(tmp);
		return *this;
	}

	list &operator=(list &&other) {
		list tmp = std::move(other);
		swap(tmp);
		return *this;
	}

	list &operator=(std::initializer_list<T> const &ilst) {
		list tmp(ilst);
		swap(tmp);
		return *this;
	}

	list(std::initializer_list<T> const &ilst) {
		empty_initialized();
		insert(end(), ilst.begin(), ilst.end());
	}

	~list() {
		destroy();
	}
public:
    size_type size() const {
        return node_size;
    }

    bool empty() const {
        return head_node->next == head_node && head_node->prev == head_node;
    }

    iterator begin() {
        return iterator(head_node->next);
    }

    iterator end() {
        return iterator(head_node);
    }

	const_iterator begin() const {
		return cbegin();
	}

	const_iterator end() const {
		return cend();
	}

    const_iterator cbegin() const {
        return const_iterator(head_node->next);
    }

    const_iterator cend() const {
        return const_iterator(head_node);
    }

	reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}

	const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}

    reference front() {
        return *begin();
    }

    reference back() {
        return *(--end());
    }

    const_reference front() const {
        return *begin();
    }

    const_reference back() const {
        return *(--end());
    }

    void push_front(value_type const &value) {
        insert_aux(begin(), value);
        ++node_size;
    }

    void push_back(value_type const &value) {
        insert_aux(end(), value);
        ++node_size;
    }

    void pop_front() {
        if (empty())
            throw list_empty();
        erase(begin());
    }

    void pop_back() {
        if (empty())
            throw list_empty();
        erase(--end());
    }

    template<typename... Args>
    void emplace_front(Args&&... args) {
        insert_aux(begin(), std::forward<Args>(args)...);
        ++node_size;
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        insert_aux(end(), std::forward<Args>(args)...);
        ++node_size;
    }

    iterator insert(iterator position, value_type const &value) {
        insert_aux(position, value);
        ++node_size;
        return position;
    }

	iterator insert(iterator pos, value_type &&value) {
		insert_aux(pos, std::move(value));
		++node_size;
		return pos;
	}

	template<typename InputIter,
			 typename = std::enable_if_t<sx::is_input_iterator_v<InputIter> && 
										 sx::is_convertible_iter_type_v<InputIter, value_type>>>
	void insert(iterator pos, InputIter first, InputIter last) {
		while (first != last) {
			pos = insert(pos, *first);
			++first;
		}
	}

	void insert(iterator pos, std::initializer_list<value_type> const &ilst) {
		insert(pos, ilst.begin(), ilst.end());
	}

	void insert(iterator pos, size_type count, value_type const &value) {
		for (size_type idx = 0; idx < count; ++idx)
				pos = insert(pos, value);
	}

    template<typename... Args>
    iterator emplace(iterator position, Args&&... args) {
        insert_aux(position, std::forward<Args>(args)...);
        ++node_size;
        return position;
    }

    void clear() {
        destroy();
        empty_initialized();
    }

    iterator erase(iterator position) {
		if (position == end())
			throw std::invalid_argument("invalid list erase iterator");
        link_node_ptr node = position.node_ptr;
        node->next->prev = node->prev;
        node->prev->next = node->next;
        link_node_ptr ret = node->next;
        destroy_node(node);
        --node_size;
        return iterator(ret);
    }

	iterator erase(iterator first, iterator last) {
		link_node_ptr first_node = first.node_ptr;
		link_node_ptr last_node = last.node_ptr;
		node_size -= sx::distance(first, last);
		last_node->prev = first_node->prev;
		first_node->prev->next = last_node;
		return last;
	}

    void remove(value_type const &value) {
        for (auto iter = begin(); iter != end(); ) {
            if (*iter == value) {
                iter = erase(iter);
                continue;
            }
            ++iter;
        }
    }

	void unique() {
		for (auto iter = ++begin(); iter != end(); ) {
			iterator prev = iter;
			--prev;
			if (*prev == *iter) 
				erase(prev);
			++iter;
		}
	}

    void splice(iterator position, list &other) {
        if (!other.empty() && this != &other) {
            transfer(position, other.begin(), other.end());
            node_size += other.size();
            other.node_size = 0;
        }
    }

    void splice(iterator position, list &other, iterator index) {
        iterator after = index;
        ++after;
        transfer(position, index, after);
        node_size += 1;
        other.node_size -= 1;
    }

    void merge(list &other) {
        iterator first1 = begin();
        iterator end1 = end();
        iterator first2 = other.begin();
        iterator end2 = other.end();

        while (first1 != end1 && first2 != end2) {
            if (*first2 < *first1) {
                iterator next = first2;
                transfer(first1, first2, ++next);
				first2 = next;
			} else {
				++first1;
			}
        }

        if (first2 != end2)
            transfer(end1, first2, end2);

        node_size = other.node_size;
        other.node_size = 0;
    }

    void reverse() {
        if (empty())
            return;

        iterator iter_first = begin();
        iterator iter_end = end();
        ++iter_first;
        while (iter_first != iter_end) {
            iterator iter_old = iter_first;
            ++iter_first;
            transfer(begin(), iter_old, iter_first);
        }
    }

    void sort() {
        if (empty())
            return;

        list carry;
        list counter[64];
        int fill = 0;
        while (!empty()) {
            carry.splice(carry.begin(), *this, begin());
            int i = 0;
            while (i < fill && !counter[i].empty()) {
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if (i == fill)
                ++fill;
        }

        for (int i = 1; i < fill; ++i) 
            counter[i].merge(counter[i - 1]);
        swap(counter[fill-1]);
    }

	void swap(list &other) noexcept {
		using std::swap;
		swap(head_node, other.head_node);
		swap(node_size, other.node_size);
	}
};

template<typename T, typename Alloc>
typename list<T, Alloc>::Allocator list<T, Alloc>::allocator;


}
#endif