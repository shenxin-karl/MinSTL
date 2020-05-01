#include <iostream>
#include <cstdlib>
#include "mallocator.hpp"
#include <vector>

int main(void)
{
	using vec_iter = std::vector<int>::iterator;
	using int_type = int;

	std::cout << typeid(int_type()).name() << std::endl;
	system("pause");
}