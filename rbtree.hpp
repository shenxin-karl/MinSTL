#ifndef RBTREE_HPP
#include <stdexcept>
#include "malgorithm.hpp"
#include "mallocator.hpp"
#include "mutility.hpp"

namespace sx {

enum __rbcolor;

struct __rbtree_node_base;

template<typename T>
struct __rbtree_node;

struct __rbtree_iterator_base;

template<typename T, typename Ptr, typename Ref>
struct __rbtree_iterator;

template<typename Key, typename Value, typename KeyOfValue,
	typename Compare, typename Alloc>
class rbtree;


enum __rbcolor {
	__RED		= true,
	__BLACK	= false
};

struct __rbtree_node_base {
	using color_type	= __rbcolor;
	using base_ptr		= __rbtree_node_base * ;
public:
	color_type	color;		/* 结点颜色 */
	base_ptr	parent;		/* 父结点 */
	base_ptr	left;		/* 左结点 */
	base_ptr	right;		/* 右结点 */

	static base_ptr minimun(base_ptr node, base_ptr nil) {
		while (node->left != nil)
			node = node->left;
		return node;
	}

	static base_ptr maximun(base_ptr node, base_ptr nil) {
		while (node->right != nil)
			node = node->right;
		return node;
	}
};


template<typename T>
struct __rbtree_node : public __rbtree_node_base {
	T	data;
public:
	__rbtree_node(T const &val) : data(val) {}
	__rbtree_node(T &&val) noexcept(noexcept(T(std::declval<T>()))) : data(std::move(val)) {}
};


struct __rbtree_iterator_base {
	using base_ptr			= __rbtree_node_base::base_ptr;
	using iterator_category = sx::bidirectional_iterator_tag;
	using difference_type	= std::ptrdiff_t;
public:
	base_ptr	node;
	base_ptr	nil;
	base_ptr	root;
public:
	__rbtree_iterator_base(base_ptr node, base_ptr nil, base_ptr root) : node(node), nil(nil), root(root) {}
	__rbtree_iterator_base(__rbtree_iterator_base const &) = default;
	__rbtree_iterator_base &operator=(__rbtree_iterator_base const &) = default;
	~__rbtree_iterator_base() = default;
public:
	void increment() {
		if (node == nil)
			throw std::invalid_argument("iterator operator++ error");

		if (node->right != nil) {
			node = node->right;
			while (node->left != nil)
				node = node->left;
		} else {
			base_ptr p = node->parent;
			while (node == p->right && p != nil) {
				node = p;
				p = p->parent;
			}
			node = p;
		}
	}

	void decrement() {
		if (node == nil) {
			node = root;
			while (node->right != nil)
				node = node->right;
		} else if (node->left != nil) {
			node = node->left;
			while (node->right != nil)
				node = node->right;
		} else {
			base_ptr p = node->parent;
			while (node == p->left && p != nil) {
				node = p;
				p = p->parent;
			}
			if (p == nil)
				throw std::invalid_argument("iterator operator--() error!");
			node = p;
		}
	}
};


template<typename T, typename Ptr, typename Ref>
struct __rbtree_iterator : public sx::__rbtree_iterator_base {
	using value_type		= T;
	using pointer			= Ptr;
	using reference			= Ref;
	using difference_type	= std::ptrdiff_t;
	using iterator_category = sx::bidirectional_iterator_tag;
	using sx::__rbtree_iterator_base::base_ptr;
public:
	__rbtree_iterator() : __rbtree_iterator_base(nullptr, nullptr, nullptr) {}
	__rbtree_iterator(base_ptr ptr, base_ptr nil, base_ptr root) : __rbtree_iterator_base(ptr, nil, root) {}
	__rbtree_iterator(__rbtree_iterator const &) = default;
	__rbtree_iterator &operator=(__rbtree_iterator const &) = default;
	~__rbtree_iterator() = default;
public:
	reference operator*() {
		return reinterpret_cast<__rbtree_node<T> *>(this->node)->data;
	}

	pointer operator->() {
		return &(this->operator*());
	}

	__rbtree_iterator operator++(int) {
		__rbtree_iterator ret = *this;
		this->increment();
		return ret;
	}

	__rbtree_iterator &operator++() {
		this->increment();
		return *this;
	}

	__rbtree_iterator operator--(int) {
		__rbtree_iterator ret = *this;
		this->decrement();
		return ret;
	}

	__rbtree_iterator &operator--() {
		this->decrement();
		return *this;
	}

	friend bool operator==(__rbtree_iterator const &first, __rbtree_iterator const &second) noexcept {
		return first.root == second.root && first.node == second.node;
	}

	friend bool operator!=(__rbtree_iterator const &first, __rbtree_iterator const &second) noexcept {
		return !(first == second);
	}
};


template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
class rbtree : public sx::comparetor<rbtree<Key, Value, KeyOfValue, Compare, Alloc>> {
protected:
	using void_pointer = void *;
	using base_ptr	   = sx::__rbtree_node_base *;
	using rb_tree_node = sx::__rbtree_node<Value>;
	using rb_tree_color = sx::__rbcolor;
	using Allocator		= decltype(sx::transform_alloator_type<Key, __rbtree_node<Value>>(Alloc{}));
public:
	using key_type			= Key;
	using value_type		= Value;
	using poineter			= value_type *;
	using reference			= value_type &;
	using const_pointer		= value_type const *;
	using const_reference	= value_type const &;
	using link_type			= rb_tree_node * ;
	using size_type			= std::size_t;
	using difference_type	= std::ptrdiff_t;
	using iterator			= sx::__rbtree_iterator<Value, Value *, Value &>;
	using const_iterator	= sx::__rbtree_iterator<Value, Value const *, Value const &>;
protected:
	static Allocator	allocator;	/* 分配器 */
	__rbtree_node_base	header;		/* 头结点 */
	__rbtree_node_base   *nil;		/* nil 哨兵结点 */
	size_type			node_size;	/* 结点数量 */
	Compare				comp;		/* 比较器 */
protected:
	static link_type get_node() {
		return allocator.allocate(1);
	}

	static void put_node(link_type node_ptr) {
		allocator.deallocate(node_ptr, sizeof(link_type));
	}

	template<typename... Args>
	static link_type create_node(Args&&... args) {
		link_type ptr = get_node();
		try {
			allocator.construct(ptr, std::forward<Args>(args)...);
			return ptr;
		} catch (...) {
			put_node(ptr);
			throw;
		}
	}

	static void destroy_node(link_type node_ptr) {
		allocator.destroy(node_ptr);
		put_node(node_ptr);
	}

	void empty_initialize() {
		nil = allocator.allocate(1);
		header.parent = header.left = header.right = nil_node();
		nil->color = __BLACK;
		node_size = 0;
	}

	bool key_compare(base_ptr first, base_ptr second) const noexcept {
		auto key = KeyOfValue();
		return comp(key(((link_type)first)->data), key(((link_type)second)->data));
	}

	link_type root() noexcept {
		return static_cast<link_type>(header.parent);
	}

	const link_type root() const noexcept {
		return const_cast<const link_type>(const_cast<rbtree *>(this)->root());
	}

	link_type nil_node() noexcept {
		return static_cast<link_type>(nil);
	}

	const link_type nil_node() const noexcept {
		return const_cast<const link_type>(const_cast<rbtree *>(this)->nil_node());
	}
	
	link_type leftmost() noexcept {
		return static_cast<link_type>(header.left);
	}

	link_type rightmost() noexcept {
		return static_cast<link_type>(header.right);
	}

	void set_root(base_ptr node_ptr) {
		header.parent = node_ptr;
	}

	void set_leftmost(base_ptr node_ptr) {
		header.left = node_ptr;
	}

	void set_rightmost(base_ptr node_ptr) {
		header.right = node_ptr;
	}

	void left_rotate(base_ptr node_ptr) noexcept {
		base_ptr right_ptr = node_ptr->right;

		node_ptr->right = right_ptr->left;
		if (right_ptr->left != nil_node())
			right_ptr->left->parent = node_ptr;

		right_ptr->parent = node_ptr->parent;
		if (node_ptr->parent == nil_node())
			set_root(right_ptr);
		else if (node_ptr == node_ptr->parent->left)
			node_ptr->parent->left = right_ptr;
		else
			node_ptr->parent->right = right_ptr;

		right_ptr->left = node_ptr;
		node_ptr->parent = right_ptr;
	}

	void right_rotate(base_ptr node_ptr) noexcept {
		base_ptr left_ptr = node_ptr->left;

		node_ptr->left = left_ptr->right;
		if (left_ptr->right != nil_node())
			left_ptr->right->parent = node_ptr;

		left_ptr->parent = node_ptr->parent;
		if (node_ptr->parent == nil_node())
			set_root(left_ptr);
		else if (node_ptr == node_ptr->parent->left)
			node_ptr->parent->left = left_ptr;
		else
			node_ptr->parent->right = left_ptr;

		left_ptr->right = node_ptr;
		node_ptr->parent = left_ptr;
	}

	template<bool Is_Unique, typename... Args>
	iterator __insert(Args&&... args) {
		base_ptr new_node = create_node(std::forward<Args>(args)...);
		new_node->left = new_node->right = new_node->parent = nil_node();
		new_node->color = __RED;

		base_ptr node_ptr = root();
		base_ptr parent_ptr = nil_node();
		while (node_ptr != nil_node()) {
			parent_ptr = node_ptr;
			
			if (Is_Unique && node_ptr != nil_node() && (!key_compare(new_node, node_ptr) && !key_compare(node_ptr, new_node))) {
				destroy_node(static_cast<link_type>(new_node));
				return iterator(node_ptr, nil_node(), root());
			}
			if (key_compare(new_node, node_ptr))
				node_ptr = node_ptr->left;
			else
				node_ptr = node_ptr->right;
		}
		
		new_node->parent = parent_ptr;
		if (parent_ptr == nil_node())
			set_root(new_node);
		else if (key_compare(new_node, parent_ptr))
			parent_ptr->left = new_node;
		else
			parent_ptr->right = new_node;

		if (leftmost() == nil_node() || comp(static_cast<link_type>(new_node)->data, leftmost()->data))
			set_leftmost(new_node);
		if (rightmost() == nil_node() || comp(rightmost()->data, static_cast<link_type>(new_node)->data))
			set_rightmost(new_node);

		__insert_fixup(new_node);
		++node_size;
		return iterator(new_node, nil_node(), root());
	}

	void __insert_fixup(base_ptr node_ptr) {
		while (node_ptr->parent->color == __RED) {
			if (node_ptr->parent == node_ptr->parent->parent->left) {
				base_ptr parnet_ptr = node_ptr->parent;
				base_ptr uncle_ptr = parnet_ptr->parent->right;
				if (uncle_ptr->color == __RED) {
					uncle_ptr->color = __BLACK;
					node_ptr->color = __BLACK;
					node_ptr->parent->color = __RED;
					node_ptr = parnet_ptr->parent;
				} else {
					if (node_ptr == parnet_ptr->right) {
						node_ptr = parnet_ptr;
						left_rotate(node_ptr);
						parnet_ptr = node_ptr->parent;
					}
					parnet_ptr->color = __BLACK;
					parnet_ptr->parent->color = __RED;	
					right_rotate(parnet_ptr->parent);
				}
			} else {
				base_ptr parent_ptr = node_ptr->parent;
				base_ptr uncle_ptr = parent_ptr->left;
				if (uncle_ptr->color == __RED) {
					uncle_ptr->color = __BLACK;
					parent_ptr->color = __BLACK;
					parent_ptr->parent->color = __RED;
					node_ptr = parent_ptr->parent;
				} else {
					if (node_ptr == parent_ptr->left) {
						node_ptr = parent_ptr;
						right_rotate(node_ptr);
						parent_ptr = node_ptr->parent;
					}
					parent_ptr->color = __BLACK;
					parent_ptr->parent->color = __RED;
					left_rotate(parent_ptr->parent);
				}
			}
		}
		root()->color = __BLACK;
	}

	void tranfers(base_ptr node, base_ptr replace) {
		replace->parent = node->parent;
		if (node->parent == nil_node())
			set_root(replace);
		else if (node == node->parent->left)
			node->parent->left = replace;
		else
			node->parent->right = replace;
	}

	base_ptr minimun(base_ptr node) {
		return __rbtree_node_base::minimun(node, nil_node());
	}

	base_ptr maxinum(base_ptr node) {
		return __rbtree_node_base::maximun(node, nil_node());
	}

	void remove_fixup(base_ptr node) {
		base_ptr brother;
		while (node != root() && node->color == __BLACK) {
			if (node == node->parent->left) {
				brother = node->parent->right;
				if (brother->color == __RED) {
					brother->color = __BLACK;
					node->parent->color = __RED;
					left_rotate(node->parent);
					brother = node->parent->right;
				}
				if (brother->left->color == __BLACK && brother->right->color == __BLACK) {
					brother->color = __RED;
					node = node->parent;
				} else {
					if (brother->right->color == __BLACK) {
						brother->left->color = __BLACK;
						brother->color = __RED;
						left_rotate(brother);
						brother = node->parent->right;
					}
					brother->color = node->parent->color;
					brother->right->color = __BLACK;
					node->parent->color = __BLACK;
					node = root();
				}
			} else {
				brother = node->parent->left;
				if (brother->color == __RED) {
					brother->color = __BLACK;
					node->parent->color = __RED;
					right_rotate(node->parent);
					brother = node->parent->left;
				}
				if (brother->left->color == __BLACK && brother->right->color == __BLACK) {
					brother->color = __RED;
					node = node->parent;
				} else {
					if (brother->left->color == __BLACK) {
						brother->right->color = __BLACK;
						brother->color = __RED;
						left_rotate(brother);
						brother = node->parent->left;
					}
					brother->color = node->parent->color;
					brother->left->color = __BLACK;
					node->parent->color = __BLACK;
					node = root();
				}
			}
		}
		node->color = __BLACK;
	}

	iterator remove(iterator position) {
		base_ptr node = position.node;
		base_ptr origin_node = node;
		rb_tree_color origin_color = origin_node->color;
		base_ptr tranfers_node;

		if (node->left == nil_node()) {
			tranfers_node = node->right;
			tranfers(node, node->right);
		}
		else if (node->right == nil_node()) {
			tranfers_node = node->left;
			tranfers(node, node->left);
		}
		else {
			origin_node = minimun(node->right);
			origin_color = origin_node->color;
			tranfers_node = origin_node->right;
			if (origin_node->parent == node)
				tranfers_node->parent = node;
			else {
				tranfers(origin_node, tranfers_node);
				origin_node->right = node->right;
				origin_node->right->parent = origin_node;
			}
			tranfers(node, origin_node);
			origin_node->left = node->left;
			origin_node->color = node->color;
			origin_node->left->parent = origin_node;
		}

		if (origin_color == __BLACK)
			remove_fixup(tranfers_node);

		if (node == leftmost()) {
			iterator new_leftmost = position;
			++new_leftmost;
			set_leftmost(new_leftmost.node);
		}
		if (node == rightmost()) {
			iterator new_rightmost = position;
			--new_rightmost;
			set_rightmost(new_rightmost.node);
		}
		++position;
		destroy_node(static_cast<link_type>(node));
		--node_size;
		return position;
	}

	void __destroy(base_ptr node) {
		if (node == nil_node())
			return;
		__destroy(node->left);
		__destroy(node->right);
		destroy_node(static_cast<link_type>(node));
	}

	template<bool Is_Unique, typename InputIterator,
		typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>>>
	void alloc_and_fill(InputIterator first, InputIterator last) {
		try {
			for (; first != last; ++first) {
				if (Is_Unique)
					insert_unique(*first);
				else
					insert_equal(*first);
			}
		} catch (...) {
			clear();
		}
	}
public:
	rbtree() {
		empty_initialize();
	}

	rbtree(rbtree const &other) : rbtree() {
		alloc_and_fill<false>(other.begin(), other.end());
	}

	template<bool Is_Unique, typename InputIterator,
		typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>>>
	rbtree(InputIterator first, InputIterator last) : rbtree() {
		alloc_and_fill<Is_Unique>(first, last);
	}

	rbtree(rbtree && other) noexcept : rbtree() {
		swap(*this, other);
	}

	rbtree &operator=(rbtree const &other) {
		if (this == &other)
			return *this;
		rbtree tmp = other;
		swap(*this, tmp);
		return *this;
	}

	rbtree &operator=(rbtree &&other) noexcept {
		rbtree tmp = std::move(other);
		swap(*this, tmp);
		return *this;
	}

	~rbtree() {
		clear();
		allocator.deallocate(static_cast<link_type>(nil), sizeof(link_type));
	}

	static const_iterator transform_const_iterator(iterator iter) noexcept {
		return const_iterator(iter.node, iter.nil, iter.root);
	}
public:
	iterator begin() noexcept {
		return iterator(leftmost(), nil_node(), root());
	}

	iterator end() noexcept {
		return iterator(nil_node(), nil_node(), root());
	}

	const_iterator begin() const noexcept {
		return cbegin();
	}

	const_iterator end() const noexcept {
		return cend();
	}

	const_iterator cbegin() const noexcept {
		return transform_const_iterator(const_cast<rbtree *>(this)->begin());

	}

	const_iterator cend() const noexcept {
		return transform_const_iterator(const_cast<rbtree *>(this)->end());
	}

	iterator insert_unique(Value const &value) {
		iterator ret = __insert<true>(value);
		return ret;
	}

	iterator insert_equal(Value const &value) {
		iterator ret = __insert<false>(value);
		return ret;
	}
	
	size_type size() const noexcept {
		return node_size;
	}

	bool empty() const noexcept {
		return size() == 0;
	}

	void clear() {
		__destroy(root());
		allocator.deallocate(nil_node(), sizeof(link_type));
		empty_initialize();
	}

	iterator find(Key const &value) {
		link_type node = root();
		while (node != nil_node()) {
			if (KeyOfValue()(node->data) == value)
				break;
			else if (comp(node->data, value))
				node = static_cast<const link_type>(node->left);
			else
				node = static_cast<const link_type>(node->right);
		}
		return iterator(node, nil_node(), root());
	}

	const_iterator find(Key const &value) const {
		iterator ret = (const_cast<rbtree *>(this)->find(value));
		return const_iterator(ret.node, nil_node(), root());
	}

	iterator erase(iterator position) {
		iterator ret = remove(position);
		return ret;
	}

	const_iterator erase(const_iterator position) {
		iterator ret = remove(iterator(position.node, nil_node(), root()));
		return const_iterator(ret.node, nil_node(), root());
	}

	iterator min() {
		return iterator(leftmost(), nil_node(), root());
	}

	iterator max() {
		return iterator(rightmost(), nil_node(), root());
	}

	const_iterator min() const noexcept {
		return transform_const_iterator(const_cast<rbtree *>(this)->min());
	}

	const_iterator max() const noexcept {
		return transform_const_iterator(const_cast<rbtree *>(this)->max());
	}

	std::pair<iterator, iterator> equal_range(Key const &key) {
		iterator first = begin();
		while (first != end() && (comp(KeyOfValue()(*first), key) || comp(key, KeyOfValue()(*first))))
			++first;
		iterator last = first;
		while (last != end() && (!comp(key, KeyOfValue()(*last)) && !comp(KeyOfValue()(*last), key)))
			++last;
		
		return std::pair<iterator, iterator>(first, last);
	}

	std::pair<const_iterator, const_iterator> equal_range(Key const &key) const {
		std::pair<iterator, iterator> ret = const_cast<rbtree *>(this)->equal_range(key);
		return std::pair<const_iterator, const_iterator>(transform_const_iterator(ret.first), 
														 transform_const_iterator(ret.second));
	}

	iterator lower_bound(Key const &key) {
		iterator first = begin();
		while (first != end() && comp(KeyOfValue()(*first), key))
			++first;
		return first;
	}

	const_iterator lower_bound(Key const key) const {
		return transform_const_iterator(const_cast<rbtree *>(this)->lower_bound(key));
	}

	iterator upper_bound(Key const &key) {
		iterator first = lower_bound(key);
		while (first != end() && (!comp(KeyOfValue()(*first), key) && !comp(key, KeyOfValue()(*first))))
			++first;
		return first;
	}

	const_iterator upper_bound(Key const key) const {
		return transform_const_iterator(const_cast<rbtree *>(this)->upper_bound(key));
	}

	friend void swap(rbtree &first, rbtree &second) noexcept {
		using std::swap;
		swap(first.header, second.header);
		swap(first.nil, second.nil);
		swap(first.node_size, second.node_size);
		swap(first.comp, second.comp);
	}
};


template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rbtree<Key, Value, KeyOfValue, Compare, Alloc>::Allocator
rbtree<Key, Value, KeyOfValue, Compare, Alloc>::allocator{};
}

#endif // !RBTREE_HPP

