#ifndef M_PRIORITY_QUEUE_HPP
#define M_PRIORITY_QUEUE_HPP
#include "vector.hpp"
#include "heap_algorithm.hpp"
#include "iterator.hpp"

namespace sx {

template<typename T, typename Container = sx::vector<T>,
	   	 typename Compare = std::less<T>>
class priority_queue;

template<typename T, typename Container, typename Compare>
void swap(priority_queue<T, Container, Compare> &, priority_queue<T, Container, Compare> &) noexcept;

template<typename T, typename Container, typename Compare>
class priority_queue {
	friend void swap<T, Container, Compare>(priority_queue<T, Container, Compare> &, priority_queue<T, Container, Compare> &) noexcept;
public:
	using value_type		= T;
	using pointer			= T * ;
	using reference			= T & ;
	using const_pointer		= T const *;
	using const_reference	= T const &;
	using size_type			= std::size_t;
protected:
	Container	container;		/* 底层容器 */
	Compare		compare;		/* 比较器 */
public:
	priority_queue() : container() {}
	priority_queue(priority_queue const &) = default;
	priority_queue(priority_queue &&) = default;
	priority_queue &operator=(priority_queue const &) = default;
	priority_queue &operator=(priority_queue &&) = default;
	~priority_queue() = default;
	explicit priority_queue(Compare const &c) : container(), compare(c) {}

	template<typename InputIterator, 
			 typename = sx::is_input_iterator_t<InputIterator>>
	priority_queue(InputIterator first, InputIterator end) : container(first, end) {
		sx::make_heap(container.begin(), container.end(), compare);
	}

	template<typename InputIterator, typename = sx::is_input_iterator_t<InputIterator>>
	priority_queue(InputIterator first, InputIterator end, Compare comp)
		: container(first, end), compare(comp) {
		sx::make_heap(container.begin(), container.end());
	}

	size_type size() const noexcept {
		return container.size();
	}

	bool empty() const noexcept {
		return container.empty();
	}

	const_reference top() const {
		return container.front();
	}

	void push(T const &value) {
		try {
			container.push_back(value);
			sx::push_heap(container.begin(), container.end(), compare);
		} catch (...) {
			container.clear();
			throw;
		}
	}

	void pop() {
		try {
			sx::pop_heap(container.begin(), container.end(), compare);
			container.pop_back();
		} catch (...) {
			container.clear();
			throw;
		}
	}
};

template<typename T, typename Container, typename Compare>
void swap(priority_queue<T, Container, Compare> &first, priority_queue<T, Container, Compare> &second) noexcept {
	using std::swap;
	swap(first.container, second.container);
	swap(first.compare, second.compare);
}

}

#endif // !M_PRIORITY_QUEUE_HPP