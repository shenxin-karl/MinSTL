#include <iostream>
#include "set.hpp"
#include "vector.hpp"
#include <cstdlib>
#include <string>
#include <utility>
#include <array>

// extern template class sx::set<int>;
using std::cout;
using std::ends;
using std::endl;
using sx::set;
using sx::vector;
using std::string;

#if 0
template<typename Container>
void print(Container const &container, string name) {
	cout << "---------- " << name << "----------" << endl;

	for (auto &val : container)
		cout << val << ends;

	cout << endl;
	cout << "---------- " << name << "----------" << endl;
}
#endif

using type = set<int>;

#if 0
void construct() {
	vector<int> vec;
	for (int i = 0; i < 100; ++i)
		vec.emplace_back(rand() % 200);
	/* 区间构造 */

	type set1(vec.begin(), vec.end());
	for (auto const &val : vec)
		cout << val << ends;
#endif
#if 0
	cout << endl << "------------------------------------\n";
	for (auto const &val : set1)
		cout << val << ends;
	cout << endl;
#endif

	/* 拷贝构造 */
#if 0
	type set2 = set1;
	for (auto const &val : set2)
		cout << val << ends;
	
	cout << endl << "size: " << set2.size();
#endif

	/* 移动构造 */
#if 0
	type set3(vec.begin(), vec.end());
	type set4 = std::move(set3);

	for (auto const &val : set3)
		cout << val << ends;

	cout << endl << endl;

	for (auto const &val : set4)
		cout << val << ends;

	cout << endl;
	cout << "set3.size: " << set3.size() << endl;
	cout << "set4.size: " << set4.size() << endl;
#endif

	/* 复制操作符 */
#if 0
	vector<int> vec1;
	for (int i = 0; i < 10; ++i)
		vec1.push_back(i);

	type set5(vec.begin(), vec.end());
	set5 = type();		/* 清空 */

	for (auto &val : set5)
		cout << val << ends;

	type set6(vec1.cbegin(), vec1.cend());
	set5 = set6;

	cout << "set5.size: " << set5.size() << endl;
#endif

	/* 移动赋值 */
#if 0
	type set7(vec.cbegin(), vec.cend());
	type set8;
	set8 = std::move(set7);

	cout << "set7.size: " << set7.size() << endl;
	cout << "set8.size: " << set8.size() << endl;
#endif

	/* 析构函数 */
#if 0
	type set9(vec.cbegin(), vec.cend());
	set9.~set();
	cout << "set9.size: " << set9.size() << endl;
}
#endif

#if 0
void insert() {
	vector<int> vec;
	for (int i = 0; i < 50; ++i)
		vec.emplace_back(rand() % 50);

	/* insert(velue_type const &) */
	type set1;
	auto par = set1.insert(50);

	cout << par.second << endl;
	cout << (par.first == set1.end()) << endl;

	par = set1.insert(50);
	cout << (par.first == set1.end()) << endl;

	cout << par.second << endl;

	print(set1, "set1");
#endif

	/* insert(first, last) */
#if 0
	type set2;
	set2.insert(vec.begin(), vec.end());
	print(set2, "set2");
	cout << "set.size: " << set2.size() << endl;
}
#endif

#if 0
void emplace() {
	type set1;
	auto par = set1.emplace(10);
	cout << par.second << endl;
	set1.emplace(10);

	cout << "set.size: " << set1.size() << endl;
}
#endif

#if 0
void erase() {
	vector<int> vec;
	for (int i = 0; i < 10; ++i)
		vec.push_back(i);
	type set1(vec.begin(), vec.end());
	print(set1, "set1");

	set1.erase(9);
	print(set1, "set1");
	cout << "set1.size: " << set1.size() << endl;
	type set2(vec.begin(), vec.end());
#endif
#if 0
	typename type::iterator iter = set2.begin();
	sx::advance(iter, 5);

	set2.erase(set2.begin(), iter);
	print(set2, "set2");
}
#endif

#if 0
void find() {
	vector<int> vec;
	for (int i = 0; i < 100; i++) 
		vec.emplace_back(rand() % 100);	

	type set1(vec.cbegin(), vec.cend());
	typename type::iterator resuelt = set1.find(10);

	if (resuelt != set1.end())
		cout << *resuelt << endl;
#endif

	/* lower_bound */
#if 0
	type set2(vec.begin(), vec.end());

	print(set2, "set2");

	auto iter = set2.lower_bound(10);
	if (iter != set2.end())
		cout << *iter << endl;
#endif

#if 0
	type set3(vec.cbegin(), vec.cend());
	print(set3, "set3");
	
	auto iter = set3.upper_bound(11);
	if (iter != set3.end())
		cout << *iter << endl;
}
#endif

using multi_type = sx::multiset<int>;

#if 0
void multi_construct() {
	/* 默认构造 */
	vector<int> vec;
	for (int i = 0; i < 50; i++)
		vec.push_back(rand() % 50);
	
	/* 默认构造 */
	multi_type mset1;
#endif

	/* 区间构造 */
#if 0
	// print(vec, "vec");
	multi_type mset2(vec.begin(), vec.end());
	// print(mset2, "mset2");
	// cout << "mset2.size: " << mset2.size() << endl;
#endif

	/* 拷贝构造 */
#if 0
	multi_type mset3(mset2);	
	// print(mset2, "mset2");
	// print(mset3, "mset3");

	// cout << "mset2.size " << mset2.size() << endl;
	// cout << "mset3.size "  << mset3.size() << endl;
#endif

	/* 移动构造 */
#if 0
	multi_type mset4 = std::move(mset3);
	print(mset3, "mset3");
	print(mset4, "mset4");
	cout << "mset3.size: " << mset3.size() << endl;
	cout << "mset4.size: " << mset4.size() << endl;
#endif

#if 0
	/* 赋值操作符 */
	vector<int> vec1;
	for (int j = 0; j < 20; j++)
		vec1.emplace_back(j);	
	
	multi_type mset5(vec1.begin(), vec1.end());
	mset5 = mset3;

	print(mset5, "mset5");
#endif
	/* 移动赋值 */
#if 0
	multi_type mset6(vec1.cbegin(), vec.cend());	
	mset6 = std::move(mset3);

	print(mset6, "mset6");

	cout << "mset3.size: " << mset3.size() << endl;
	cout << "mset6.size: " << mset6.size() << endl;
}
#endif

#if 0	
void multi_insert() {
	vector<int> vec;
	for (int i = 0; i < 20; i++)
		vec.emplace_back(i);

	/* insert(key_type const &) */
	multi_type mset1(vec.cbegin(), vec.cend());
	auto iter = mset1.insert(10);

	print(mset1, "mset1");
	if (iter != mset1.end())
		cout << *iter << endl;

	cout << "mset1.size: " << mset1.size() << endl;
#endif

	/* insert(iterator, key_type const &) */
#if 0
	multi_type mset2(vec.cbegin(), vec.cend());
	auto iter = mset2.insert(mset2.begin(), 11);
	print(mset2, "mset2");
	if (iter != mset2.end())
		cout << *iter << endl;	

	cout << "mset2.size: " << mset2.size() << endl;
#endif

	/* insert(InputIterator first, InputIterator last) */
#if 0
	multi_type mset3(vec.cbegin(), vec.cend());
	int arr[10] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
	mset3.insert(arr, arr + (sizeof(arr) / sizeof(int)));
	print(mset3, "mset3");
	cout << "mset3.size: " << mset3.size() << endl;

	cout << "mset.min: " << *mset3.min() << endl;
	cout << "mset.max: " << *mset3.max() << endl;
}
#endif

#if 0
void multi_emplace() {
	vector<int> vec;
	for (int i = 0; i < 10; i++)
		vec.emplace_back(i);
	
	multi_type mset1(vec.cbegin(), vec.cend());
	mset1.emplace(20);
	print(mset1, "mset1");
	cout << "mset1.size: " << mset1.size() << endl;
	cout << "mset1.min: " << *mset1.min() << endl;
	cout << "mset1.max: " << *mset1.max() << endl;
}
#endif

#if	0
void multi_erase() {
	vector<int> vec;
	for (int i = 0; i < 50; i++) {
		vec.push_back(i);
		vec.push_back(i);
	}
	
	multi_type mset1(vec.begin(), vec.end());
	mset1.erase(2);
	print(mset1, "mset1");
	mset1.erase(mset1.begin());
	print(mset1, "mset1");

	auto first = mset1.begin();
	auto last = first;
	sx::advance(last, 10);
	mset1.erase(first, last);	
	print(mset1, "mset1");
}
#endif

#if 0
void multi_find() {
	vector<int> vec;
	for (size_t i = 0; i < 50; i++)
		vec.push_back(i);

	multi_type mset1(vec.begin(), vec.end());
	auto result = mset1.find(5);
	if (result != mset1.end())
		cout << *result << endl;
}
#endif

#if 0
int main(void) {
	construct();
	insert();
	emplace();
	erase();
	find();

	multi_construct();
	multi_insert();
	multi_emplace();
	multi_erase();
	multi_find();
	
	cout << endl;
	system("pause");
}
#endif