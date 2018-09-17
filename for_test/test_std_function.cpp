#include <functional>
#include <iostream>
#include <algorithm>
#include <stdarg.h> // for va_list, va_start, va_arg, va_end
#include <tuple>

using std::function;
using std::bind;
using std::cout;
using std::endl;
using std::for_each;

// class CLS_A_Qwersdf {
// public:
//     CLS_A_Qwersdf() : a(5) {}
//     CLS_A_Qwersdf(int m) : a(m) {}
//     void fun1() {
//         cout << "fun1 " << a << endl;
//     }
//
// private:
//     void fun2() {
//         cout << "fun2 " << a << endl;
//     }
//
//     int a;
// };
//
// class CLS_B_SLKDJFOE : public CLS_A_Qwersdf {
// public:
//     CLS_B_SLKDJFOE() :CLS_A_Qwersdf() {}
//     CLS_B_SLKDJFOE(int a) :CLS_A_Qwersdf(a) {}
//     void fun3() {
//         this->CLS_A_Qwersdf::fun();
//     }
// };
//
// void test_class_341234() {
//     CLS_A_Qwersdf cls1(8);
//     cls1.fun1();
//     cls1.CLS_A_Qwersdf::fun1();
//
//     CLS_B_SLKDJFOE cls2(9);
//     cls2.fun1();
//     cls2.fun3();
// }

template<class T>
class class_for_each {
public:
	void operator()(const T &x) {
		cout << x << endl;
	}
	void display_non_static(const T&x) {
		cout << x << endl;
	}

	static void display_static(const T&x) {
		cout << x << endl;
	}
};

template<class T>
void display_int_sdkfj(const T&x) {
	cout << x << endl;
}
void test_for_each() {
	int ia[] = { 4, 5, 9, 10, 3 };
	cout << "====================" << endl;
	std::for_each(ia, ia + 5, [](int &a) { cout << a << endl; });
	cout << "====================" << endl;
	std::for_each(ia, ia + 5, class_for_each<int>());
	cout << "====================" << endl;
	std::for_each(ia, ia + 5, display_int_sdkfj<int>);
	cout << "====================" << endl;
	std::for_each(ia, ia + 5, class_for_each<int>::display_static);
	cout << "========= bind class static func ===========" << endl;
	std::for_each(ia, ia + 5, std::bind(&class_for_each<int>::display_static, std::placeholders::_1));
	cout << "==========bind class non static func ==========" << endl;
	class_for_each<int> cfe;
	std::for_each(ia, ia + 5, std::bind(&class_for_each<int>::display_non_static, &cfe, std::placeholders::_1));
	cout << "====================" << endl;
}

int max_sdfjl(int n, ...)  // 定参 n 表示后面变参数量，定界用，输入时切勿搞错
{
	va_list ap;       // 定义一个 va_list 指针来访问参数表
	va_start(ap, n);     // 初始化 ap，让它指向第一个变参
	int maximum = -0x7FFFFFFF;   // 这是一个最小的整数
	int temp;
	for (int i = 0; i < n; i++)
	{
		temp = va_arg(ap, int);   // 获取一个 int 型参数，并且 ap 指向下一个参数
		if (maximum < temp)
			maximum = temp;
	}
	va_end(ap);       // 善后工作，关闭 ap
	return maximum;
}

int printf_sdkfjl(const char* const fmt, ...) {
	va_list args;
	int n;
	char buf[2048];

	va_start(args, fmt);
	n = vsprintf(buf, fmt, args);
	va_end(args);

	cout << buf << endl;
	return n;
}

//递归终止函数
void print_23434()
{
	cout << "empty" << endl;
}
//展开函数
template <class T, class ...Args>
void print_23434(T head, Args... rest)
{
	cout << "parameter " << head << endl;
	int par_num = sizeof...(rest);
	if (par_num > 0)
		print_23434(rest...);
	else
		cout << "empty 2" << endl;
}

void my_more_args_fun() {
	cout << "last" << endl;
}

template<typename T, typename... Args>
void my_more_args_fun(T &t, Args... args) {
	cout << t << endl;
	my_more_args_fun(args...);
}

void test_more_args() {
	my_more_args_fun("tom", 2, 0.3);
	print_23434("tom", 2, "jerry", 0.5);
	printf_sdkfjl("%d %s %d", 5, "hello", 4);
	max_sdfjl(4, 5, 1, 9, 2);
}

void test_tuple() {
	auto tup1 = std::make_tuple(3.14, 1, 'a');
	std::tuple<int, std::string, short> tp1{ 1, "tom", 2 };
	int sz = tup1._Mysize;
	double d = std::get<0>(tup1);
	int    i = std::get<1>(tup1);
	auto   a = std::get<2>(tup1);
	cout << "typeid(std::get<2>(tup1)).name()" << typeid(std::get<2>(tup1)).name() << endl;
	std::tuple_element<0, decltype(tup1)>::type t;
	std::tuple_element<1, decltype(tup1)>::type t1;
	std::tuple_element<2, decltype(tup1)>::type t2;
}

int test_fun1_123kjsld(int a) {
	return a + 1;
}
void test_std_bind() {
	//test_more_args();
	//test_class_341234();
	//test_for_each();
//     std::function<int(int)> f1 = bind(test_fun1_123kjsld, std::placeholders::_1);
//     cout << f1(5) << endl;
	test_tuple();
}