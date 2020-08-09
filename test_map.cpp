#include <iostream>
#include <string>
#include <cstdlib>
#include "vector.hpp"
#include "map.hpp"
#include "unordered_set.hpp"
#include "unordered_map.hpp"

using std::cout;
using std::ends;
using std::endl;
#if 0
using sx::vector;
using std::string;
//template class sx::map<int, int>;
//template class sx::multimap<int, int>;
template class sx::unordered_set<int>;
template class sx::unordered_multiset<int>;

template class sx::unordered_map<int, int>;

using type = sx::map<int, int>;
using multi_type = sx::multimap<int, int>;




template<typename Container>
void print(Container const &container, string name) {
	cout << "---------- " << name << "----------" << endl;

	for (auto &val : container)
		cout << "[" << val.first << ", " << val.second << "]" << endl;

	cout << endl;
	cout << "---------- " << name << "----------" << endl;
}


void construct() {
	vector<std::pair<int, int>> vec;
	for (int i = 0; i < 20; i++)
		vec.push_back(std::make_pair(i, rand() % 50));
	
	type map1;
	type map2(vec.begin(), vec.end());
	//print(map2, "map2");

	type map3(map2);
	//print(map3, "map3");

	type map4(std::move(map3));
	//print(map3, "map3");
	//print(map4, "map4");

	type map5;
	map5 = map4;
	print(map5, "map5");

	type map6;
	map6 = std::move(map5);
	print(map5, "map5");
	print(map6, "map6");
	cout << map6.size();
}
#endif

//using iterator = sx::map<int, int>::iterator;
//using const_iterator = sx::map<int, int>::const_iterator;

#if 0
void insert() {
	vector<std::pair<int, int>> vec;
	for (size_t i = 0; i < 50; i++)
		vec.push_back({i, i});

	type map1(vec.begin(), vec.end());

	std::pair<iterator, bool> result = map1.insert({ 54, 100 });
	//print(map1, "map1");

	vector<std::pair<int, int>> vec1;
	for (int i = 70; i < 90; ++i)
		vec1.push_back({ i, i });

	map1.insert(vec1.cbegin(), vec1.cend());
	print(map1, "map1");
}
#endif

#if 0
void erase() {
	vector<std::pair<int, int>> vec;
	for (int i = 0; i < 20; ++i)
		vec.emplace_back(std::make_pair(i, i));

	type map1(vec.begin(), vec.end());
	map1.erase(10);
	// print(map1, "map1");

	iterator first = map1.begin();
	++first;
	iterator last = first;
	sx::advance(last, 5);
	map1.erase(first, last);
	//print(map1, "map1");
}
#endif

#if 0
void find() {
	vector<std::pair<int, int>> vec;
	for (int i = 0; i < 20; ++i)
		vec.emplace_back(std::make_pair(i, i));

	type map1(vec.cbegin(), vec.cend());
	iterator result = map1.find(5);
	if (result != map1.end()) 
		cout << "[" << (*result).first << ", " << (*result).second << "]" << endl;
	
	map1[6] = 10;
	print(map1, "map1");

	iterator first = map1.lower_bound(6);
	if (first != map1.end())
		cout << "[" << (*first).first << ", " << (*first).second << "]" << endl;

	iterator upper_iter = map1.upper_bound(6);
	if (upper_iter != map1.end()) 
		cout << "[" << (*upper_iter).first << ", " << (*upper_iter).second << "]" << endl;

	std::pair<iterator, iterator> range = map1.equal_range(9);
	for (; range.first != range.second; ++range.first)
		cout << "[" << (*range.first).first << ", " << (*range.first).second << "]" << endl;
}
#endif

#if 0
void foo() {
	type map1;
	cout << "map1.empty: " << map1.empty() << endl;
	cout << "map1.size: " << map1.size() << endl;

	vector<std::pair<int, int>> vec;
	for (int i = 0; i < 20; ++i)
		vec.push_back({ i, i });
	
	map1.insert(vec.cbegin(), vec.cend());
	cout << "map1.empty: " << map1.empty() << endl;
	cout << "map1.size: " << map1.size() << endl;
}
#endif

#if 0
void multi_construct() {
	vector<std::pair<int, int>> vec;
	for (int i = 0; i < 20; ++i)
		vec.push_back({ rand() % 20, i });

	multi_type mmap1(vec.begin(), vec.end());
	print(mmap1, "mmap1");

	mmap1.insert(std::pair<int, int>{ 5, 10 });
	mmap1.insert(std::pair<int, int>{ 50, 10 });
	print(mmap1, "mmap1");

	multi_type mmap2 = std::move(mmap1);
	cout << "mmap1.size: " << mmap1.size() << endl;
	cout << "mmap2.size: " << mmap2.size() << endl;

	multi_type mmap3 = mmap2;
	cout << "mmap3.szie: " << mmap3.size() << endl;
	print(mmap3, "mmap3");

	multi_type mmap4;
	mmap4 = std::move(mmap3);
	cout << "mmap3.size: " << mmap3.size() << endl;
	cout << "mmap4.size: " << mmap4.size() << endl;
}
#endif

#if 0
void multi_insert() {
	vector<std::pair<int, int>> vec;
	for (int i = 0; i < 20; ++i)
		vec.push_back({ i, i });

	multi_type map1;
	map1.insert(vec.cbegin(), vec.cend());
	print(map1, "map1");

	cout << "map1.size: " << map1.size() << endl;
	cout << "map1.max: " << (*map1.max()).first << endl;
	cout << "map1.min: " << (*map1.min()).first << endl;

	map1.insert({ 0, 100 });
	print(map1, "map1");
	
	map1.emplace(10, 50);
	print(map1, "map1");
}
#endif

#if 0
void erase() {
	vector<std::pair<int, int>> vec;
	for (int i = 0; i < 20; i++) {
		vec.push_back({ 1, 1 });
	}


	multi_type map(vec.cbegin(), vec.cend());
	map.erase(1);
	print(map, "map");


	auto min = map.min();
	auto max = map.max();

	if (min != map.end())
		cout << (*min).first << endl;

	if (max != map.end())
		cout << (*max).first << endl;
}
#endif // 1

#if 0
void multi_find() {
	vector<std::pair<int, int>> vec;
	for (size_t i = 0; i < 50; i++)
		vec.push_back({ i, i });

	multi_type mset1(vec.begin(), vec.end());
	auto result = mset1.find(5);
	if (result != mset1.end())
		cout << (*result).first << endl;
}
#endif
#if 0
int main() {
	//construct();
	//insert();
	//erase();
	//find();
	//foo();

	//multi_construct();
	//multi_insert();
	//erase();
	//multi_find();

	cout << endl;
	system("pause");
}
#endif
