#include <iostream>
#include "vector.hpp"
#include <string.h>

#if 0
using std::cout;
using std::endl;
using std::ends;
using sx::vector;


struct String {
public:
    String() : str(nullptr), length(0) {}

    String(char const *str) : str(str), length(strlen(str)) {}

    ~String() { cout << "~String()" << endl; }
public:
    char const	 *str;
    std::size_t   length; 
};


void vector_construct() {
    vector<String> vec;
    vector<String> vec1 = { 5, String() };
    vector<String> vec2 = vec1;
    vec = vec2;
    vector<String> vec3({ String(), String(), String() });
    cout << "vec1.size:" << vec1.size() << endl;
    cout << "vec2.size:" << vec2.size() << endl;
    cout << "vec.size:" << vec.size() << endl;
    cout << "vec3.size:" << vec3.size() << endl;
}

void vector_insert() {
    vector<String> vec;
	String str("hello world!");

    vec.push_back(str);
    cout << "vec.size:" << vec.size() << endl;

    for (int i = 0; i < 10; ++i) 
        vec.push_back(str);

    cout << "vec.size:" << vec.size() << endl;
	vec.pop_back();
	cout << "vec.size:" << vec.size() << endl;

	cout << "vec.size:" << vec.size() << endl;
	vec.insert(vec.end(), str);

	cout << "vec.empty:" << vec.empty() << endl;
	vec.emplace_back(str);

	vec.erase(vec.cbegin(), vec.cend());
	cout << "vec.size:" << vec.size() << endl;
}

void vector_iterator() {
    vector<String> vec;
	String str("hello world!");
	vec.insert(vec.begin(), 10, str);
	cout << "vec.size:" << vec.size() << endl;

    using iterator = vector<String>::iterator;
    using const_iterator = vector<String>::const_iterator;
    using reverse_iterator = vector<String>::reverse_iterator;
    using const_reverse_iterator = vector<String>::const_reverse_iterator;

    for (const_reverse_iterator iter = vec.crbegin(); iter != vec.crend(); ++iter)
        cout << (*iter).str << endl;

	vec.clear();
	cout << "vec.size:" << vec.size() << endl;
}

void vector_at() {
	vector<String> vec;
	String str("hello world!");
	vec.emplace(vec.begin(), str);
	vec.emplace(vec.end(), String("1111111!"));
	cout << "vec.size:" << vec.size() << endl;

	String &front = vec[0];
	String &str1 = vec.at(0);
	String &str2 = vec.front();
	String &str3 = vec.back();

	cout << front.str << endl;
	cout << str1.str << endl;
	cout << str2.str << endl;
	cout << str3.str << endl;

	vec[2];			
	vec.at(2);		// 越界访问, 直接抛异常
}

void vector_reserve() {
	vector<String> vec;
	cout << "vec.capacity:" << vec.capacity() << endl;
	vec.reserve(50);
	cout << "vec.capacity:" << vec.capacity() << endl;
	vec.reserve(20);			// 预留空间不能够缩小 vector
	cout << "vec.capacity:" << vec.capacity() << endl;
}

void vector_resize() {
	vector<String> vec;
	vec.resize(20);
	cout << "vec.capacity:" << vec.capacity() << endl;
	cout << "vec.size:" << vec.size() << endl;
	
	for (auto const &str : vec)
		cout << static_cast<void const *>(str.str) << endl;			// 20 个默认构造的 String
}
#endif

#if 0
int main(void) {
	//vector_construct();
	//vector_insert();
    //vector_iterator();
	//vector_at();
	//vector_reserve();
	//vector_resize();
	system("pause");
}
#endif