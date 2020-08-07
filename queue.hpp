#ifndef M_QUEUE_HPP
#define M_QUEUE_HPP
#include "deque.hpp"

namespace sx {

template<typename T, typename Container = deque<T>>
class queue;

template<typename T, typename Container>
void swap(queue<T, Container> &, queue <T, Container> &) noexcept;

template<typename T, typename Container>
bool operator==(queue<T, Container> &, queue<T, Container> &) noexcept;

template<typename T, typename Container>
bool operator!=(queue<T, Container> &, queue<T, Container> &) noexcept;


template<typename T, typename Container>
class queue {
	friend void swap<T, Container>(queue<T, Container> &, queue<T, Container> &) noexcept;
	friend bool operator==<T, Container>(queue<T, Container> const &, queue<T, Container> const &) noexcept;
	friend bool operator!=<T, Container>(queue<T, Container> const &, queue<T, Container> const &) noexcept;
public:
	using value_type		= typename Container::value_type;
	using pointer			= typename Container::pointer;
	using reference			= typename Container::reference;
	using const_pointer		= typename Container::const_pointer;
	using const_reference	= typename Container::const_reference;
	using size_type			= typename Container::size_type;
protected:
	Container	container;
public:
	queue() = default;
	queue(queue const &) = default;
	queue(queue &&) = default;
	queue &operator=(queue const &) = default;
	queue &operator=(queue &&) = default;
	~queue() = default;
public:
	size_type size() const noexcept {
		return container.size();
	}

	bool empty() const noexcept {
		return container.empty();
	}

	reference front() {
		return container.front();
	}

	const_reference front() const {
		return container.front();
	}

	reference back() {
		return container.back();
	}

	const_reference back() const {
		return container.back();
	}

	void push(value_type const &value) {
		container.push_back(value);
	}

	void pop() {
		container.pop_front();
	}

	template<typename... Args>
	void emplace(Args&&... args) {
		container.emplace_back(std::forward<Args>(args)...);
	}

	void swap(queue const &other) {
		if (this == &other)
			return;
		sx::swap(*this, other);
	}
};

template<typename T, typename Container>
void swap(queue<T, Container> &first, queue <T, Container> &second) noexcept {
	using std::swap;
	swap(first.container, second.container);
}

template<typename T, typename Container>
bool operator==(queue<T, Container> &first, queue<T, Container> &second) noexcept {
	return first.container == second.container;
}

template<typename T, typename Container>
bool operator!=(queue<T, Container> &first, queue<T, Container> &second) noexcept {
	return first.container != second.container;
}


}	// !namespace sx

#endif // !M_QUEUE_HPP
