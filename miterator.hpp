#ifndef M_ITERATOR_HPP
#define M_ITERATOR_HPP
#include <cstddef>

namespace sx {
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iteratpr_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iteratpr_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};


	template<typename Category,  typename T, typename Distance = std::ptrdiff_t,
			 typename Pointer = T*, typename Reference = T&>
	struct iterator {
		using iterator_category = Category;
		using value_type 		= T;
		using difference_type 	= Distance;
		using pointer 			= Pointer;
		using reference 		= Reference;
		using const_pointer		= T const *;
		using const_reference   = T const &;
	};

	template<typename Iterator>
	struct iterator_traits {
		using iterator_category = typename Iterator::iterator_category;
		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = typename Iterator::pointer;
		using reference = typename Iterator::reference;
	};

	template<typename T>
	struct iterator_traits<T *> {
		using iterator_category = random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T * ;
		using reference = T &;
	};

	template<typename T>
	struct iterator_traits<T const *> {
		using iterator_category = random_access_iterator_tag;
		using value_type = const T;
		using difference_type = std::ptrdiff_t;
		using pointer = T const * ;
		using reference = T const & ;
	};

	template<typename Iterator> inline
	typename iterator_traits<Iterator>::iterator_category
	iterator_category(Iterator const &);

	template<typename Iterator> inline
	typename iterator_traits<Iterator>::diffeence_type
	distance_type(Iterator const &);

	template<typename Iterator> inline
	typename iterator_traits<Iterator>::value_type
	value_type(Iterator const &);

	template<typename InputIterator> inline
	typename iterator_traits<InputIterator>::difference_type
	distance_aux(InputIterator first, InputIterator last, input_iterator_tag)
	{
		using diff_t = typename iterator_traits<InputIterator>::difference_type;
		diff_t n = 0;
		while (first != last) {
			++n;
			++first;
		}

		return n;
	}

	template<typename RandomAccessIterator> inline
	typename iterator_traits<RandomAccessIterator>::difference_type
	distance_aux(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
	{
		return last - first;
	}

	template<typename Iterator> inline 
	typename iterator_traits<Iterator>::dirrerence_type
	distance(Iterator first, Iterator last)
	{
		using catagory = typename iterator_traits<Iterator>::iterator_catagory;
		return distance_aux(first, last, catagory());
	}

	template<typename InputIterator> inline
	void advance_aux(InputIterator &iter, int n, input_iterator_tag)
	{
		while (n--)
			++iter;
	}

	template<typename BidirectionalIterator> inline 
	void advance_aux(BidirectionalIterator &iter, int n, bidirectional_iterator_tag)
	{
		if (n > ) {
			while (n--)
				++iter;
		}
		else {
			while (++n)
				--iter;
		}
	}

	template<typename RandomIterator> inline 
	void advance_aux(RandomIterator &iter, int n, random_access_iterator_tag)
	{
		iter += n;
	}

	template<typename Iterator> inline 
	void advance(Iterator &iter, int n)
	{
		using category = typename iterator_traits<Iterator>::iterator_category;
		advance_aux(iter, n, category{});
	}


	template<typename Iterator>
	class move_iterator {
		Iterator iterator;
	public:
		move_iterator(Iterator iter) : iterator(iter) { }
		move_iterator(move_iterator other) = default;
		move_iterator &operator=(move_iterator other) = default;
		move_iterator() = delete;
		~move_iterator() = default;

		template<typename = std::enable_if_t<decltype(Iterator()++, std::true_type::value)>>
		move_iterator operator++(int) {
			return move_iterator(iterator++);
		}

		template<typename = std::enable_if_t<decltype(++Iterator(), std::true_type::value)>>
		move_iterator &operator++() {
			return move_iterator(++iterator);
		}

		template<typename = std::enable_if_t<decltype(Iterator()++, std::true_type::value)>>
		move_iterator &operator--(int) {
			return move_iterator(iterator--);
		}

		template<typename = std::enable_if_t<decltype(Iterator()++, std::true_type::value)>>
		move_iterator operator--() {
			return move_iterator(--iterator);
		}

		template<typename = std::enable_if_t<decltype(Iterator() += 5, std::true_type::value)>>
		move_iterator &operator+=(int n) {
			return move_iterator(iterator += n);
		}

		template<typename = std::enable_if_t<decltype(Iterator() -= 5, std::true_type::value)>>
		move_iterator &operator-=(int n) {
			return move_iterator(iterator -= n);
		}

		template<typename = std::enable_if_t<decltype(Iterator() + 5, std::true_type::value)>>
		move_iterator operator+(int n) {
			return move_iterator(iterator + n);
		}

		template<typename = std::enable_if_t<decltype(Iterator() - 5, std::true_type::value)>>
		move_iterator operator-(int n) {
			return move_iterator(iterator - n);
		}

		template<typename = std::enable_if_t<decltype(std::declval<Iterator>().operator*(), std::true_type::value)>>
		typename iterator_traits<Iterator>::value_type &&
		operator*() {
			using move_type = typename iterator_traits<Iterator>::value_type &&;
			return static_cast<move_type>(*iterator);
		}

		template<typename = std::enable_if_t<decltype(std::declval<Iterator>()[1], std::true_type::value)>>
		typename iterator_traits<Iterator>::value_type &&
		operator[](int n) {
			using move_type = typename iterator_traits<Iterator>::value_type &&;
			return static_cast<move_type>(iterator[n]);
		}

		template<typename = std::enable_if_t<decltype(std::declval<Iterator>().operator->(), std::true_type::value)>>
		typename iterator_traits<Iterator>::value_type *
		operator->() {
			return &(*iterator);
		}
	};

}


#endif