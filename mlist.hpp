#ifndef M_LIST_HPP
#define M_LIST_HPP
#include "mallocator.hpp"
#include "miterator.hpp"

namespace sx
{

template<typename T, typename Alloc = allocator<T>> class list;

template <typename T>
struct list_node {
    list_node  *next;
    list_node  *prev;
    T           data;
};

template <typename T>
class list_iterator : public iterator<bidirectional_iterator_tag, T> {
    friend class list<T>;
    using self      = list_iterator<T>;
    using link_type = list_node<T> *;
private:
    link_type   node;
public:
    list_iterator(link_type node_ptr) : node(node_ptr) {}
    list_iterator(iterator const &other) : node(other.node) {}
    list_iterator() : node(nullptr) {}
    list_iterator(self const &) = default;
    list_iterator(self &&) = default;
    self &operator=(self const &) = default;
    self &operator=(self &&) = default;
    ~list_iterator() = default;
public:
    friend bool operator==(self const &a, self const &b) {
        return a.node == b.node;
    }

    friend bool operator!=(self const &a, self const &b) {
        return !(a == b);
    }

    reference operator*() {
        return node->data;
    }

    pointer operator->() {
        return &(node->data);
    }

    self &operator++() {
        node = node->next;
        return *this;
    }

    self operator++(int) {
        self result(node);
        node = node->next;
        return result;
    }

    self &operator--() {
        node = node->prev;
        return *this;
    }

    self &operator--() {
        self result(node);
        node = node->prev;
        return result;
    }
};


template <typename T>
class list_const_iterator : public iterator<bidirectional_iterator_tag, T> {
    friend class list<T>;
    using self      = list_const_iterator<T>;
    using link_type = list_node<T> *;
private:
    const link_type       node;
public:
    list_const_iterator(link_type node_ptr) : node(node_ptr) {}
    list_const_iterator(iterator const &other) : node(other.node) {}
    list_const_iterator() : node(nullptr) {}
    list_const_iterator(self const &) = default;
    list_const_iterator(self &&) = default;
    self &operator=(self const &) = default;
    self &operator=(self &&) = default;
    ~list_const_iterator() = default;
public:
    friend bool operator==(self const &first, self const &second) {
        return first.node == second.node;
    }

    friend bool operator!=(self const &first, self const &second) {
        return !(first == second);
    }

    const_reference operator*() {
        return node->data;
    }

    const_pointer operator->() {
        return &(node->data);
    }

    self &operator++() {
        node = node->next;
        return *this;
    }

    self operator++(int) {
        self result(node);
        node = node->next;
        return result;
    }

    self &operator--() {
        node = node->prev;
        return *this;
    }

    self &operator--() {
        self result(node);
        node = node->prev;
        return result;
    }
};



template <typename T>
class list_reverse_iterator : public iterator<bidirectional_iterator_tag, T> {
    friend class list<T>;
    using self      = list_reverse_iterator<T>;
    using link_type = list_node<T> *;
private:
    link_type       node;
public:
    list_reverse_iterator(link_type node_ptr) : node(node_ptr) {}
    list_reverse_iterator() : node(nullptr) {}
    list_reverse_iterator(self const &) = default;
    list_reverse_iterator(self &&) = default;
    self &operator=(self const &) = default;
    self &operator=(self &&) = default;
    ~list_reverse_iterator() = default;
public:
    friend bool operator==(self const &first, self const &second) {
        return first.node == second.node;
    }

    friend bool operator!=(self const &first, self const &second) {
        return !(first == second);
    }

    reference operator*() {
        return node->data;
    }

    pointer operator->() {
        return &(node->data);
    }

    self &operator++() {
        node = node->prev;
        return *this;
    }

    self operator++(int) {
        self result(node);
        node = node->prev;
        return result;
    }

    self &operator--() {
        node = node->next;
        return *this;
    }

    self operator--(int) {
        self result(node);
        node = node->next;
        return result;
    }
};


template <typename T>
class list_reverse_iterator : public iterator<bidirectional_iterator_tag, T> {
    friend class list<T>;
    using self      = list_reverse_iterator<T>;
    using link_type = list_node<T> *;
private:
    link_type       node;
public:
    list_reverse_iterator(link_type node_ptr) : node(node_ptr) {}
    list_reverse_iterator() : node(nullptr) {}
    list_reverse_iterator(self const &) = default;
    list_reverse_iterator(self &&) = default;
    self &operator=(self const &) = default;
    self &operator=(self &&) = default;
    ~list_reverse_iterator() = default;
public:
    friend bool operator==(self const &first, self const &second) {
        return first.node == second.node;
    }

    friend bool operator!=(self const &first, self const &second) {
        return !(first == second);
    }

    const_reference operator*() {
        return node->data;
    }

    const_pointer operator->() {
        return &(node->data);
    }

    self &operator++() {
        node = node->prev;
        return *this;
    }

    self operator++(int) {
        self result(node);
        node = node->prev;
        return result;
    }

    self &operator--() {
        node = node->next;
        return *this;
    }

    self operator--(int) {
        self result(node);
        node = node->next;
        return result;
    }
};


template <typename T, typename Alloc>
class list {
public:
    using link_node                 = list_node<T>;
    using link_type                 = link_node *;
    using value_type                = T;
    using pointer                   = value_type *;
    using referenc                  = value_type &;
    using size_type                 = std::size_t;
    using difference_type           = std::ptrdiff_t;
    using const_pointer             = T const *;
    using const_reference           = T const &;
    using iterator                  = list_iterator<T>;
    using reverse_iterator          = list_reverse_iterator<T>;
    using const_iterator            = list_const_iterator<T>;
    using const_reverse_iterator    = list_reverse_iterator<T>;
protected:
    static Alloc      allocator;
    link_type         node;
    size_type         size;
protected:
    static link_type get_node() {
        return allocator.allocate();
    }

    static void put_node(link_type node_ptr) {
        allocator.deallocate(node_ptr);
    }

    template<typename... Args>
    static link_type create_node(Args&&... args) {
        link_type node_ptr = get_node();
        allocator.construct(&node_ptr->data, std::forward<Args>(args)...);
        return node_ptr;
    }

    static void destroy_node(link_type node_ptr) {
        allocator.destroy(&node_ptr->data);
        put_node(node_ptr);
    }

    static void transfer(iterator position, iterator first, iterator end) {
        if (position == end)
            return;

        iterator last = end;
        --last;
        
        first.node->prev->next = last.node->next;
        last.node->next->prev = first.node->prev;
        first.node->prev = position.node->prev;
        first.node->prev->next = first.node;
        last.node->next = position.node;
        position.node->prev = last.node;
    }

    static void insert_aux(iterator position, link_type node_ptr) {
        node_ptr->next = position.node;
        node_ptr->prev = position.node->prev;
        node_ptr->next->prev = node_ptr;
        node_ptr->prev->next = node_ptr;
    }

    template<typename Compfunc>
    static void list_sort_aux(iterator first, iterator last, Compfunc const &comp_func) {
        iterator flag = last;
        if (first == last || first == --flag)
            return;

        iterator left = first;
        iterator right = last;
        value_type value = std::move(*left);
        while (left != right) {
            while (left != right && comp_func(value, *right))
                --right;
            if (left != right && comp_func(*right, value))
                std::swap(*(left++), *right);
            while (left != right && comp_func(*left, value))
                ++left;
            if (left != right && comp_func(value, *left))
                std::swap(*(right--), *left);
        }

        *left = std::move(value);
        list_sort_aux(first, --left, comp_func);
        list_sort_aux(++right, last, comp_func);
    }

    void empty_initialize() {
        node = get_node();
        node->next = node->prev = node;
        size = 0;
    }
public:
    list() { 
        empty_initialize(); 
    }

    list(list const &other) {
        empty_initialize();
        for (const_iterator iter = other.begin(); iter != other.end(); ++iter) 
            push_back(*iter);
    }

    list(list &&other) 
    : node(other.node), size(other.size) {
        other.empty_initialize();
    }

    list &operator=(list &&other) {
        using std::swap;
        list tmp;
        swap(tmp, other);
        swap(*this, tmp);
        return *this;
    }

    list &operator=(list other) {
        using std::swap;
        swap(*this, other);
        return *this;
    }

    ~list() {
        iterator cur = begin();
        while (cur != end()) {
            iterator next = cur;
            ++next;
            erase(cur);
            cur = next;
        }

        destroy_node(node);
        node = nullptr;
    }
public:
    bool empty() const {
        return size  == 0;
    }

    size_type size() const {
        return size;
    }

    iterator begin() {
        return iterator(node->next);
    }

    iterator end() {
        return iterator(node);
    }

    const_iterator cbegin() const {
        return const_iterator(node->next);
    }

    const_iterator cend() const {
        return const_iterator(node);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(node->prev);
    }

    reverse_iterator rend() {
        return reverse_iterator(node);
    }

    const_reverse_iterator cbegin() const {
        return const_reverse_iterator(node->prev);
    }

    const_reverse_iterator cend() const {
        return const_reverse_iterator(node);
    }

    referenc front() {
        return *begin();
    }

    referenc back() {
        return *(--end());
    }

    iterator insert(iterator position, value_type const &value) {
        link_type node_ptr = create_node(value);
        insert_aux(position, node_ptr);
        ++size;
        return iterator(node_ptr);
    }

    const_iterator insert(const_iterator position, value_type const &value) {
        link_type node_ptr = create_node(value_type);
        insert_aux(const_iterator(position.node), node_ptr);
        ++size;
        return const_iterator(node_ptr);
    }

    template<typename... Args>
    iterator emplace(iterator position, Args&&... args) {
        link_type node_ptr = create_node(std::forward<Args>(args)...);
        insert_aux(position, node_ptr);
        ++size;
        return node_ptr;
    }

    template<typename... Args>
    const_iterator emplace(const_iterator position, Args&&... args) {
        link_type node_ptr = create_node(std::forward<Args>(args)...);
        insert_aux(const_iterator(position.node), node_ptr);
        ++size;
        return const_iterator(node_ptr);
    }

    void push_back(value_type const &value) {
        insert(end(), value);
        ++size;
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        emplace(end(), value_type(std::forward<Args>(args)...));
        ++size;
    }

    void push_front(value_type const &value) {
        insert(begin(), value);
        ++size;
    }

    template<typename... Args>
    void emplace_front(Args&&... args) {
        emplace(begin(), std::forward<Args>(args)...);
        ++size;
    }

    iterator erase(iterator position) {
        link_type next_node = position.node->next;
        link_type prev_node = position.node->prev;
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        --size;
        return iterator(next_node);
    }

    const_iterator erase(const_iterator position) {
        return const_iterator(erase(iterator(position.node)));
    }

    void pop_front() {
        erase(begin());
    }
    
    void pop_back() {
        erase(--end());
    }

    void clear() {
        link_type cur = node->next;
        while (cur != node) {
            link_type tmp = cur;
            cur = cur->next;
            destroy_node(tmp);
        }

        node->next = node;
        node->prev = node;
        size = 0;
    }

    void remove(value_type const &value) {
        iterator first = cbegin();
        iterator end = cend();
        while (first != end) {
            if (*first == value) {
                first = erase(first);
                continue;
            }
            ++first;
        }
    }

    void unique() {
        iterator first = cbegin();
        iterator end = cend();
        if (first == end)
            return;
        iterator next = first;
        while (++next != end) {
            if (*first == *next) 
                erase(next);
            else 
                first = next;
            next = first;
        }
    }

    void splice(iterator position, list &other) {
        if (!other.empty())
            transfer(position, other.begin(), other.end());
        size += other.size();
    }

    void splice(iterator position, list &, iterator i) {
        iterator j = i;
        ++j;
        if (position == j || position == i)
            return;
        transfer(position, i, j);
        ++size;
    }

    void splice(iterator position, list &, iterator first, iterator end) {
        if (first != end) {
            transfer(position, first, end);
            size += distance(first, end);
        }
    }

    void splice(const_iterator position, list &other) {
        splice(iterator(position.node), other);
    }

    void splice(const_iterator position, list &x, const_iterator i) {
        splice(iterator(position.node), x, iterator(i.node));
    }

    void splice(const_iterator position, list &x, const_iterator first, const_iterator end) {
        splice(iterator(position.node), x, iterator(first.node), iterator(end.node));
    }

    void meger(list &other) {
        if (this == other)
            return;

        iterator first1 = begin();
        iterator end1   = end();
        iterator first2 = other.begin();
        iterator end2   = other.end();
        while (first1 != end1 && first2 != end2) {
            if (*first1 < *first2) {
                iterator next = first2;
                transfer(first1, first2, ++next);
                first2 = next;
            } else 
                ++first1;
        }
        if (first2 != end2)
            transfer(end1, first2, end2);

        size += other.size();
    }

    void reverse() {
        if (empty())
            return;

        iterator first = begin();
        ++first;
        while (first != end()) {
            iterator old = first;
            ++first;
            transfer(begin(), old, first);
        }
    }

    void sort() {
        if (empty())
            return;

        return list_sort_aux(begin(), --end());        
    }

    friend void swap(list &first, list &second) noexcept {
        std::swap(first.node, second.node);
        std::swap(first.size, second.size);
    }
};

} // namespace sx

#endif