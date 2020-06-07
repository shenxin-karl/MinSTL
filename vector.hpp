#ifndef M_VECTOR_HPP
#define M_VECTOR_HPP
#include "allocator.hpp"
#include "iterator.hpp"
#include "utility.hpp"
#include <cstddef>
#include <initializer_list>
#include <algorithm>

namespace sx {

class vector_empty : public std::exception {
};

template<typename T, typename Alloc = sx::allocator<T>>
class vector;

template<typename T, typename Alloc>
void swap(vector<T, Alloc> &, vector<T, Alloc> &) noexcept;

template<typename T, typename Alloc>
class vector : public sx::comparetor<vector<T, Alloc>> {
public:
	using value_type 		= T;
	using size_type 		= std::size_t;
	using difference_type 	= std::ptrdiff_t;
	using pointer 			= T *;
	using reference 		= T &;
	using const_pointer 	= T const *;
	using const_reference 	= T const &;
	using iterator 			= T *;
	using const_iterator 	= T const *;
private:
	static Alloc 		allocator;			/* 分配器 */
	iterator 			start;				/* 使用空间开始 */
	iterator 			finish;				/* 使用空间末尾 */
	iterator 			end_of_store;		/* 可用空间末尾 */
public:
	vector() : start(nullptr), finish(nullptr), end_of_store(nullptr) {}

	vector(size_type n, value_type const &val) {
		start = allocator.allocate(n);
		sx::uninitialized_fill_n(start, n, val);
		finish = end_of_store = start + n;
	}

	vector(vector const &other) {
		start = alloc_and_fill(other.begin(), other.end());
		finish = start + other.size();
		end_of_store = start + other.size();
	}

	vector(vector &&other) 
	: start(other.start), finish(other.finish), end_of_store(other.end_of_store) {
		other.start = other.finish = other.end_of_store = nullptr;
	}

	vector &operator=(vector const &other) {
		vector tmp = other;
		swap(*this, tmp);
		return *this; 
	}

	vector &operator=(vector &&other) {
		vector tmp = std::move(other);
		swap(*this, tmp);
		return *this;
	}

	~vector() {
		allocator.destroy(start, finish);
		deallocate();
	}

	vector(std::initializer_list<T> const &list) {
		start = alloc_and_fill(list.begin(), list.end());
		finish = end_of_store = start + list.size();
	}

	template<typename InputIterator, 
			 typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>>>
	vector(InputIterator first, InputIterator end) {
		difference_type distance = sx::distance(first, end);
		start = allocator.allocate(distance);
		try {
			finish = uninitialized_copy(first, end, start);
			end_of_store = finish;
		} catch (...) {
			allocator.deallocate(start, distance);
			throw;
		}
	}
private:
	void deallocate() {
		allocator.deallocate(start, end_of_store - start);
	}

	template<typename InputIterator>
	static iterator alloc_and_fill(InputIterator first, InputIterator end) {
		difference_type distance = sx::distance(first, end);
		iterator result = allocator.allocate(distance);
		try {
			uninitialized_copy(first, end, result);
			return result;
		} catch (...) {
			allocator.deallocate(result, distance);
			throw;
		}
	}

	template<typename Func1, typename Func2>
	static void conditional_execute(Func1 &func1, Func2 &func2, std::true_type) {
		func1();
	}

	template<typename Func1, typename Func2>
	static void conditional_execute(Func1 &func1, Func2 &func2, std::false_type) {
		func2();
	}

	/* 插入辅助函数, 当到达 position 位置时, 调用 func 闭包函数立即构造元素 */
	template<typename ConstructFunc>
	iterator insert_aux(iterator position, ConstructFunc const &construct_func) {
		if (finish != end_of_store) {
			if (position == end()) {
				++finish;
				construct_func(position);
				return position;
			}

			allocator.construct(finish, *(finish - 1));
			++finish;
			std::copy_backward(position, finish - 2, finish - 1);
			construct_func(position);
			return position;
		}

		const size_type old_size = capacity();
		const size_type new_size = old_size != 0 ? old_size * 2 : 1;
		iterator new_start = allocator.allocate(new_size);
		iterator new_finish = new_start;
		iterator result;

		/* 使用移动构造, 移动元素 */
		auto has_noexcept_move_construct = [&]()->void {
			new_finish = sx::uninitialized_copy(std::make_move_iterator(start), 
												std::make_move_iterator(position), new_start);
			construct_func(new_finish);
			result = new_finish;
			++new_finish;
			new_finish = sx::uninitialized_copy(std::make_move_iterator(position), 
												std::make_move_iterator(finish), new_finish);
		};
		
		/* 使用拷贝构造, 移动元素 */
		auto has_not_noexcept_move_construct = [&]()->void {
			try {
				new_finish = sx::uninitialized_copy(start, position, new_start);
				construct_func(new_finish);
				result = new_finish;
				++new_finish;
				new_finish = sx::uninitialized_copy(position, finish, new_finish);
			} catch(...) {
				for (iterator beg = new_start; beg != new_finish; ++beg)
					allocator.destroy(beg);
				allocator.deallocate(new_start, new_size);
				throw;
			}
		};
		conditional_execute(has_noexcept_move_construct, 
					 		has_not_noexcept_move_construct, has_noexcept_move_construct_t<T>{});
		allocator.destroy(start, finish);
		deallocate();
		start = new_start;
		finish = new_finish;
		end_of_store = start + new_size;
		return result;
	}
public:
	size_type size() const noexcept {
		return finish - start;
	}

	bool empty() const noexcept {
		return size() == 0;
	}

	size_type capacity() const noexcept {
		return end_of_store - start;
	}

	iterator begin() noexcept {
		return start;
	}

	iterator end() noexcept {
		return finish;
	}

	const_iterator begin() const noexcept {
		return start;
	}

	const_iterator end() const noexcept {
		return finish;
	}

	const_iterator cbegin() const {
		return start;
	}

	const_iterator cend() const {
		return finish;
	}

	reference front() {
		return *start;
	}

	const_reference front() const {
		return *start;
	}

	reference back() {
		return *(finish - 1);
	}

	const_reference back() const {
		return *(finish - 1);
	}

	void pop_back() {
		if (empty())
			throw vector_empty();

		--finish;
		allocator.destroy(finish);
	}

	iterator insert(iterator position, value_type const &value) {
		auto construct_func = [&](iterator pos) {
			allocator.construct(pos, value);
		};
		return insert_aux(position, construct_func);
	}

	template<typename... Args>
	iterator emplace(iterator position, Args&&... args) {
		auto construct_func = [&](iterator pos) {
			allocator.construct(pos, std::forward<Args>(args)...);
		};
		return insert_aux(position, construct_func);
	}

	void push_back(value_type const &value) {
		insert_aux(end(), [&](iterator pos) {
			allocator.construct(pos, value);
		});
	}

	template<typename... Args>
	void emplace_back(Args&&... args) {
		insert_aux(end(), [&](iterator pos) {
			allocator.construct(pos, std::forward<Args>(args)...);
		});
	}

	iterator insert(iterator position, size_type size, value_type const &value) {
		if (size == 0)
			return position;

		size_type store_left = end_of_store - finish;
		/* 不同开辟新空间 */
		if (size <= store_left) {
			size_type element_after = finish - position;
			if (element_after > size) {
				sx::uninitialized_copy(finish - size, finish, finish);
				std::copy_backward(position, finish - size, finish);
				std::fill_n(position, size, value);
			} else {
				sx::uninitialized_copy(position, finish, finish + (size - element_after));
				sx::uninitialized_fill_n(finish, size - element_after, value);
				std::fill_n(position, finish - position, value);
			}
			finish += size;
			return position + size + 1;
		} else {
			size_type old_size = capacity();
			size_type new_size = old_size != 0 ? std::max(old_size * 2, size + old_size) : size;
			iterator new_start = allocator.allocate(new_size);
			iterator new_finish = new_start;
			iterator ret;
			
			/* 使用移动构造, 移动元素 */
			auto has_noexcept_move_construct = [&]()->void {
				new_finish = sx::uninitialized_copy(std::make_move_iterator(start),
									   				std::make_move_iterator(position), new_start);
				new_finish = sx::uninitialized_fill_n(new_finish, size, value);
				ret = new_finish;
				new_finish = sx::uninitialized_copy(std::make_move_iterator(position), 
													std::make_move_iterator(finish), new_finish);
			};

			/* 使用拷贝构造, 移动元素 */
			auto has_not_noexcept_move_construct = [&]()->void {
				try {
					new_finish = sx::uninitialized_copy(start, position, new_finish);
					new_finish = sx::uninitialized_fill_n(new_finish, size, value);
					ret = new_finish;
					new_finish = sx::uninitialized_copy(position, finish, new_finish);
				} catch(...) {
					for (iterator beg = new_start; beg != new_finish; ++beg) 
						allocator.destroy(beg);
					allocator.deallocate(new_start, new_size);
					throw;
				}
			};
			conditional_execute(has_noexcept_move_construct, has_noexcept_move_construct,
						 		has_noexcept_move_construct_t<T>{});
			allocator.destroy(start, finish);
			deallocate();
			start = new_start;
			finish = new_finish;
			end_of_store = new_start + new_size;
			return ret;
		}
	}

	const_iterator insert(const_iterator position, std::size_t size, value_type const &value) {
		iterator ret = insert(const_cast<iterator>(position), size, value);
		return const_cast<const_iterator>(ret);
	}
	
	const_iterator insert(const_iterator position, value_type const &value) {
		iterator ret = insert(const_cast<iterator>(position), value);
		return const_cast<const_iterator>(ret);
	}

	iterator erase(iterator begin, iterator end) {
		iterator iter = std::copy(end, finish, begin);
		allocator.destroy(iter, finish);
		finish = iter;
		return begin;
	}

	iterator erase(iterator position) {
		if (position+1 != end()) 
			std::copy(position+1, finish, position);
		--finish;
		allocator.destroy(finish);
		return position;
	}

	const_iterator erase(const_iterator begin, const_iterator end) {
		iterator ret = erase(const_cast<iterator>(begin), const_cast<iterator>(end));
		return const_cast<const_iterator>(ret);
	}

	const_iterator erase(const_iterator postion) {
		iterator ret = erase(const_cast<iterator>(postion));
		return const_cast<const_iterator>(ret);
	}

	void clear() {
		erase(begin(), end());
	}

	reference operator[](std::size_t index) {
		return start[index];
	}

	const_reference operator[](std::size_t index) const {
		return start[index];
	}

	reference at(std::size_t index) {
		if (index > size())
			throw std::out_of_range("invalid index");

		return (*this)[index];
	}

	const_reference at(std::size_t index) const {
		if (index > size())
			throw std::out_of_range("invalid index");

		return (*this)[index];
	}

	void reserve(size_type reserve_size) {
		if (reserve_size < capacity())
			return;

		iterator new_start = allocator.allocate(reserve_size);
		iterator new_finish = new_start;
		
		/* 移动构造移动元素 */
		auto has_noexcept_move_construct = [&]()->void {
			new_finish = sx::uninitialized_copy(std::make_move_iterator(start), 
												std::make_move_iterator(finish), new_start);
		};

		/* 拷贝构造移动元素 */
		auto has_not_noexcept_move_construct = [&]()->void {
			try {
				new_finish = sx::uninitialized_copy(start, finish, new_start);
			} catch(...) {
				allocator.deallocate(new_start, reserve_size);
				throw;
			}
		};
		conditional_execute(has_noexcept_move_construct, has_not_noexcept_move_construct,
							sx::has_noexcept_move_construct_t<value_type>{});
		allocator.deallocate(start, capacity());
		start = new_start;
		finish = new_finish;
		end_of_store = start + reserve_size;
	}


	void resize(size_type size, value_type const &value = value_type{}) {
		if (size == this->size())
			return;

		/* 新大小缩小空间 */
		if (size < this->size()) {
			allocator.destroy(start + size, finish);
			finish = start + size;
		
		/* 增加空间 */
		} else {
			const size_type old_size = capacity();
			if (size > capacity()) {
				const size_type new_size = std::max(2 * old_size, old_size + size);
				reserve(new_size);
			}
			finish = sx::uninitialized_fill_n(start + old_size, size - old_size, value);
		}
	}

	void swap(vector &other) noexcept {
		using std::swap;
		swap(start, other.start);
		swap(finish, other.finish);
		swap(end_of_store, other.end_of_store);
	}


};

template<typename T, typename Alloc>
void swap(vector<T, Alloc> &lhs, vector<T, Alloc> &rhs) noexcept {
	lhs.swap(rhs);
}

template<typename T, typename Alloc>
Alloc vector<T, Alloc>::allocator;

}

#endif