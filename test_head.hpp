#pragma once
#include <iostream>

using std::cout;
using std::endl;
using std::ends;

struct String {
public:
	String() : str(nullptr), length(0) {}

	String(char const *str) : str(str), length(strlen(str)) {}

	~String() { cout << "~String()" << endl; }
public:
	char const	 *str;
	std::size_t   length;
};