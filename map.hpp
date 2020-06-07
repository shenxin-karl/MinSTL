#ifndef M_MAP_HPP
#define M_MAP_HPP
#include "allocator.hpp"
#include "utility.hpp"
#include "rbtree.hpp"
#include <utility>

namespace sx {

template<typename Key, typename Value,
	typename Compare = std::less<Key>,
	typename Alloc = sx::allocator<std::pair<const Key, Value>>>
class map;

template<typename Key, typename Value,
	typename Compare, typename Alloc>
void swap(map<Key, Value, Compare, Alloc> &, map<Key, Value, Compare, Alloc> &) noexcept;


template<typename Key, typename Value,
	typename Compare = std::less<Key>,
	typename Alloc = sx::allocator<std::pair<const Key, Value>>>
class multimap;


template<typename Key, typename Value,
	typename Compare, typename Alloc>
void swap(multimap<Key, Value, Compare, Alloc> &, multimap<Key, Value, Compare, Alloc> &) noexcept;



template<typename Key, typename Value,
	typename Compare, typename Alloc>
class map : public sx::comparetor<map<Key, Value, Compare, Alloc>> {
public:
	using key_type		= Key;
	using value_type	= std::pair<const Key, Value>;
	using key_compare	= Compare;

	/* pair */
	struct value_compare {
		Compare comp;
		value_compare(Compare comp) : comp(comp) {}
	public:
		bool operator()(value_type const &first, value_type const &second) const {
			return comp(first.first, second.first);
		}
	};
	
	/* 提取 value 中的 key 值 */
	struct key_of_value {
		key_type const &operator()(value_type const &par) const {
			return par.first;
		}
	};
private:
	using Container = sx::rbtree<key_type, value_type, key_of_value, key_compare, Alloc>;
public:
	using pointer			= typename Container::pointer;
	using reference			= typename Container::reference;
	using const_pointer		= typename Container::const_pointer;
	using const_reference	= typename Container::const_reference;
	using difference_type	= typename Container::difference_type;
	using size_type			= typename Container::size_type;
	using iterator			= typename Container::iterator;
	using const_iterator	= typename Container::const_iterator;
private:
	Container container;		/* 底层红黑树容器 */
public:
	map() : container(Compare()) {}

	explicit map(Compare const &comp) : container(comp) {}

	template<typename InputIterator,
		typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	map(InputIterator first, InputIterator last) {
		container.insert_unique(first, last);
	}

	map(map const &other) : container(other.container) {}

	map(map &&other) : container(std::move(other.container)) {}

	map &operator=(map const &other) {
		container = other.container;
		return *this;
	}

	map &operator=(map &&other) {
		container = std::move(other.container);
		return *this;
	}

	~map() {}
public:
	size_type size() const noexcept {
		return container.size();
	}

	bool empty() const noexcept {
		return container.empty();
	}

	void clear() {
		container.clear();
	}

	iterator begin() noexcept {
		return container.begin();
	}

	iterator end() noexcept {
		return container.end();
	}

	const_iterator begin() const noexcept {
		return cbegin();
	}

	const_iterator end() const noexcept {
		return cend();
	}

	const_iterator cbegin() const noexcept {
		return container.cbegin();
	}

	const_iterator cend() const noexcept {
		return container.cend();
	}

	void swap(map &other) {
		container.swap(other.container);
	}

	std::pair<iterator, bool> insert(value_type const &val) {
		return container.insert_unique(val);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args) {
		return container.emplace_unique(std::forward<Args>(args)...);
	}

	template<typename InputIterator, 
		typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	void insert(InputIterator first, InputIterator last) {
		container.insert_unique(first, last);
	}

	iterator erase(iterator position) {
		return container.erase(position);
	}

	iterator erase(key_type const &key) {
		return container.erase(key);
	}

	iterator erase(iterator postion, key_type const &key) {
		return container.erase(key);
	}

	iterator erase(iterator first, iterator last) {
		while (first != last)
			erase(first++);
		return last;
	}

	iterator find(key_type const &key) {
		return container.find(key);
	}

	const_iterator find(key_type const &key) const {
		return container.transform_const_iterator(const_cast<map *>(this)->find(key));
	}

	iterator lower_bound(key_type const &key) {
		return container.lower_bound(key);
	}

	const_iterator lower_bound(key_type const &key) const {
		return container.transform_const_iterator(const_cast<map *>(this)->lower_bound(key));
	}

	iterator upper_bound(key_type const &key) {
		return container.upper_bound(key);
	}

	const_iterator upper_bound(key_type const &key) const {
		return container.transform_const_iterator(const_cast<map *>(this)->upper_bound(key));
	}

	std::pair<iterator, iterator> equal_range(key_type const &key) {
		return std::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
	}

	std::pair<const_iterator, const_iterator> equal_range(key_type const &key) const {
		std::pair<iterator, iterator> ret = const_cast<map *>(this)->equal_range(key);
		return std::pair<const_iterator, const_iterator>(container.transform_const_iterator(ret.first),
														 container.transform_const_iterator(ret.second));
	}

	iterator max() {
		return container.max();
	}

	iterator min() {
		return container.min();
	}

	size_type count(Key const &key) const {
		return container.count(key);
	}

	Value &operator[](key_type const &key) {
		std::pair<iterator, bool> result = insert(value_type(key, Value()));
		return (*result.first).second;
	}

	friend void swap(map<Key, Value, Compare, Alloc> &first, map<Key, Value, Compare, Alloc> &second) noexcept {
		return first.swap(second);
	}
};



template<typename Key, typename Value,
	typename Compare, typename Alloc>
class multimap : public sx::comparetor<map<Key, Value, Compare, Alloc>> {
public:
	using key_type		= Key;
	using value_type	= std::pair<const Key, Value>;
	using key_compare	= Compare;

	/* pair */
	struct value_compare {
		Compare comp;
		value_compare(Compare comp) : comp(comp) {}
	public:
		bool operator()(value_type const &first, value_type const &second) const {
			return comp(first.first, second.first);
		}
	};
	
	/* 提取 value 中的 key 值 */
	struct key_of_value {
		key_type const &operator()(value_type const &par) const {
			return par.first;
		}
	};
private:
	using Container = sx::rbtree<key_type, value_type, key_of_value, key_compare, Alloc>;
public:
	using pointer			= typename Container::pointer;
	using reference			= typename Container::reference;
	using const_pointer		= typename Container::const_pointer;
	using const_reference	= typename Container::const_reference;
	using difference_type	= typename Container::difference_type;
	using size_type			= typename Container::size_type;
	using iterator			= typename Container::iterator;
	using const_iterator	= typename Container::const_iterator;
private:
	Container container;		/* 底层红黑树容器 */
public:
	multimap() : container(Compare()) {}

	explicit multimap(Compare const &comp) : container(comp) {}

	template<typename InputIterator,
		typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	multimap(InputIterator first, InputIterator last) {
		container.insert_equal(first, last);
	}

	multimap(multimap const &other) : container(other.container) {}

	multimap(multimap &&other) : container(std::move(other.container)) {}

	multimap &operator=(multimap const &other) {
		container = other.container;
		return *this;
	}

	multimap &operator=(multimap &&other) {
		container = std::move(other.container);
		return *this;
	}

	~multimap() {}
public:
	size_type size() const noexcept {
		return container.size();
	}

	bool empty() const noexcept {
		return container.empty();
	}

	void clear() {
		container.clear();
	}

	iterator begin() noexcept {
		return container.begin();
	}

	iterator end() noexcept {
		return container.end();
	}

	const_iterator begin() const noexcept {
		return cbegin();
	}

	const_iterator end() const noexcept {
		return cend();
	}

	const_iterator cbegin() const noexcept {
		return container.cbegin();
	}

	const_iterator cend() const noexcept {
		return container.cend();
	}

	void swap(multimap &other) {
		container.swap(other.container);
	}

	iterator insert(value_type const &val) {
		return container.insert_equal(val);
	}

	template<typename... Args>
	iterator emplace(Args&&... args) {
		return container.emplace_equal(std::forward<Args>(args)...);
	}

	template<typename InputIterator, 
		typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	void insert(InputIterator first, InputIterator last) {
		container.insert_unique(first, last);
	}

	iterator erase(iterator position) {
		return container.erase(position);
	}

	size_type erase(key_type const &key) {
		size_type count = 0;
		std::pair<iterator, iterator> range = equal_range(key);
		for (iterator iter = range.first; iter != range.second;) {
			++count;
			erase(iter++);
		}

		return count;
	}

	iterator erase(iterator postion, key_type const &key) {
		return container.erase(key);
	}

	iterator find(key_type const &key) {
		return container.find(key);
	}

	const_iterator find(key_type const &key) const {
		return container.transform_const_iterator(const_cast<multimap *>(this)->find(key));
	}

	iterator lower_bound(key_type const &key) {
		return container.lower_bound(key);
	}

	const_iterator lower_bound(key_type const &key) const {
		return container.transform_const_iterator(const_cast<multimap *>(this)->lower_bound(key));
	}

	iterator upper_bound(key_type const &key) {
		return container.upper_bound(key);
	}

	const_iterator upper_bound(key_type const &key) const {
		return container.transform_const_iterator(const_cast<multimap *>(this)->upper_bound(key));
	}

	std::pair<iterator, iterator> equal_range(key_type const &key) {
		return std::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
	}

	std::pair<const_iterator, const_iterator> equal_range(key_type const &key) const {
		std::pair<iterator, iterator> ret = const_cast<multimap *>(this)->equal_range(key);
		return std::pair<const_iterator, const_iterator>(container.transform_const_iterator(ret.first),
														 container.transform_const_iterator(ret.second));
	}

	friend void swap(multimap<Key, Value, Compare, Alloc> &first, multimap<Key, Value, Compare, Alloc> &second) noexcept {
		return first.swap(second);
	}
};

}

#endif
