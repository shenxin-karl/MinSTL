#ifndef M_FORWARD_LIST_HPP
#define M_FORWARD_LIST_HPP
#include <cstddef>
#include "miterator.hpp"
#include "mallocator.hpp"
#include <exception>
#include <utility>

namespace sx {

struct __forward_node_base;

template<typename T> 
struct __forward_node;

template<typename T>
struct __forward_list_iterator_base;

template<typename T, typename Ptr, typename Ref>
struct __forward_list_iterator;

template<typename T>
bool operator==(__forward_list_iterator_base<T> const &, __forward_list_iterator_base<T> const &) noexcept;

template<typename T>
bool operator!=(__forward_list_iterator_base<T> const &, __forward_list_iterator_base<T> const &) noexcept;

template<typename T, typename Alloc = sx::allocator<T>>
class forward_list;

template<typename T, typename Alloc>
void swap(forward_list<T, Alloc> &, forward_list<T, Alloc> &) noexcept;

template<typename T, typename Alloc>
bool operator==(forward_list<T, Alloc> const &, forward_list<T, Alloc> const &) noexcept;

template<typename T, typename Alloc>
bool operator!=(forward_list<T, Alloc> const &, forward_list<T, Alloc> const &) noexcept;



class forward_list_empty : public std::exception {
};

struct __forward_node_base {
	__forward_node_base	*next;
};

template<typename T>
struct __forward_node : public __forward_node_base {
	T	data;
public:
	template<typename... Args>
	__forward_node(Args&&... args) : data(std::forward<Args>(args)...) {}
};

template<typename T>
struct __forward_list_iterator_base {
	using size_type			= std::size_t;
	using difference_type	= std::ptrdiff_t;
	using iterator_category = sx::bidirectional_iterator_tag;
public:
	__forward_node_base	*node;
public:
	__forward_list_iterator_base() : node(nullptr) {}
	__forward_list_iterator_base(__forward_list_iterator_base const &) = default;
	__forward_list_iterator_base &operator=(__forward_list_iterator_base const &) = default;
	~__forward_list_iterator_base() = default;
	__forward_list_iterator_base(__forward_node_base *ptr) : node(ptr) {}
public:
	void incr() {
		node = node->next;
	}

	friend bool operator==<T>(__forward_list_iterator_base const &, __forward_list_iterator_base const &) noexcept;
	friend bool operator!=<T>(__forward_list_iterator_base const &, __forward_list_iterator_base const &) noexcept;
};

template<typename T>
bool operator==(__forward_list_iterator_base<T> const &first, __forward_list_iterator_base<T> const &second) noexcept {
	return first.node == second.node;
}

template<typename T>
bool operator!=(__forward_list_iterator_base<T> const &first, __forward_list_iterator_base<T> const &second) noexcept {
	return !(first == second);
}


template<typename T, typename Ptr, typename Ref>
struct __forward_list_iterator : public __forward_list_iterator_base<T> {
	using value_type		= T;
	using pointer			= Ptr;
	using reference			= Ref;
	using difference_type	= std::size_t;
public:
	__forward_list_iterator() = default;
	__forward_list_iterator(__forward_list_iterator const &) = default;
	__forward_list_iterator &operator=(__forward_list_iterator const &) = default;
	~__forward_list_iterator() = default;
	__forward_list_iterator(__forward_node_base *ptr) : __forward_list_iterator_base<T>(ptr) {}
public:
	reference operator*() {
		return ((__forward_node<T> *)this->node)->data;
	}

	pointer operator->() {
		return &(this->operator*());
	}

	__forward_list_iterator &operator++() {
		this->incr();
		return *this;
	}

	__forward_list_iterator operator++(int) {
		__forward_list_iterator ret = *this;
		++(*this);
		return ret;
	}

	__forward_list_iterator &operator--() = delete;
	__forward_list_iterator operator--(int) = delete;
};


template<typename T, typename Alloc>
class forward_list {
	friend void swap<T, Alloc>(forward_list<T, Alloc> &, forward_list<T, Alloc> &) noexcept;
	friend bool operator==<T, Alloc>(forward_list<T, Alloc> const &, forward_list<T, Alloc> const &) noexcept;
	friend bool operator!=<T, Alloc>(forward_list<T, Alloc> const &, forward_list<T, Alloc> const &) noexcept;
public:
	using value_type		= T;
	using pointer			= T * ;
	using reference			= T & ;
	using const_pointer		= T const *;
	using const_reference	= T const &;
	using difference_type	= std::ptrdiff_t;
	using size_type			= std::size_t;
	using iterator			= __forward_list_iterator<T, T *, T &>;
	using const_iterator	= __forward_list_iterator<T, T const *, T const &>;
private:
	using link_node			= __forward_node<T>;
	using link_node_base	= __forward_node_base;
	using iterator_base		= __forward_list_iterator_base<T>;
	using Allocator			= decltype(sx::transform_alloator_type<T, link_node>(Alloc{}));
protected:
	static Allocator		allocator;		/* 分配器 */
	link_node_base			head;			/* 头结点 */
	size_type				node_size;
public:
	forward_list() noexcept : node_size(0) {
		head.next = nullptr; 
	}

	forward_list(forward_list const &other) { 
		alloc_and_fill(other.begin(), other.end()); 
	}

	forward_list(forward_list &&other) noexcept : head(other.head), node_size(other.node_size) {
		other.head.next = nullptr;
		other.node_size = 0;
	}

	forward_list &operator=(forward_list const &other) {
		forward_list tmp = other;
		sx::swap(*this, tmp);
		return *this;
	}

	forward_list &operator=(forward_list &&other) {
		forward_list tmp;
		sx::swap(*this, tmp);
		sx::swap(*this, other);
		return *this;
	}

	~forward_list() { 
		clear();  
	}

	template<typename InputIterator, typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>>>
	forward_list(InputIterator first, InputIterator end) : forward_list() {
		alloc_and_fill(first, end);
	}
private:
	template<typename... Args>
	static link_node *create_node(Args&&... args) {
		link_node *node_ptr = allocator.allocate();
		try {
			allocator.construct(node_ptr, std::forward<Args>(args)...);
			node_ptr->next = nullptr;
		} catch (...) {
			allocator.deallocate(node_ptr, sizeof(link_node));
			throw;
		}
		return node_ptr;
	}

	static void destroy_node(link_node *ptr) {
		allocator.destroy(ptr);
		allocator.deallocate(ptr, sizeof(link_node));
	}

	template<typename InputIterator>
	void alloc_and_fill(InputIterator first, InputIterator end) {
		if (first == end)
			return;

		link_node_base *cur = &head;
		link_node_base *new_node;
		try {
			cur->next = new_node = create_node(*first);
			cur = new_node;
			++first;
			node_size++;
			while (first != end) {
				new_node = create_node(*first);
				++first;
				cur->next = new_node;
				cur = new_node;
				node_size++;
			}
			new_node->next = nullptr;
		} catch (...) {
			link_node_base *node = head.next;
			while (node != cur) {
				link_node *del_node = static_cast<link_node *>(node);
				node = node->next;
				allocator.destroy(del_node);
				allocator.deallocate(del_node, sizeof(link_node));
			}
			head.next = nullptr;
			node_size = 0;
			throw;
		}
	}
public:
	iterator begin() noexcept {
		return iterator(head.next);
	}

	const_iterator begin() const noexcept {
		return const_iterator(head.next);
	}

	iterator end() noexcept {
		return iterator(nullptr);
	}

	const_iterator end() const noexcept {
		return const_iterator(nullptr);
	}

	iterator before_begin() noexcept {
		return iterator(&head);
	}

	const_iterator before_begin() const noexcept {
		return const_iterator(const_cast<__forward_node_base *>(&head));
	}

	size_type size() const noexcept {
		return node_size;
	}

	bool empty() const noexcept {
		return head.next == nullptr;
	}

	void clear() {
		if (empty())
			return;

		erase_after(before_begin(), end());
	}

	reference front() {
		if (empty())
			throw std::out_of_range("forward_list::front error!!! forward_list is empty");

		return static_cast<link_node *>(head.next)->data;
	}

	const_reference front() const {
		if (empty())
			throw std::out_of_range("forward_list::front error!!! forward_list is empty");

		return static_cast<link_node *>(head.next)->data;
	}

	void push_front(value_type const &value) {
		link_node *node = create_node(value);
		node->next = head.next;
		head.next = node;
		node_size++;
	}

	void pop_front() {
		if (empty())
			throw forward_list_empty();

		link_node *node = static_cast<link_node *>(head.next);
		head.next = node->next;
		destroy_node(node);
		node_size--;
	}

	template<typename... Args>
	void emplace_front(Args&&... args) {
		link_node *node = create_node(std::forward<Args>(args)...);
		node->next = head.next;
		head.next = node;
		node_size++;
	}

	iterator erase_after(iterator pos) {
		link_node_base *node = pos.node;
		link_node_base *after = node->next;
		if (after == nullptr)
			return end();
		
		node->next = after->next;
		node_size--;
		destroy_node(static_cast<link_node *>(after));
		return iterator(node->next);
	}

	const_iterator erase_after(const_iterator pos) {
		iterator ret = erase_after(iterator(pos.node));
		return const_iterator(ret.node);
	}

	iterator erase_after(iterator first, iterator last) {
		link_node_base *node = first.node;
		if (node == nullptr)
			return end();

		link_node_base *carry = node->next;
		node->next = last.node;
		while (carry != last.node) {
			link_node_base *next = carry->next;
			destroy_node(static_cast<link_node *>(carry));
			node_size--;
			carry = next;
		}

		return last;
	}

	const_iterator erase_after(const_iterator first, const_iterator last) {
		iterator ret = erase_after(iterator(first.node), iterator(last.node));
		return const_iterator(ret.node);
	}

	iterator insert_after(iterator pos, value_type const &value) {
		if (pos == end())
			throw std::invalid_argument("pos error");

		link_node_base *node = pos.node;
		link_node_base *new_node = create_node(value);
		new_node->next = node->next;
		node->next = new_node;
		node_size++;
		return iterator(new_node);
	}

	const_iterator insert_after(const_iterator pos, value_type const &value) {
		iterator ret = insert_after(iterator(pos.node), value);
		return const_iterator(pos.node);
	}

	template<typename... Args>
	iterator emplace_after(iterator pos, Args&&... args) {
		if (pos == end())
			throw std::invalid_argument("forward_list::emplace_after(pos, args...) error!!! pos error");

		link_node_base *node = pos.node;
		link_node_base *new_node = create_node(std::forward<Args>(args)...);
		new_node->next = node->next;
		node->next = new_node;
		node_size++;
		return iterator(new_node);
	}

	template<typename... Args>
	const_iterator emplace_after(const_iterator pos, Args&&... args) {
		iterator ret = emplace_after(iterator(pos), std::forward<Args>(args)...);
		return const_iterator(ret.node);
	}

	template<typename InputIterator, 
			 typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>>>
	iterator insert_after(iterator pos, InputIterator first, InputIterator last) {
		if (pos == end())
			throw std::invalid_argument("forward_list::insert(pos, first, last) pos error");
	
		link_node_base *node = pos.node;
		link_node_base *range_head = nullptr;
		link_node_base *curr = nullptr;
		size_type range_size = 0;
		try {
			range_head = curr = create_node(*first);
			++first;
			++range_size;
			while (first != last) {
				link_node_base *next = create_node(*first);
				++first;
				++range_size;
				curr->next = next;
				curr = next;
			}
			curr->next = node->next;
			node->next = range_head;
			node_size += range_size;
		} catch (...) {
			while (range_head != curr) {
				link_node_base *next = range_head->next;
				destroy_node(static_cast<link_node *>(range_head));
				range_head = next;
			}
			throw;
		}

		return iterator(curr);
	}

	template<typename InputIterator, 
			 typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>>>
	const_iterator insert_after(const_iterator pos, InputIterator first, InputIterator last) {
		iterator ret = insert_after(iterator(pos.node), first, last);
		return const_iterator(ret.node);
	}

	void splic_after(iterator pos, forward_list &other, iterator first, iterator last) {
		if (first == last || other.empty())
			throw std::invalid_argument("forward_list::splic_after(pos, other, first, last) error");

		iterator before = first;
		iterator curr = first;
		++curr;
		size_type range_size = 0;
		while (curr != last) {
			before = curr;
			++curr;
			++range_size;
		}

		link_node_base *first_node = first.node;
		link_node_base *left_node = first_node->next;
		first_node->next = last.node;

		link_node_base *right_node = before.node;
		link_node_base *pos_node = pos.node;
		right_node->next = pos_node->next;
		pos_node->next = left_node;

		node_size += range_size;
		other.node_size -= range_size;
	}

	void splic_after(iterator pos, forward_list &other) {
		splic_after(pos, other, other.before_begin(), other.end());
	}

	void splic_after(iterator pos, forward_list &other, iterator i) {
		iterator j = i;
		if (++j == end())
			throw std::invalid_argument("forward_list::splic_after(pos, other, i) error, iterator i invalid");

		splic_after(pos, other, i, ++j);
	}

	template<typename Compare>
	void meger(forward_list &other, Compare comp) {
		if (this == &other)
			return;

		iterator first1 = before_begin();
		iterator first2 = other.before_begin();
		iterator last(nullptr);
		iterator next1;
		iterator next2;
		while (true) {
			next1 = first1;
			next2 = first2;
			++next1;
			++next2;
			if (next1 == last || next2 == last)
				break;

			if (comp(*next2, *next1)) {
				first2.node->next = next2.node->next;
				next2.node->next = next1.node;
				first1.node->next = next2.node;
				first1 = next2;
			}
			else
				++first1;
		}

		if (next2 != last)
			splic_after(first1, other, first2, last);

		node_size += other.node_size;
		other.node_size = 0;
	}

	void meger(forward_list &other) {
		meger(other, std::less<>{});
	}

	template<typename Compare>
	void sort(Compare comp) {
		if (empty())
			return;

		forward_list carry;
		forward_list counter[64];
		int fill = 0;
		while (!empty()) {
			carry.splic_after(carry.before_begin(), *this, before_begin());
			int i = 0;
			while (i < fill && !counter[i].empty()) {
				counter[i].meger(carry, comp);
				carry.swap(counter[i++]);
			}
			carry.swap(counter[i]);
			if (i == fill)
				++fill;
		}

		for (int i = 1; i < fill; ++i)
			counter[i].meger(counter[i - 1], comp);
		
		(*this).swap(counter[fill - 1]);
	}

	void sort() {
		sort(std::less<>{});
	}

	void swap(forward_list &other) noexcept {
		sx::swap(*this, other);
	}
};

template<typename T, typename Alloc>
typename forward_list<T, Alloc>::Allocator forward_list<T, Alloc>::allocator{};

template<typename T, typename Alloc>
void swap(forward_list<T, Alloc> &first, forward_list<T, Alloc> &second) noexcept {
	using std::swap;
	swap(first.head, second.head);
	swap(first.node_size, second.node_size);
}

template<typename T, typename Alloc>
bool operator==(forward_list<T, Alloc> const &first, forward_list<T, Alloc> const &second) noexcept {
	if (first.size() != second.size())
		return false;

	auto first1 = first.begin();
	auto first2 = second.begin();
	auto end1 = first.end();
	auto end2 = first.end();

	while (first1 != end1 && first2 != end2) {
		if (*first1 != *first2)
			return false;
	}
	return true;
}

template<typename T, typename Alloc>
bool operator!=(forward_list<T, Alloc> const &first, forward_list<T, Alloc> const &second) noexcept {
	return !(first == second);
}

}


#endif // !M_FORWARD_LIST_HPP
