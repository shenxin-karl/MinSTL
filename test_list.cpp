#include <iostream>
#include "test_head.hpp"
#include "list.hpp"
#include <array>
#include <cstdlib>
#include <ctime>

using std::cout;
using std::endl;
using std::ends;
using sx::list;

static void list_constrcut() {
	list<String> lst;
	list<String> lst1{ String("1"), String("2"), String("3") };
	list<String> lst2(lst1);
	lst = lst1;
	cout << "lst.size:" << lst.size() << endl;
	cout << "lst1.size:" << lst1.size() << endl;
	cout << "lst2.size:" << lst2.size() << endl;
	cout << "-----------------------" << endl;
	list<String> lst3(std::move(lst2));
	cout << "lst2.size:" << lst2.size() << endl;
	cout << "lst3.size:" << lst3.size() << endl;
	list<String> lst4;
	lst4 = std::move(lst1);
	print(lst4, "lst4");
	cout << "-----------------------" << endl;
	cout << "lst1.size:" << lst1.size() << endl;
	cout << "lst4.size:" << lst4.size() << endl;
}

static void list_iterator() {
	list<String> lst = { String("1"), String("2"), String("3") };

	cout << "iterator" << endl;
	for (auto iter = lst.begin(); iter != lst.end(); ++iter)
		cout << *iter << endl;

	cout << "const_iterator" << endl;
	for (auto iter = lst.cbegin(); iter != lst.cend(); ++iter)
		cout << *iter << endl;

	cout << "reverse_iterator" << endl;
	for (auto iter = lst.rbegin(); iter != lst.rend(); ++iter)
		cout << *iter << endl;

	cout << "const_reverse_iterator" << endl;
	for (auto iter = lst.crbegin(); iter != lst.crend(); ++iter)
		cout << *iter << endl;
}

static void list_insert() {
	list<String> lst;
	lst.insert(lst.begin(), String("1"));
	lst.insert(lst.begin(), String("2"));
	lst.insert(lst.begin(), String("3"));
	lst.insert(lst.begin(), String("4"));
	lst.insert(lst.begin(), String("5"));
	cout << "lst.size:" << lst.size() << endl;

	String str_arr[] = { String("6"), String("7"), String("8") };
	lst.insert(lst.end(), std::begin(str_arr), std::end(str_arr));
	cout << "lst.size:" << lst.size() << endl;
	lst.insert(lst.end(), { String("9"), String("10") });
	lst.insert(lst.end(), 5, String("0"));
	cout << "lst.size:" << lst.size() << endl;
	lst.emplace(lst.begin(), "123");
	lst.emplace_front("012");
	lst.emplace_back("789");
	print(lst, "lst");

	list<String> lst1;
	swap(lst, lst1);
	cout << "lst.size:" << lst.size() << endl;
	cout << "lst1.size:" << lst1.size() << endl;
}


static void list_remove() {
	list<String> lst({ String("1"), String("2"), String("3") });
	cout << "lst.size:" << lst.size() << endl;
	auto iter = lst.begin();
	size_t size = lst.size();
	for (int i = 0; i < size; ++i)
		iter = lst.erase(iter);
	cout << "lst.size:" << lst.size() << endl;
	print(lst, "lst");

	lst.insert(lst.end(), 5, String("5"));
	print(lst, "lst");
	lst.remove(String("5"));
	print(lst, "lst");
	cout << "lst.size:" << lst.size() << endl;

	lst.insert(lst.begin(), 5, String("6"));
	print(lst, "lst");
	lst.clear();
	cout << "lst.size:" << lst.size() << endl;
}

static void list_unique() {
	list<String> lst({ String("1"), String("1"), String("2"), String("2") });
	lst.unique();
	print(lst, "lst");
}

static void list_splice() {
	list<String> lst1 = { String("1"), String("2"), String("3") };
	list<String> lst2 = { String("4"), String("5"), String("6") };
	list<String> lst3 = lst2;
	lst1.splice(lst1.end(), lst2);
	print(lst1, "lst1");
	lst1.splice(lst1.end(), lst3, lst3.begin());
	print(lst1, "lst1");
	print(lst2, "lst2");
	print(lst3, "lst3");
}

static void list_sort() {
	list<int> lst;
	srand(static_cast<unsigned int>(std::time(nullptr)));
	for (int i = 0; i < 100; ++i)
		lst.emplace_back(rand() % 300);

	lst.sort();
	print(lst, "lst");
	lst.reverse();
	print(lst, "lst");
}

#if 0
int main(void) {
	//list_constrcut();
	//list_iterator();
	//list_insert();
	//list_remove();
	//list_unique();
	//list_splice();
	//list_sort();
	system("pause");
}
#endif