#ifndef RBTREE_HPP
#include <stdexcept>
#include "malgorithm.hpp"
#include "mallocator.hpp"

namespace sx {

enum rbcolor;

struct rbtree_node_base;

template<typename T>
struct rbtree_node;

struct rbtree_iterator_base;

template<typename T, typename Ptr, typename Ref>
struct rbtree_iterator;

template<typename Key, typename Value, typename KeyOfValue,
	typename Compare, typename Alloc>
class rbtree;


enum rbcolor {
	RED		= true,
	BLACK	= false
};

struct rbtree_node_base {
	using color_type	= rbcolor;
	using base_ptr		= rbtree_node_base * ;
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
struct rbtree_node : public rbtree_node_base {
	T	data;
public:
	rbtree_node(T const &val) : data(val) {}
	rbtree_node(T &&val) noexcept(noexcept(T(std::declval<T>()))) : data(std::move(val)) {}
};


struct rbtree_iterator_base {
	using base_ptr			= rbtree_node_base::base_ptr;
	using iterator_category = sx::bidirectional_iterator_tag;
	using difference_type	= std::ptrdiff_t;
public:
	base_ptr	node;
	base_ptr	nil;
	base_ptr	root;
public:
	rbtree_iterator_base(base_ptr node, base_ptr nil, base_ptr root) : node(node), nil(nil), root(root) {}
	rbtree_iterator_base(rbtree_iterator_base const &) = default;
	rbtree_iterator_base &operator=(rbtree_iterator_base const &) = default;
	~rbtree_iterator_base() = default;
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
struct rbtree_iterator : public sx::rbtree_iterator_base {
	using value_type		= T;
	using pointer			= Ptr;
	using reference			= Ref;
	using difference_type	= std::ptrdiff_t;
	using iterator_category = sx::bidirectional_iterator_tag;
	using sx::rbtree_iterator_base::base_ptr;
public:
	rbtree_iterator() : rbtree_iterator_base(nullptr, nullptr, nullptr) {}
	rbtree_iterator(base_ptr ptr, base_ptr nil, base_ptr root) : rbtree_iterator_base(ptr, nil, root) {}
	rbtree_iterator(rbtree_iterator const &) = default;
	rbtree_iterator &operator=(rbtree_iterator const &) = default;
	~rbtree_iterator() = default;
public:
	reference operator*() {
		return reinterpret_cast<rbtree_node<T> *>(this->node)->data;
	}

	pointer operator->() {
		return &(this->operator*());
	}

	rbtree_iterator operator++(int) {
		rbtree_iterator ret = *this;
		this->increment();
		return ret;
	}

	rbtree_iterator &operator++() {
		this->increment();
		return *this;
	}

	rbtree_iterator operator--(int) {
		rbtree_iterator ret = *this;
		this->decrement();
		return ret;
	}

	rbtree_iterator &operator--() {
		this->decrement();
		return *this;
	}

	friend bool operator==(rbtree_iterator const &first, rbtree_iterator const &second) noexcept {
		return first.root == second.root && first.node == second.node;
	}

	friend bool operator!=(rbtree_iterator const &first, rbtree_iterator const &second) noexcept {
		return !(first == second);
	}
};


template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
class rbtree {
protected:
	using void_pointer = void *;
	using base_ptr	   = sx::rbtree_node_base *;
	using rb_tree_node = sx::rbtree_node<Value>;
	using rb_tree_color = sx::rbcolor;
	using Allocator		= decltype(sx::transform_alloator_type<Key, rbtree_node<Value>>(Alloc{}));
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
	using iterator			= sx::rbtree_iterator<Value, Value *, Value &>;
protected:
	static Allocator	allocator;	/* 分配器 */
	rbtree_node_base	header;		/* 头结点 */
	rbtree_node_base    nil;		/* nil 哨兵结点 */
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
		header.parent = header.left = header.right = nil_node();
		nil.color = BLACK;
	}

	bool key_compare(base_ptr first, base_ptr second) const noexcept {
		auto key = KeyOfValue();
		return comp(key(((link_type)first)->data), key(((link_type)second)->data));
	}

	link_type root() noexcept {
		return static_cast<link_type>(header.parent);
	}

	link_type nil_node() noexcept {
		return static_cast<link_type>(&nil);
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
		new_node->color = RED;

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
		while (node_ptr->parent->color == RED) {
			if (node_ptr->parent == node_ptr->parent->parent->left) {
				base_ptr parnet_ptr = node_ptr->parent;
				base_ptr uncle_ptr = parnet_ptr->parent->right;
				if (uncle_ptr->color == RED) {
					uncle_ptr->color = BLACK;
					node_ptr->color = BLACK;
					node_ptr->parent->color = RED;
					node_ptr = parnet_ptr->parent;
				} else {
					if (node_ptr == parnet_ptr->right) {
						node_ptr = parnet_ptr;
						left_rotate(node_ptr);
						parnet_ptr = node_ptr->parent;
					}
					parnet_ptr->color = BLACK;
					parnet_ptr->parent->color = RED;	
					right_rotate(parnet_ptr->parent);
				}
			} else {
				base_ptr parent_ptr = node_ptr->parent;
				base_ptr uncle_ptr = parent_ptr->left;
				if (uncle_ptr->color == RED) {
					uncle_ptr->color = BLACK;
					parent_ptr->color = BLACK;
					parent_ptr->parent->color = RED;
					node_ptr = parent_ptr->parent;
				} else {
					if (node_ptr == parent_ptr->left) {
						node_ptr = parent_ptr;
						right_rotate(node_ptr);
						parent_ptr = node_ptr->parent;
					}
					parent_ptr->color = BLACK;
					parent_ptr->parent->color = RED;
					left_rotate(parent_ptr->parent);
				}
			}
		}
		root()->color = BLACK;
	}
public:
	rbtree() : node_size(0) {
		empty_initialize();
	}


public:
	iterator begin() {
		return iterator(leftmost(), nil_node(), root());
	}

	iterator end() {
		return iterator(nil_node(), nil_node(), root());
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
};


template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc>
typename rbtree<Key, Value, KeyOfValue, Compare, Alloc>::Allocator
rbtree<Key, Value, KeyOfValue, Compare, Alloc>::allocator{};
}

#endif // !RBTREE_HPP

