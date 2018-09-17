#include<cassert>
#include<cstring>
#include<iostream>
using namespace std;

void Throw() {
	throw 1;
}
void NoBlockThrow() {
	Throw();
}
//��ʾ���������׳��쳣������׳��ص���std::terminate�жϳ���ִ��
//noexceptΪ���η�
void BlockThrow()noexcept {
	Throw();
}

void test_noexcept1() {
	try {
		Throw();
	}
	catch (...) {
		cout << "Found Throw.\n";
	}

	try {
		NoBlockThrow();
	}
	catch (...) {
		cout << "Throw is no block.\n";
	}

	try {
		BlockThrow();
	}
	catch (...) {
		cout << "Found Throw 1.\n";
	}

	cout << "end" << endl;
}

class my_except : public exception {
public:
	my_except() {
		cout << "my_except()" << endl;
	}
	my_except(char *msg) : exception(msg) {
		//this->exception::exception(msg);
		cout << "my_except(msg)" << endl;
	}
	char const* what() const
	{
		return this->::exception::what();
		//return _msg.c_str();
	}

private:
};
struct A {
	~A() {
		throw 1;
	}
};
struct B {
	~B()noexcept(false) {
		throw 2;
	}
};
struct C {
	B b;
};
struct D {
	void test() noexcept(false) {
		my_except me("hello, this is for test exception\n");
		throw me;
	}
	//     ~D() noexcept(false) {
	//         throw my_except("my_except");
	//     }
};
void funA() { A a; }
void funB() { B b; }
void funC() { C c; }
void funD() {
	D d;
	d.test();
}

void test_noexcept2() {
	try {
		funB();
	}
	catch (...) {
		cout << "caught funB.\n";
	}

	try {
		funC();
	}
	catch (...) {
		cout << "caught funC.\n";
	}

	try
	{
		funD();
	}
	catch (my_except &e) {
		cout << e.what();
	}
	catch (...) {
		cout << "catch my_except" << endl;
	}

	try {
		funA();
	}
	catch (...) {
		cout << "caught funA.\n";
	}
}

void TestIntType()
{
	try
	{
		throw 1;
	}
	catch (...)
	{
		cout << "�� try block ��, ׼���׳�һ���쳣." << endl;
	}
}

void TestDoubleType()
{
	try
	{
		throw 0.5;
	}
	catch (...)
	{
		cout << "�� try block ��, ׼���׳�һ���쳣." << endl;
	}
}

void TestEmptyPointType()
{
	try
	{
		int* p = NULL;
		*p = 3;
	}
	catch (...)
	{
		cout << "�Ƿ���ַ�����쳣" << endl;     // ���񲻵��Ƿ���ַ����
	}
}

void TestDivZeroType()
{
	try
	{
		int b = 0;
		int a = 3 / b;
	}
	catch (...)
	{
		cout << "0���쳣" << endl;     // ���񲻵�����0
	}
}

void TestMemoryOutType()
{
	int * a = new int[4];
	try
	{
		for (int i = 0; i < 245; i++)
		{
			a++;
		}
		*a = 3;
	}
	catch (...)
	{
		cout << "�ڴ�Խ���쳣" << endl;   // ���񲻵�Խ��
	}
}

void test_normal_func_exception() {
	throw my_except("this is a normal exception");
}
void test_normal_func_exception2() noexcept(false) {
	throw my_except("this is a normal exception 2");
}

int test_noexcept()
{
	//test_noexcept1();
	//test_noexcept2();

	//TestEmptyPointType();
	//TestDivZeroType();
	//TestMemoryOutType();

	try {
		test_normal_func_exception();
		test_normal_func_exception2();
	}
	catch (my_except &e) {
		cout << e.what();
	}
	catch (...) {
		cout << "sdfsdfsdf" << endl;
	}
	//finally{} // ��׼c++û��finally���������������þֲ������������������ͷ���Դ

	return 0;
}