#ifndef M_UNORDERED_SET_HPP
#define M_UNORDERED_SET_HPP
#include "hash_table.hpp"
#include "type_traits.hpp"
#include <functional>

namespace sx {

template<typename Key, typename HashFunc = std::hash<Key>,
	typename EqualFunc = std::equal_to<Key>, typename Alloc = sx::allocator<Key>>
class unordered_set;

template<typename Key, typename HashFunc,
	typename EqualFunc, typename Alloc>
class unordered_set {
	using hashtable = sx::hash_table<Key, Key, HashFunc, sx::identity<Key>, EqualFunc, Alloc>;
	using hasher			= typename hashtable::hasher;
	using key_equal			= typename hashtable::key_equal;
public:
	using key_type			= typename hashtable::key_type;
	using value_type		= typename hashtable::value_type;
	using pointer			= typename hashtable::pointer;
	using reference			= typename hashtable::reference;
	using const_pointer		= typename hashtable::const_pointer;
	using const_reference	= typename hashtable::const_reference;
	using difference_type	= typename hashtable::difference_type;
	using size_type			= typename hashtable::size_type;
	using iterator			= typename hashtable::const_iterator;
	using const_iterator	= typename hashtable::const_iterator;
public:
	hashtable	table;		/* µ×²ã hashtable ÈÝÆ÷ */
public:
	unordered_set() : table(100, HashFunc(), EqualFunc()) { }

	explicit unordered_set(size_type n) : table(n, HashFunc(), EqualFunc()) { }

	unordered_set(size_type n, HashFunc const &hash_func, EqualFunc const &equal_func)
		: table(n, hash_func, equal_func) { }

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_set(InputIterator first, InputIterator last)
		: table(100, HashFunc(), EqualFunc()) {
		table.insert_unique(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_set(InputIterator first, InputIterator last, size_type n)
		: table(n, HashFunc(), EqualFunc()) {
		table.insert_unique(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_set(InputIterator first, InputIterator last, size_type n, hasher const &hash_func)
		: table(n, hash_func, EqualFunc()) {
		table.insert_unique(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_set(InputIterator first, InputIterator last, size_type n, 
		hasher const &hash_func, key_equal const &eqaul_func)
		: table(n, hash_func, eqaul_func) {
		table.insert_unique(first, last);
	}

	unordered_set(unordered_set const &other) : table(other.table) { }

	unordered_set(unordered_set &&other) : table(std::move(other.table)) { }

	unordered_set &operator=(unordered_set const &other) {
		if (this != &other)
			table = other.table;
		return *this;
	}

	unordered_set &operator=(unordered_set &&other) {
		if (this != &other) {
			unordered_set tmp = std::move(other);
			table = std::move(tmp.table);
		}
		return *this;
	}

	~unordered_set() = default;
public:
	size_type size() const noexcept {
		return table.size();
	}

	bool empty() const noexcept {
		return table.empty();
	}

	static size_type max_size() noexcept {
		return hashtable::max_size();
	}

	iterator begin() noexcept {
		return static_cast<iterator>(table.begin());
	}

	iterator end() noexcept {
		return static_cast<iterator>(table.end());
	}

	const_iterator begin() const noexcept {
		return cbegin();
	}

	const_iterator end() const noexcept {
		return cend();
	}

	const_iterator cbegin() const noexcept {
		return table.cbegin();
	}

	const_iterator cend() const noexcept {
		return table.cend();
	}

	void swap(unordered_set &other) noexcept {
		sx::swap(table, other.table);
	}
	
	iterator find(key_type const &key) const {
		return static_cast<iterator>(table.find(key));
	}

	size_type count(key_type const &key) const {
		return table.count(key);
	}

	std::pair<const_iterator, const_iterator> equal_range(key_type const &key) const {
		using hahs_iterator = typename hashtable::iterator;
		std::pair<hahs_iterator, hahs_iterator> ret = table.equal_range(key);
		return { static_cast<const_iterator>(ret.first), static_cast<const_iterator>(ret.second) };
	}

	std::pair<iterator, bool> insert(value_type const &val) {
		std::pair<typename hashtable::iterator, bool> ret = table.insert_unique(val);
		return { static_cast<iterator>(ret.first), ret.second };
	}

	template<typename InpuIterator,
		typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	void insert(InpuIterator first, InpuIterator last) {
		return static_cast<iterator>(table.insert_unique(first, last));
	}

	template<typename... Args>
	iterator emplace(Args&&... args) {
		return static_cast<iterator>(table.emplace_unique(std::forward<Args>(args)...));
	}

	size_type erase(key_type const &key) {
		return table.erase(key);
	}

	iterator erase(iterator pos) {
		using hash_iterator = typename hashtable::iterator;
		return static_cast<iterator>(table.erase(static_cast<hash_iterator>(pos)));
	}

	iterator erase(iterator first, iterator last) {
		using hash_iterator = typename hashtable::iterator;
		return static_cast<iterator>(table.erase(static_cast<hash_iterator>(first), static_cast<hash_iterator>(last)));
	}

	void clear() {
		table.clear();
	}

	void resize(size_type n) noexcept {
		return table.resize(n);
	}

	size_type bucket_count() const noexcept {
		return table.bucket_count();
	}

	static size_type max_bucket_count() noexcept {
		return hashtable::max_bucket_count();
	}
};


template<typename Key, typename HashFunc = std::hash<Key>,
	typename EqualFunc = std::equal_to<Key>, typename Alloc = sx::allocator<Key>>
class unordered_multiset;

template<typename Key, typename HashFunc,
	typename EqualFunc, typename Alloc>
class unordered_multiset {
	using hashtable			= sx::hash_table<Key, Key, HashFunc, sx::identity<Key>, EqualFunc, Alloc>;
	using hasher			= typename hashtable::hasher;
	using key_equal			= typename hashtable::key_equal;
public:
	using key_type			= typename hashtable::key_type;
	using value_type		= typename hashtable::value_type;
	using pointer			= typename hashtable::pointer;
	using reference			= typename hashtable::reference;
	using const_pointer		= typename hashtable::const_pointer;
	using const_reference	= typename hashtable::const_reference;
	using difference_type	= typename hashtable::difference_type;
	using size_type			= typename hashtable::size_type;
	using iterator			= typename hashtable::const_iterator;
	using const_iterator	= typename hashtable::const_iterator;
public:
	hashtable	table;		/* µ×²ã hashtable ÈÝÆ÷ */
public:
	unordered_multiset() : table(100, HashFunc(), EqualFunc()) { }

	explicit unordered_multiset(size_type n) : table(n, HashFunc(), EqualFunc()) { }

	unordered_multiset(size_type n, HashFunc const &hash_func, EqualFunc const &equal_func)
		: table(n, hash_func, equal_func) { }

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_multiset(InputIterator first, InputIterator last)
		: table(100, HashFunc(), EqualFunc()) {
		table.insert_equal(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_multiset(InputIterator first, InputIterator last, size_type n)
		: table(n, HashFunc(), EqualFunc()) {
		table.insert_equal(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_multiset(InputIterator first, InputIterator last, size_type n, hasher const &hash_func)
		: table(n, hash_func, EqualFunc()) {
		table.insert_equal(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_multiset(InputIterator first, InputIterator last, size_type n,
		hasher const &hash_func, key_equal const &eqaul_func)
		: table(n, hash_func, eqaul_func) {
		table.insert_equal(first, last);
	}

	unordered_multiset(unordered_multiset const &other) : table(other.table) { }

	unordered_multiset(unordered_multiset &&other) : table(std::move(other.table)) { }

	unordered_multiset &operator=(unordered_multiset const &other) {
		if (this != &other)
			table = other.table;
		return *this;
	}

	unordered_multiset &operator=(unordered_multiset &&other) {
		if (this != &other) {
			unordered_multiset tmp = std::move(other);
			table = std::move(tmp.table);
		}
		return *this;
	}

	~unordered_multiset() = default;
public:
	size_type size() const noexcept {
		return table.size();
	}

	bool empty() const noexcept {
		return table.empty();
	}

	static size_type max_size() noexcept {
		return hashtable::max_size();
	}

	iterator begin() noexcept {
		return static_cast<iterator>(table.begin());
	}

	iterator end() noexcept {
		return static_cast<iterator>(table.end());
	}

	const_iterator begin() const noexcept {
		return cbegin();
	}

	const_iterator end() const noexcept {
		return cend();
	}

	const_iterator cbegin() const noexcept {
		return table.cbegin();
	}

	const_iterator cend() const noexcept {
		return table.cend();
	}

	void swap(unordered_multiset &other) noexcept {
		sx::swap(table, other.table);
	}
	
	iterator find(key_type const &key) const {
		return static_cast<iterator>(table.find(key));
	}

	size_type count(key_type const &key) const {
		return table.count(key);
	}

	std::pair<const_iterator, const_iterator> equal_range(key_type const &key) const {
		using hash_iterator = typename hashtable::iterator;
		 std::pair<hash_iterator, hash_iterator> ret = table.equal_range(key);
		return { static_cast<const_iterator>(ret.first), static_cast<const_iterator>(ret.second) };
	}

	iterator insert(value_type const &val) {
		typename hashtable::iterator ret = table.insert_equal(val);
		return static_cast<iterator>(ret);
	}

	template<typename InpuIterator,
		typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	void insert(InpuIterator first, InpuIterator last) {
		return static_cast<iterator>(table.insert_equal(first, last));
	}

	template<typename... Args>
	iterator emplace(Args&&... args) {
		return static_cast<iterator>(table.emplace_equal(std::forward<Args>(args)...));
	}

	size_type erase(key_type const &key) {
		return table.erase(key);
	}

	iterator erase(iterator pos) {
		using hash_iterator = typename hashtable::iterator;
		return static_cast<iterator>(table.erase(static_cast<hash_iterator>(pos)));
	}

	iterator erase(iterator first, iterator last) {
		using hash_iterator = typename hashtable::iterator;
		return static_cast<iterator>(table.erase(static_cast<hash_iterator>(first), static_cast<hash_iterator>(last)));
	}

	void clear() {
		table.clear();
	}

	void resize(size_type n) noexcept {
		return table.resize(n);
	}

	size_type bucket_count() const noexcept {
		return table.bucket_count();
	}

	static size_type max_bucket_count() noexcept {
		return hashtable::max_bucket_count();
	}
};


template<typename Key, typename HashFunc, typename EqualFunc, typename Alloc>
void swap(unordered_set<Key, HashFunc, EqualFunc, Alloc> &lhs, unordered_set<Key, HashFunc, EqualFunc, Alloc> &rhs) noexcept {
	return lhs.swap(rhs);
}

template<typename Key, typename HashFunc, typename EqualFunc, typename Alloc>
void swap(unordered_multiset<Key, HashFunc, EqualFunc, Alloc> &lhs, unordered_multiset<Key, HashFunc, EqualFunc, Alloc> &rhs) noexcept {
	return lhs.swap(rhs);
}

}
#endif // !M_UNORDERED_SET_HPP