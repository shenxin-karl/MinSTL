#ifndef M_UNORDERED_MAP_HPP
#define M_UNORDERED_MAP_HPP
#include "hash_table.hpp"
#include <utility>
#include <functional>

namespace sx {

template<typename Key, typename Value,
	typename HashFunc = std::hash<Key>, typename EqualFunc = std::equal_to<Key>,
	typename Alloc = sx::allocator<std::pair<const Key, Value>>>
class unordered_map;


template<typename Key, typename Value,
	typename HashFunc, typename EqualFunc,
	typename Alloc>
void swap(unordered_map<Key, Value, HashFunc, EqualFunc, Alloc> &, unordered_map<Key, Value, HashFunc, EqualFunc, Alloc> &) noexcept;


template<typename Key, typename Value,
	typename HashFunc, typename EqualFunc,
	typename Alloc>
class unordered_map {
	/* 提取 key  */
	struct extract_key_type {
		Key const &operator()(std::pair<const Key, Value> const &par) const noexcept {
			return par.first;
		}
	};
public:
	using key_type			= Key;
	using value_type		= std::pair<const Key, Value>;
private:
	using hashtable			= sx::hash_table<value_type, key_type, HashFunc, extract_key_type, EqualFunc, Alloc>;
public:
	using hasher			= typename hashtable::hasher;
	using key_equal			= typename hashtable::key_equal;
	using pointer			= typename hashtable::pointer;
	using reference			= typename hashtable::reference;
	using const_pointer		= typename hashtable::const_pointer;
	using const_reference	= typename hashtable::const_reference;
	using difference_type	= typename hashtable::difference_type;
	using size_type			= typename hashtable::size_type;
	using iterator			= typename hashtable::iterator;
	using const_iterator	= typename hashtable::const_iterator;
private:
	hashtable				table;		/* 底层 hash table 容器 */
public:
	unordered_map() : table(100, HashFunc(), EqualFunc()) { }

	explicit unordered_map(size_type n) : table(n, HashFunc(), EqualFunc()) { }

	unordered_map(size_type n, HashFunc const &hash_func, EqualFunc const &equal_func)
		: table(n, hash_func, equal_func) { }

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_map(InputIterator first, InputIterator last)
		: table(100, HashFunc(), EqualFunc()) {
		table.insert_unique(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_map(InputIterator first, InputIterator last, size_type n)
		: table(n, HashFunc(), EqualFunc()) {
		table.insert_unique(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_map(InputIterator first, InputIterator last, size_type n, hasher const &hash_func)
		: table(n, hash_func, EqualFunc()) {
		table.insert_unique(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_map(InputIterator first, InputIterator last, size_type n,
			hasher const &hash_func, key_equal const &eqaul_func)
		: table(n, hash_func, eqaul_func) {
		table.insert_unique(first, last);
	}

	unordered_map(unordered_map const &other) : table(other.table) { }

	unordered_map(unordered_map &&other) : table(std::move(other.table)) { }

	unordered_map &operator=(unordered_map const &other) {
		if (this != &other)
			table = other.table;
		return *this;
	}

	unordered_map &operator=(unordered_map &&other) {
		if (this != &other) {
			unordered_map tmp = std::move(other);
			table = std::move(tmp.table);
		}
		return *this;
	}

	~unordered_map() = default;
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
		return table.begin();
	}

	iterator end() noexcept {
		return table.end();
	}

	const_iterator cbegin() const noexcept {
		return table.cbegin();
	}

	const_iterator cend() const noexcept {
		return table.cend();
	}

	const_iterator begin() const noexcept {
		return cbegin();
	}

	const_iterator end() const noexcept {
		return cend();
	}

	template<typename T, 
		typename = std::enable_if_t<std::is_convertible_v<T, value_type>>>
	std::pair<iterator, bool> insert(T const &val) {
		return table.insert_unique(val);
	}

	std::pair<iterator, bool> insert(value_type const &val) {
		return table.insert_unique(val);
	}

	template<typename InputIterator,
		typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	void insert(InputIterator first, InputIterator last) {
		table.insert_unique(first, last);
	}

	template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args) {
		return table.emplace_unique(std::forward<Args>(args) ...);
	}

	size_type erase(key_type const &key) {
		return table.erase(key);
	}

	iterator erase(iterator pos) {
		return table.erase(pos);
	}

	iterator erase(iterator first, iterator last) {
		return table.erase(first, last);
	}

	iterator find(key_type const &key) const {
		return table.find(key);
	}

	size_type count(key_type const &key) const {
		return table.count(key);
	}

	std::pair<iterator, iterator> equal_range(key_type const &key) const {
		return table.equal_range(key);
	}

	void swap(unordered_map &other) noexcept {
		table.swap(other.table);
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

	Value &operator[](key_type const &key) {
		std::pair<iterator, bool> ret = table.insert_unique(value_type());
		return (*(ret.first)).second;
	}
};

template<typename Key, typename Value,
	typename HashFunc, typename EqualFunc,
	typename Alloc> inline 
void swap(unordered_map<Key, Value, HashFunc, EqualFunc, Alloc> &lhs, unordered_map<Key, Value, HashFunc, EqualFunc, Alloc> &rhs) noexcept {
	return lhs.swap(rhs);
}


template<typename Key, typename Value, 
	typename HashFunc = std::hash<Key>, typename EqualKey = std::equal_to<Key>, 
	typename Alloc = sx::allocator<std::pair<const Key, Value>>>
class unordered_multimap;

template<typename Key, typename Value, 
	typename HashFunc, typename EqualFunc,
	typename Alloc>
void swap(unordered_multimap<Key, Value, HashFunc, EqualFunc, Alloc> &, unordered_multimap<Key, Value, HashFunc, EqualFunc, Alloc> &) noexcept;


template<typename Key, typename Value, 
	typename HashFunc, typename EqualFunc,
	typename Alloc>
class unordered_multimap {
	using hashtable = sx::hash_table<Key, Key, HashFunc, sx::identity<Key>, EqualFunc, Alloc>;
	using hasher = typename hashtable::hasher;
	using key_equal = typename hashtable::key_equal;
public:
	using key_type = typename hashtable::key_type;
	using value_type = typename hashtable::value_type;
	using pointer = typename hashtable::pointer;
	using reference = typename hashtable::reference;
	using const_pointer = typename hashtable::const_pointer;
	using const_reference = typename hashtable::const_reference;
	using difference_type = typename hashtable::difference_type;
	using size_type = typename hashtable::size_type;
	using iterator = typename hashtable::const_iterator;
	using const_iterator = typename hashtable::const_iterator;
public:
	hashtable	table;		/* 底层 hashtable 容器 */
public:
	unordered_multimap() : table(100, HashFunc(), EqualFunc()) { }

	explicit unordered_multimap(size_type n) : table(n, HashFunc(), EqualFunc()) { }

	unordered_multimap(size_type n, HashFunc const &hash_func, EqualFunc const &equal_func)
		: table(n, hash_func, equal_func) { }

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_multimap(InputIterator first, InputIterator last)
		: table(100, HashFunc(), EqualFunc()) {
		table.insert_equal(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_multimap(InputIterator first, InputIterator last, size_type n)
		: table(n, HashFunc(), EqualFunc()) {
		table.insert_equal(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_multimap(InputIterator first, InputIterator last, size_type n, hasher const &hash_func)
		: table(n, hash_func, EqualFunc()) {
		table.insert_equal(first, last);
	}

	template<typename InputIterator,
		std::enable_if_t<sx::is_input_iterator_v<InputIterator>
		&& sx::is_convertible_iter_type_v<InputIterator, value_type>>>
	unordered_multimap(InputIterator first, InputIterator last, size_type n,
			hasher const &hash_func, key_equal const &eqaul_func)
		: table(n, hash_func, eqaul_func) {
		table.insert_equal(first, last);
	}

	unordered_multimap(unordered_multimap const &other) : table(other.table) { }

	unordered_multimap(unordered_multimap &&other) : table(std::move(other.table)) { }

	unordered_multimap &operator=(unordered_multimap const &other) {
		if (this != &other)
			table = other.table;
		return *this;
	}

	unordered_multimap &operator=(unordered_multimap &&other) {
		if (this != &other) {
			unordered_multimap tmp = std::move(other);
			table = std::move(tmp.table);
		}
		return *this;
	}

	~unordered_multimap() = default;
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

	void swap(unordered_multimap &other) noexcept {
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
void swap(unordered_multimap<Key, HashFunc, EqualFunc, Alloc> &lhs, unordered_multimap<Key, HashFunc, EqualFunc, Alloc> &rhs) noexcept {
	return lhs.swap(rhs);
}


}

#endif // !M_UNORDERED_MAP_HPP
