#ifndef M_VECTOR_HPP
#define M_VECTOR_HPP
#include "mallocator.hpp"
#include "miterator.hpp"
#include <cstddef>
#include <initializer_list>
#include <algorithm>

namespace sx {
class vector_empty : public std::exception {
};

template<typename T, typename Alloc = allocator<T>>
class vector {
public:
	using value_type = T;
	using pointer = value_type *;
	using referenc = value_type &;
	using iterator = value_type *;
	using const_iterator = value_type const *;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using const_pointer = T const *;
	using const_reference = T const &;
protected:
	static Alloc	allocator;
	iterator		start;				/* 使用空间开始位置 */
	iterator		finish;			/* 使用空间结束位置 */
	iterator		end_of_store;		/* 存储空间结尾位置 */
protected:
	template<typename Func1, typename Func2>
	void __insert_aux(Func1 &func1, Func2 &func2, std::true_type) {
		func1();
	}

	template<typename Func1, typename Func2>
	void __insert_aux(Func1 &func1, Func2 &func2, std::false_type) {
		func2();
	}

	/* 插入辅助函数 */
	void insert_aux(iterator position, value_type const &value) {
		const size_type old_size = size();
		const size_type new_size = old_size != 0 ? old_size * 2 : 1;
		iterator new_start = allocator.allocate(new_size);
		iterator new_finish = new_start;

		auto has_noexcept_func = [&]()->void {
			new_finish = sx::uninitialized_copy(std::make_move_iterator(start),
											std::make_move_iterator(position),
											new_start);
			allocator.construct(new_finish, value);
			++new_finish;
			new_finish = sx::uninitialized_copy(std::make_move_iterator(position),
												std::make_move_iterator(finish),
												new_finish);
		};

		auto has_not_noexcept_func = [&]()->void {
			try {
				new_finish = sx::uninitialized_copy(start, position, new_start);
				allocator.construct(new_finish, value);
				new_finish = sx::uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				allocator.destroy(new_start, new_finish);
				allocator.deallocate(new_start, new_size);
				throw;
			}
		};

		__insert_aux(has_noexcept_func, has_not_noexcept_func, has_noexcept_move_construct_t<T>());
		allocator.construct(finish, value);
		++finish;
	}

	void deallocate() {
		if (start != nullptr)
			allocator.deallocate(start, (end_of_store - start));
	}

	void copy_backward(iterator first, iterator end, iterator desc) {
		std::ptrdiff_t distan = distance(first, end);
		desc += distan;
		for (iterator iter = end-1; iter >= first; iter--, desc--)
			*desc = *iter;
	}

	/* 初始化函数 */
	static void fill_initialize(size_type n, value_type const &value) {
		start = allocate_and_fill(n, value);
		finish = start + n;
		end_of_store = finish;
	}

	/* 分配并且进行初始化 */
	static iterator allocate_and_fill(size_type n, value_type const &value) {
		iterator result = allocator.allocate(n);
		uninitialized_fill_n(result, n, value);
		return result;
	}
public:
	vector() : start(nullptr), finish(nullptr), end_of_store(nullptr) {
	}

	vector(size_type n, value_type const &value) { 
		fill_initialize(n, value); 
	}

	vector(int n, value_type const &value) { 
		fill_initialize(n, value); 
	}

	vector(long n, value_type const &value) { 
		fill_initialize(n, value); 
	}
	
	explicit vector(size_type n) { 
		fill_initialize(n, T{}); 
	}
	
	vector(std::initializer_list<T> const &list) {
		start = allocator.allocate(list.size() + 1);
		uninitialized_copy(list.begin(), list.end(), start);
		finish = start + list.size();
		end_of_store = finish + 1;
	}

	vector(vector const &other) {
		if (other.empty()) {
			start = finish = end_of_store = nullptr;
			return;
		}

		const size_type other_size = other.end_of_store - other.start;
		start = allocator.allocate(other_size);
		finish = start;
		finish = uninitialized_copy(other.start, other.finish);
		end_of_store = start + other_size;
	}

	vector &operator=(vector other) {
		swap(*this, other);
	}

	vector(vector &&other) 
	: start(other.start), finish(other.finish), end_of_store(other.end_of_store) {
		other.start = other.finish = other.end_of_store = nullptr;
	}

	vector &operator=(vector &&other) {
		if (this == &other)
			return *this;

		~vector();
		start = other.start;
		finish = other.finish;
		end_of_store = other.end_of_store;
		other.start = other.finish = other.end_of_store = nullptr;
		return *this;
	}

	~vector() {
		allocator.destroy(start, finish);
		deallocate();
	}

	iterator begin() noexcept {
		return start;
	}

	const_iterator cbegin() noexcept {
		return static_cast<const_iterator>(start);
	}

	iterator end() noexcept {
		return finish;
	}

	const_iterator cend() noexcept {
		return static_cast<const_iterator>(finish);
	}

	size_type size() const noexcept {
		return finish - start;
	}

	bool empty() const noexcept {
		return size() == 0;
	}

	size_type capacity() const noexcept {
		return end_of_store - start;
	}

	referenc operator[](size_type n) {
		return *(start + n);
	}

	referenc front() {
		return *start;
	}

	const_reference front() const {
		return *start;
	}

	referenc back() {
		return *(finish-1);
	}

	const_reference back() const {
		return *(finish-1);
	}

	void push_back(value_type const &value) {
		if (finish != end_of_store) {
			allocator.construct(finish, value);
			++finish;
		}
		else
			insert_aux(end(), value);
	}

	void pop_back() {
		if (empty())
			throw vector_empty();

		--finish;
		allocator.destroy(finish);
	}

	template<typename... Args>
	void emplace_back(Args&&... args) {
		if (finish != end_of_store) {
			allocator.construct(finish, std::forward<Args>(args)...);
			++finish;
			return;
		}

		const size_type old_size = size();
		const size_type new_size = old_size * 2;
		iterator new_start = allocator.allocate(new_size);
		iterator new_finish = new_start;
		if constexpr (has_noexcept_move_construct<value_type>::value)
			new_finish = uninitialized_copy(std::make_move_iterator(start), 
											std::make_move_iterator(finish), 
											new_finish);
		else {
			try {
				new_finish = uninitialized_copy(start, finish, new_finish);
			}
			catch (...) {
				allocator.destroy(new_start, new_finish);
				allocator.deallocate(new_start);
				throw;
			}
		}

		allocator.construct(new_finish, std::forward<Args>(args)...);
		++new_finish;

		allocator.destroy(start, finish);
		deallocate();
		start = new_start;
		finish = new_finish;
		end_of_store = new_start + new_size;
	}

	template<typename... Args>
	void emplace_insert(iterator position, Args&&... args) {
		insert_aux(position, value_type(std::forward<Args>(args)...));
	}

	iterator erase(iterator position) {
		if (position + 1 != end())
			std::copy(position + 1, finish, position);
		
		--finish;
		allocator.destroy(finish);
		return position;
	}

	iterator erase(iterator first, iterator last) {
		iterator i = std::copy(last, finish, first);
		allocator.destroy(i, finish);
		finish = finish - (last - first);
		return first;
	}

	void clear() {
		erase(begin(), end());
	}

	void insert(iterator position, size_type n, value_type const &value) {
		if (n == 0)
			return;

		/* 空间足够 */
		if (size_type(end_of_store - finish) >= n) {
			value_type value_copy = value;
			const size_type element_after = finish - position;
			iterator old_finish = finish;
			if (element_after > n) {
				uninitialized_copy(finish - n, finish, finish);
				finish += n;
				copy_backward(position, old_finish - n, old_finish);
				fill(position, position + n, value_copy);
			}
			else {
				uninitialized_fill_n(finish, n - element_after, value_copy);
				finish += n - element_after;
				uninitialized_copy(position, old_finish, finish);
				finish += old_finish - position;
				fill(position, old_finish, value_copy);
			}
		}
		/* 空间不足 */
		else {
			const size_type old_size = size();
			const size_type new_size = old_size + std::max(old_size, n);
			iterator new_start = allocator.allocate(new_size);
			iterator new_finish = new_start;
			if constexpr (has_noexcept_move_construct<value_type>::value) {
				new_finish = uninitialized_copy(std::make_move_iterator(start), 
												std::make_move_iterator(position), new_finish);
				new_finish = uninitialized_fill_n(new_finish, n, value);
				new_finish = uninitialized_copy(std::make_move_iterator(position),
												std::make_move_iterator(finish), new_finish);
			} else {
				try {
					new_finish = uninitialized_copy(start, position, new_finish);
					new_finish = uninitialized_fill_n(new_finish, n, value);
					new_finish = uninitialized_copy(position, finish, new_finish);
				}
				catch (...) {
					allocator.destroy(new_start, new_finish);
					allocator.deallocate(new_start, new_size);
					throw;
				}
				allocator.destroy(start, end);
				deallocate();
				start = new_start;
				finish = new_finish;
				end_of_store = new_start + new_size;
			}
		}
	}

	void insert(iterator position, value_type const &value) {
		insert_aux(position, value);
	}

	void insert(const_iterator position, value_type const &value) {
		insert_aux(reinterpret_cast<iterator>(position), value);
	}

	friend void swap(vector &first, vector &second) noexcept {
		std::swap(first.start, second.start);
		std::swap(first.finish, second.finish);
		std::swap(first.end_of_store, second.end_of_store);
	}
};

}

#endif