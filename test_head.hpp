#pragma once
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::ends;
using std::string;

struct String {
public:
	String() : str(nullptr), length(0) {}

	String(char const *str) : str(str), length(strlen(str)) {}

	~String() { cout << "~String()" << endl; }

	friend std::ostream &operator<<(std::ostream &os, String const &str) {
		cout << str.str;
		return os;
	}

	friend bool operator==(String const &first, String const &second) noexcept {
		if (first.str == second.str)
			return true;

		if (!first.str && second.str || first.str && !second.str)
			return false;

		return strcmp(first.str, second.str) == 0;
	}
public:
	char const	 *str;
	std::size_t   length;
};


template<typename Container>
void print(Container const &container, string name) {
	cout << "---------- " << name << "----------" << endl;

	for (auto const &val : container)
		cout << val << endl;

	cout << endl;
	cout << "---------- " << name << "----------" << endl;
}