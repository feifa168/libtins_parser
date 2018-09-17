#include<iostream>
#include<string.h>
#include<bitset>
#include<boost/any.hpp>
#include<boost/utility.hpp>
#include<typeinfo>

using namespace std;

class InParam
{
public:
	template<typename... Arg>
	InParam(Arg... args)
	{
		PutArg(args...);
		// m_veParamValue.push_back( head );
		// cout << typeid( head ).name()  << " : " << head << " | ";
		// InParam( last... );  // 这个必须实现一个另外一个一样的构造函数
	}

	template<typename T>
	void PutArg(T value) {
		cout << typeid(value).name() << "|" << value << endl;
		m_veParamValue.push_back(value);
	}

	template<typename Head, typename... Rail>
	void PutArg(Head head, Rail... last) {
		const char *pname = typeid(head).name();
		cout << pname << "|" << head << endl;
		m_veParamValue.push_back(head);
		PutArg(last...);
	}

	~InParam() {
		cout << "~~~~ " << m_veParamValue.size() << endl;
	}

private:
	vector<boost::any> m_veParamValue;
};

struct X {
	int a;
	X() :a(5) { cout << "X" << endl; }
	~X() { cout << "~X" << endl; }
};
void inner(const X&) { cout << "inner(const X&)" << endl; }
void inner(X&&) { cout << "inner(X&&)" << endl; }
template<typename T>
void outer(T&& t) { inner(forward<T>(t)); }

void test_class_construct() {
	X a;
	{
		X *pa = &a;
	}
	{
		X &ra = a;
	}
}
void test_forward()
{
	{
		X a;
		//     outer(a);
		//     outer(X());
		//     inner(forward<X>(X()));
		//     inner(forward<X>(a));
		{
			X &a1 = forward<X>(X());
		}
		{
			X &a2 = forward<X>(a);
		}
	}
}

void test_template_moree_args()
{
	test_class_construct();
	test_forward();
	InParam ip("aaa", 5, 9.5555, 9999L);
	cout << "001" << endl;
}