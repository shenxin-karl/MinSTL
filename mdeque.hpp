#ifndef M_DEQUE_HPP
#define M_DEQUE_HPP
#include "mallocator.hpp"
#include "miterator.hpp"
#include <limits>

namespace sx {

inline std::size_t deque_buffer_size(std::size_t n, std::size_t sz) {
    return n != 0 ? n : (sz < 512 ? std::size_t(512 / sz) : std::size_t(1));
}

template<typename T, typename Alloc = sx::allocator<T>> 
class deque;

template<typename T>
class deque_iterator {
    template<typename Alloc>
    using deque_type = deque<T, Alloc>;
    friend class deque_type;
    using iterator_category = random_access_iterator_tag;
    using value_type        = T;
    using reference         = T &;
    using pointer           = T *;
    using difference_type   = std::ptrdiff_t;
    using const_pointer     = T const *;
    using const_reference   = T const &;
    using self              = deque_iterator<T>;
    using map_pointer       = T **;
protected:
    T              *cur;
    T              *first;
    T              *last;
    map_pointer     node;
protected:
    difference_type get_buffer_size() {
        return deque_buffer_size(0, sizeof(T));
    }

    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + difference_type(get_buffer_size());
    }
public:
    reference operator*() {
        return *cur;
    }

    pointer operator->() {
        return cur;
    }

    difference_type operator-(self const &other) const {
        return get_buffer_size() * (node - other.node - 1) + (cur - first) + (other.last - other.cur));
    }

    self &operator++() {
        ++cur;
        if (cur == last) {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    self operator++(int) {
        self tmp = *tmp;
        ++*this;
        return tmp;
    }

    self operator--() {
        if (cur == first) {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self &operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        difference_type buffer_size = get_buffer_size();
        if (offset > 0 && offset < buffer_size)
            cur += n;
        else {
            difference_type node_offset;
            node_offset = offset > 0 ? offset / buffer_size :
                          -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node * buffer_size);
        }
        return *this;
    }

    self operator+(int n) {
        self result = *this;
        result += n;
        return result;
    }

    self &operator-=(int n) {
        *this += -n;
        return *this;
    }

    difference_type operator-(self const &other) {
        return get_buffer_size() * (node - other.node - 1) + (cur - first) + (other.last - x.cur);
    }

    self operator-(int n) {
        self result = *this;
        result += -n;
        return result;
    }

    reference operator[](int n) {
        return *(*this + n);
    }

    friend bool operator==(self const &first, self const &second) {
        return first.cur == second.cur;
    }

    friend bool operator!=(self const &first, self const &second) {
        return !(first == second);
    }

    friend bool operator<(self const &first, self const &second) {
        return first.node == second.node ? first.cur < second.cur : first.node < second.node;
    }

    friend bool operator>(self const &first, self const &second) {
        return first.node == second.node ? first.cur > second.cur : first.node > second.node;
    }

    friend bool operator<=(self const &first, self const &second) {
        return first == second || first < second;
    }

    friend bool operator>=(self const &first, self const &second) {
        return first == second || first > second;
    }
};


template<typename T, typename Alloc> 
class deque {
public:
    using value_type                = T;
    using pointer                   = T*;
    using reference                 = T&;
    using const_pointer             = T const *;
    using const_reference           = T const &;
    using difference_type           = std::ptrdiff_t;
    using size_type                 = std::size_t;
    using iterator                  = deque_iterator<T>;
    using map_pointer               = pointer *;
protected:
    static Alloc    data_allocator;
    static Alloc    map_allocator;
    map_pointer     map;            /* 指向 map, map 是块连续空间, 其中每个原始都是一个指针, 指向一个缓冲区 */
    size_type       size;           /* map 内可容纳多个指针 */
    iterator        start;          /* 表现的第一个结点 */
    iterator        finish;         /* 表现的最后一个结点 */
public:

public:
    iterator begin() {
        return start;
    }

    iterator end() {
        return end;
    }

    reference operator[](int n) {
        return start[difference_type(n)]
    }

    reference front() {
        return *start;
    }

    reference back() {
        iterator result = finish;
        --result;
        return *result;
    }

    size_type size() const {
        return finisn - start;
    }

    size_type max_size() const {
        return size_type(-1);
    }

    bool empty() const {
        return finish == start;
    }
};

};

#endif