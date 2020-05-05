#ifndef M_STACK_HPP
#define M_STACK_HPP
#include "mdeque.hpp"

namespace sx {

template<typename T, typename Container> class stack;

template<typename T, typename Container> 
bool operator==(stack<T, Container> const &, stack<T, Container> const &) noexcept;

template<typename T, typename Container>
bool operator!=(stack<T, Container> const &, stack<T, Container> const &) noexcept;

template<typename T, typename Container>
void swap(stack<T, Container> &, stack<T, Container> &) noexcept;

template<typename T, typename Container = deque<T>>
class stack {
    friend void swap(stack<T, Container> &, stack<T, Container> &) noexcept;
    friend bool operator==(stack<T, Container> const &, stack<T, Container> const &) noexcept;
    friend bool operator!=(stack<T, Container> const &, stack<T, Container> const &) noexcept;
public:
    using value_type        = typename Container::value_type;
    using pointer           = typename Container::pointer;
    using reference         = typename Container::reference;
    using const_pointer     = typename Container::const_pointer;
    using const_reference   = typename Container::const_reference;
    using size_type         = std::size_t;
protected:
    Container   container;
public:
    stack() = default;
    stack(stack const &) = default;
    stack(stack &&) = default;
    stack &operator=(stack const &) = default;
    stack &operator=(stack &&) = default;
    ~stack() = default;
public:
    bool empty() const noexcept {
        return container.empty();
    }

    size_type size() const noexcept {
        return container.size();
    }

    reference top() {
        return container.back();
    }

    const_reference top() const {
        return container.back();
    }

    void push(value_type const &value) {
        container.push_back(value);
    }

    template<typename... Args>
    void emplace(Args&&... args) {
        container.emplace_back(std::forward<Args>(args)...);
    }

    void pop(value_type const &value) {
        container.pop_back();
    }

    void swap(stack &other) noexcept {
        if (this == &other)
            return;
        sx::swap(*this, other);
    }
};

template<typename T, typename Container>
bool operator==(stack<T, Container> const &first, stack<T, Container> const &second) noexcept {
    return first.container == second.container;
}

template<typename T, typename Container>
bool operator!=(stack<T, Container> const &first, stack<T, Container> const &second) noexcept {
    return first.container != second.container;
}

template<typename T, typename Container>
void swap(stack<T, Container> &first, stack<T, Container> &second) noexcept {
    using std::swap;
    swap(first.container, second.container);
}

} // !namespace sx

#endif