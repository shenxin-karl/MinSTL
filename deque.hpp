
#ifndef M_DEQUE_HPP
#define M_DEQUE_HPP
#include "allocator.hpp"
#include "iterator.hpp"
#include "algorithm.hpp"
#include "utility.hpp"

namespace sx {

/* 获得 deque 缓冲区的大小 */
constexpr inline std::size_t __deque_buf_size(std::size_t n, std::size_t sz) noexcept {
    return n != 0 ? n : (sz < 512 ? 512 / sz : 1);
}

template<typename T, typename Alloc = sx::allocator<T>> class deque;
template<typename T, typename Ptr, typename Ref> class __deque_iterator;
template<typename T, typename Alloc> void swap(deque<T, Alloc> &, deque<T, Alloc> &) noexcept;

template<typename T, typename Ptr, typename Ref>
class __deque_iterator {
	template<typename Type, typename Allocator>
	friend class deque;
public:
    using value_type            = T;
    using pointer               = Ptr;
    using reference             = Ref;
    using difference_type       = std::ptrdiff_t;
    using size_type             = std::size_t;
    using iterator_category     = sx::random_access_iterator_tag;
    using map_pointer           = T **;
public:
    pointer         cur;        /* 当前缓存区中指向的元素 */
    pointer         first;      /* 当前缓冲区的头 */
    pointer         end;        /* 当前缓存区的尾 */
    map_pointer     node;       /* 指向管控中心 */
public:
	__deque_iterator() : cur(nullptr), first(nullptr), end(nullptr), node(nullptr) {}
	__deque_iterator(__deque_iterator const &) = default;
	__deque_iterator &operator=(__deque_iterator const &) = default;
	~__deque_iterator() = default;
	__deque_iterator(pointer cur, pointer first, pointer end, map_pointer node) 
		: cur(cur), first(first), end(end), node(node) {}
private:
    static difference_type buffer_size() noexcept {
        return sx::__deque_buf_size(0, sizeof(T));
    }

    void set_node(map_pointer new_node) noexcept {
        node = new_node;
        first = *new_node;
        end = first + buffer_size();
    }
public:
    pointer operator->() noexcept {
        return &(this->operator*());
    }

    reference operator*() {
        return *cur;
    }

    reference operator[](int index) {
        __deque_iterator tmp = *this;
        tmp += index;
        return *tmp;
    }

    difference_type operator-(__deque_iterator const &other) const noexcept {
        return buffer_size() * (node - other.node - 1) + (cur - first) + (other.end - other.cur);
    }

    __deque_iterator &operator++() noexcept {
        ++cur;
        if (cur == end) {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    __deque_iterator operator++(int) noexcept {
        __deque_iterator ret = *this;
        ++(*this);
        return ret;
    }

    __deque_iterator &operator--() noexcept {
        if (cur == first) {
            set_node(node - 1);
            cur = end;
        }
        --cur;
        return *this;
    }

    __deque_iterator operator--(int) noexcept {
        __deque_iterator ret = *this;
        --(*this);
        return ret;
    }

    __deque_iterator &operator+=(int n) {
        difference_type offset = n + (end - cur);       /* 抽象成从 first 开始移动 */
        if (offset >= 0 && offset < buffer_size()) 
            cur += n;
        else {
            difference_type node_offset;
            node_offset = offset > 0 ? offset / buffer_size() 
                         : -difference_type((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * buffer_size()); 
        }
        return *this;
    }

    __deque_iterator operator+(int n) const noexcept {
        __deque_iterator ret = *this;
        ret += n;
        return ret;
    }

    __deque_iterator &operator-=(int n) noexcept {
        *this += -n;
        return *this;
    }

    __deque_iterator operator-(int n) const noexcept {
        __deque_iterator ret = *this;
        ret += -n;
        return ret;
    }

    friend bool operator==(__deque_iterator const &first, __deque_iterator const &second) noexcept {
        return first.cur == second.cur;
    }

    friend bool operator!=(__deque_iterator const &first, __deque_iterator const &second) noexcept {
        return !(first == second);
    }

    friend bool operator>(__deque_iterator const &first, __deque_iterator const &second) noexcept {
        return first.node == second.node ? first.cur > second.cur : first.node > second.node;
    }

    friend bool operator<(__deque_iterator const &first, __deque_iterator const &second) noexcept {
        return first != second && !(first > second);
    }

    friend bool operator>=(__deque_iterator const &first, __deque_iterator const &second) noexcept {
        return first == second || first > second;
    }

    friend bool operator<=(__deque_iterator const &first, __deque_iterator const &second) noexcept {
        return first == second || first < second;
    }
};



template<typename T, typename Alloc>
class deque : public sx::comparetor<deque<T, Alloc>> {
	friend void swap<T, Alloc>(deque<T, Alloc> &, deque<T, Alloc> &) noexcept;
public:
    using value_type        = T;
    using pointer           = T *;
    using reference         = T &;
    using const_pointer     = T const *;
    using const_reference   = T const &;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    using iterator          = __deque_iterator<T, T *, T &>;
	using const_iterator	= __deque_iterator<T, T const *, T const &>;
protected:
    using map_pointer       = T **;
    using Map_Alloc         = decltype(sx::transform_alloator_type<T, pointer>(Alloc{}));
protected:
    static Alloc            data_allocator;     /* 数据分配器 */
    static Map_Alloc        map_allocator;      /* map 分配器 */
    iterator                start;              /* 第一个元素迭代器 */
    iterator                finish;             /* 最后一个元素的迭代器 */
    map_pointer             map;                /* map 中控区指针 */
    size_type               map_size;           /* map 内有多少指针 */
public:
    deque() : start(), finish(), map(nullptr), map_size(0) {
		empty_initialze();
	}

    deque(int n, value_type const &value) 
    : start(), finish(), map(nullptr), map_size(0) {
        fill_initialize(n, value);
    }

	deque(deque const &other) : start(), finish(), map(nullptr), map_size(0) {
		alloc_and_fill(other.begin(), other.end());
	}

	deque(deque &&other) : deque() {
		sx::swap(*this, other);
	}

	deque &operator=(deque const &other) {
		deque tmp = other;
		sx::swap(*this, tmp);
		return *this;
	}

	deque &operator=(deque &&other) {
		deque tmp;
		sx::swap(tmp, other);
		sx::swap(*this, tmp);
		return *this;
	}

	template<typename InputIterator, 
			 typename = std::enable_if_t<sx::is_input_iterator_v<InputIterator>>>
	deque(InputIterator first, InputIterator end) {
		alloc_and_fill(first, end);
	}

	~deque() {
		clear();
		data_allocator.deallocate(start.first, buffer_size());
		map_allocator.deallocate(map, map_size);
	}
private:
	/* 获得默认 deque map 大小 */
    static constexpr size_type initial_map_size() {
        return 8;
    }

	/* 获得缓冲区的大小 */
    static size_type buffer_size() noexcept {
        return __deque_buf_size(0, sizeof(T));
    }

    /* 创建中控区和分配结点, 并设置好 start 和 finish 迭代器的位置 */
    void create_map_and_nodes(size_type num_elements) {
        size_type num_nodes = num_elements / buffer_size() + 1;
        map_size = std::max(initial_map_size(), num_nodes + 2);
        map = map_allocator.allocate(map_size);
        size_type buff_size = buffer_size();

        map_pointer nstart = map + (map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;
        map_pointer cur;
        try {
            for (cur = nstart; cur <= nfinish; ++cur) 
                *cur = data_allocator.allocate(buff_size);
        } catch(...) {
            for (map_pointer beg = nstart; beg <= cur; ++beg) 
                data_allocator.deallocate(*beg, buff_size);
            
            map_allocator.deallocate(map, map_size);
            throw;
        }

        /* 设置好 start 和 finish 迭代器的位置 */
        start.set_node(nstart);
        finish.set_node(nfinish);
        start.cur = start.first;
        finish.cur = finish.first + (num_elements % buff_size);
    }

    /* 初始化并填充 */
    void fill_initialize(int n, value_type const &value) {
        create_map_and_nodes(n);
        size_type buff_size = buffer_size();
        map_pointer cur;
        try {
            for (cur = start.node; cur < finish.node; ++cur) 
                uninitialized_fill_n(*cur, buff_size, value);
            uninitialized_fill(finish.first, finish.cur, value);
        } catch(...) {
            for (map_pointer node = start.node; node != cur; ++node) 
                data_allocator.destroy(*node, *node + buff_size);
            
            for (map_pointer node = start.node; node <= finish.node; ++node) 
                data_allocator.deallocate(*node, buff_size);

            map_allocator.deallocate(map, map_size);
            throw;
        }
    }

	/* 分配一个新缓冲区 */
    static pointer allocate_node() {
        return data_allocator.allocate(buffer_size());
    }

	/* 销毁一个缓冲区 */
    static void deallocate_node(pointer &node) {
        data_allocator.deallocate(node, buffer_size());
        node = nullptr;
    }

	/* 重启设置 map 中控区 */
	void reallocate_map(size_type nodes_to_add, bool add_at_front) {
		size_type old_num_nodes = finish.node - start.node + 1;
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		map_pointer new_nstart;
		/* 如果空间足够, 那么不需要重写申请 map 空间, 只需要调整前后的位置即可 */
		if (map_size > 2 * new_num_nodes) {
			new_nstart = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
			if (new_nstart > start.node)
				sx::copy(start.node, finish.node, new_nstart);
			else 
				sx::copy_backward(start.node, finish.node + 1, finish.node + 1 + (start.node - new_nstart));

		/* 重新申请新的 map 空间 */
		} else {
			size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
			map_pointer new_map = map_allocator.allocate(new_map_size);
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
			sx::copy(start.node, finish.node, new_nstart);
			map_allocator.deallocate(map, map_size);
			
			map = new_map;
			map_size = new_map_size;
		}

		/* 重新设置迭代器位置 */
		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes - 1);
	}
	
	void reserve_map_at_front(size_type nodes_to_add = 1) {
		if (nodes_to_add > static_cast<size_type>(start.node - map))
			reallocate_map(nodes_to_add, true);
	}

	void reserve_map_at_back(size_type nodes_to_add = 1) {
		if (nodes_to_add + 1 > map_size - (finish.node - map))
			reallocate_map(nodes_to_add, false);
	}

	/* 尾插入辅助函数 */
    template<typename... Args>
    void push_back_aux(Args&&... args) {
        reserve_map_at_back();				/* 如果符合某种条件必须更换一个新 map */
        *(finish.node + 1) = allocate_node();
        try {
            data_allocator.construct(finish.cur, std::forward<Args>(args)...);
            finish.set_node(finish.node + 1);
            finish.cur = finish.first;
        } catch(...) {
            deallocate_node(*(finish.node + 1));
            throw;
        }
    }

	/* 头插入辅助函数 */
    template<typename... Args>
    void push_front_aux(Args&&... args) {
        reserve_map_at_front();				/* 如果符合条件必须更换一个新 map */
        *(start.node - 1) = allocate_node();
        try {
            start.set_node(start.node - 1);
            start.cur = start.end - 1;
            data_allocator.construct(start.cur, std::forward<Args>(args)...);
        } catch(...) {
            start.set_node(start.node + 1);
            start.cur = start.first;
            deallocate_node(*(start.node - 1));
            throw;
        }
    }

	/* 空初始化 */
	void empty_initialze() {
		create_map_and_nodes(__deque_buf_size(0, sizeof(T)));
		finish = start = start + (finish - start) / 2;
	}

	void pop_front_aux() {
		data_allocator.destroy(start.cur);
		data_allocator.deallocate(start.first, buffer_size());
		start.set_node(start.node + 1);
		start.cur = start.first;
	}

	void pop_back_aux() {
		data_allocator.deallocate(finish.first, buffer_size());
		finish.set_node(finish.node - 1);
		finish.cur = finish.end - 1;
		data_allocator.destroy(finish.cur);
	}

    iterator insert_aux(iterator pos, value_type const &value) {
        difference_type index = pos - start;
        /* 如果 pos 前面的元素比较少 */
        if (index < static_cast<difference_type>((size() / 2))) {
            push_front(front());
            pos = start + index;
            sx::copy(start + 2, pos, start + 1);
        } else {
            push_back(back());
            pos = start + index;
            sx::copy_backward(pos, finish - 2, finish - 1);
        }
        *pos = value;
        return pos;
    }

	template<typename InputIterator>
	void alloc_and_fill(InputIterator first, InputIterator end) {
		difference_type offset = sx::distance(first, end);
		create_map_and_nodes(offset);
		
		iterator iter = start;
		try {
			for (; iter != finish; ++iter, ++first)
				data_allocator.construct(&*iter, *first);
		} catch (...) {
			for (iterator beg = start; beg != iter; ++beg)
				data_allocator.destroy(&*beg);
			
			for (map_pointer node = start.node; node <= finish.node; ++node)
				data_allocator.deallocate(*node, buffer_size());

			throw;
		}
	}
public:
    iterator begin() noexcept {
        return start;
    }

    iterator end() noexcept {
        return finish;
    }

	const_iterator begin() const noexcept {
		return cbegin();
	}

	const_iterator end() const noexcept {
		return cend();
	}

	const_iterator cbegin() const noexcept {
		return const_iterator(start.cur, start.first, start.end, start.node);
	}

	const_iterator cend() const noexcept {
		return const_iterator(finish.cur, finish.first, finish.end, finish.node);
	}

    size_type size() const noexcept {
        return finish - start;
    }

    size_type max_size() const noexcept {
        return size_type(-1);
    }

    bool empty() const noexcept {
        return finish == start;
    }

    reference front() {
        return *start;
    }

    const_reference front() const {
        return *cbegin();
    }

    reference back() {
        iterator ret = finish;
        --ret;
        return *ret;
    }

    const_reference back() const {
        const_iterator ret = cend();
        --ret;
        return *ret;
    }

    void push_back(value_type const &value) {
        if (finish.cur != (finish.end - 1)) {
            data_allocator.construct(finish.cur, value);
            ++finish.cur;
        } else 
            push_back_aux(value);
    }

    void push_front(value_type const &value) {
        if (start.cur != start.first) {
            --start.cur;
            data_allocator.construct(start.cur, value);
        } else 
            push_front_aux(value);
    }

    template<typename... Args>
    void emplace_front(Args&&... args) {
        if (start.cur != start.first) {
            --start.cur;
            data_allocator.construct(start.cur, std::forward<Args>(args)...);
        } else 
            push_front_aux(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (finish.cur != (finish.end - 1)) {
            data_allocator.construct(finish.cur, std::forward<Args>(args)...);
            ++finish.cur;
        } else 
            push_back_aux(std::forward<Args>(args)...);
    }

	void pop_front() {
		if (start.cur != start.end - 1) {
			data_allocator.destroy(start.cur);
			++start.cur;
		} else
			pop_front_aux();
	}

	void pop_back() {
		if (finish.cur != finish.first) {
			--finish.cur;
			data_allocator.destroy(finish.cur);
		} else
			pop_back_aux();
	}

    /* 清除整个 deque, 但是会留下一个 缓冲区 这是 deque 的策略 */
    void clear() {
        /* 从第二个缓冲区开始到末尾的缓冲区之间, 中间的缓冲区都是饱满的 */
        for (map_pointer cur = start.node + 1; cur < finish.node; ++cur) {
            data_allocator.destroy(*cur, *cur + buffer_size());
            data_allocator.deallocate(*cur, buffer_size());
        }

        /* 剩余两个缓冲区, start 和 finish 各占用一个 */
        if (start.node != finish.node) {
            data_allocator.destroy(start.cur, start.end);
            data_allocator.destroy(finish.first, finish.cur);
            data_allocator.deallocate(finish.first, buffer_size());
        
        /* 只有一个 start 缓冲区 */
        } else 
            data_allocator.destroy(start.cur, start.end);
        
        finish = start;
    }

    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        /* pos 如果前面的元素比较少 */
        if (index < static_cast<difference_type>(size() / 2)) {
            sx::copy_backward(start, pos, next);
            pop_front();
        } else {
            sx::copy(pos + 1, finish, pos);
            pop_back();
        }

        return start + index;
    }

    iterator erase(iterator first, iterator end) {
        /* 如果是清除所有的元素则调用 clear() */
        if (first == start && end == finish) {
            clear();
            return finish;

        } else {
            difference_type n = end - first;                /* 清除区间的元素的数量 */
            difference_type elems_before = first - start;   /* 清除区间前方的元素数量 */
            /* 删除区间的前方元素比较少 */
            if (elems_before < static_cast<difference_type>((size() - n) / 2)) {
                sx::copy_backward(start, start + elems_before, end);
                iterator new_start = start + n;
                data_allocator.destroy(start, new_start);

                /* 释放缓冲区 */
                for (map_pointer cur = start.node; cur < new_start.node; ++cur)
                    data_allocator.deallocate(*cur, buffer_size());
                /* 更新 start 位置 */
                start = new_start;
            } else {
                sx::copy(end, finish, first);
                iterator new_finish = finish - n;
                data_allocator.destroy(new_finish, finish);
                for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) 
                    data_allocator.deallocate(*cur);
                finish = new_finish;
            }

            return start + elems_before;
        }
    }

    iterator insert(iterator pos, value_type const &value) {
        if (pos == start) {
            push_front(value);
            return start;
        } else if (pos == finish) {
            push_back(value);
            iterator ret = finish;
            return --ret;
        } else 
            return insert_aux(pos, value);
    }

    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args) {
        if (pos == start) {
            emplace_front(std::forward<Args>(args)...);
            return start;
        } else if (pos == finish) {
            emplace_back(std::forward<Args>(args)...);
            iterator ret = finish;
            return --ret;
        } else 
            return insert_aux(pos, std::forward<Args>(args)...);
    }
};

template<typename T, typename Alloc>
void swap(deque<T, Alloc> &first, deque<T, Alloc> &second) noexcept {
	using std::swap;
	swap(first.start, second.start);
	swap(first.finish, second.finish);
	swap(first.map, second.map);
	swap(first.map_size, second.map_size);
}

template<typename T, typename Alloc>
Alloc deque<T, Alloc>::data_allocator{};

template<typename T, typename Alloc>
typename deque<T, Alloc>::Map_Alloc
deque<T, Alloc>::map_allocator{};

};	// !namespace sx

#endif