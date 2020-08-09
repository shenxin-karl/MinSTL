#include <iostream>
#include "test_head.hpp"
#include "list.hpp"

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
}

int main(void) {
	list_constrcut();
	system("pause");
}