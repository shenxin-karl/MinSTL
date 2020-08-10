#include <iostream>
#include "test_head.hpp"
#include "list.hpp"
#include <array>

using std::cout;
using std::endl;
using std::ends;
using sx::list;

void list_constrcut() {
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

void list_iterator() {
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

void list_insert() {
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
	print(lst, "lst");
}


int main(void) {
	//list_constrcut();
	//list_iterator();
	list_insert();
	system("pause");
}