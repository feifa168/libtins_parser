#include <iostream>
#include <vector>
#include <string>
#include "stdio.h"
using namespace std;

class A {
public:
    int m_k;
    int m_t;
    A(int k, int t) :m_k(k), m_t(t) {
        cout << "construct...." << endl;
    }
    ~A() {
        cout << "destruct...." << endl;
    }
    A(A &a) {
        cout << "copy construct..." << endl;
    }
};

//  如果函数返回值是一个对象，要考虑return语句的效率  
A getObjRef() {
    /***********  直接返回临时对象  *************/
    // 编译器直接把临时对象创建并初始化在外部  
    // 存储单元(主调函数的栈帧上)中，省去了拷  
    // 贝和析构的花费，提高了效率  
    /*****************************************/
    return A(3, 4);
}
//  如果函数返回值是一个对象，要考虑return语句的效率  
A getObj() {
    /************** 返回本地对象 ****************/
    /* 以下这种写法实际上执行了三步:
    1. 构造本地对象a
    2. 调用拷贝构造，将本地对象a拷贝到外部存储器
    3. 调用析构函数析构本地对象a
    */
    /******************************************/
    A a(3, 4);
    return a;
}

class obj {
public:
    obj() { cout << ">> create obj " << endl; }
    obj(const obj& other) { cout << ">> copy create obj " << endl; }
};

vector<obj> foo() {
    vector<obj> c;
    c.push_back(obj());

    cout << "---- exit foo ----" << endl;
    return c;
}

void test_vector() {
    vector<obj> k;
    k = foo();
}

template <class T>
class container
{
public:
    T* value;

public:
    container() : value(NULL) {};
    ~container() { delete value; }

    container(const container& other)
    {
        value = new T(*other.value);
    }

    const container& operator = (const container& other)
    {
        delete value;
        value = new T(*other.value);
        return *this;
    }

    void push_back(const T& item)
    {
        delete value;
        value = new T(item);
    }
};

container<obj> foo_container()
{
    container<obj> c;
    c.push_back(obj());

    cout << "---- exit foo ----" << endl;
    return c;
}

void test_container() {
    container<obj> k;
    k = foo_container();
}

string test_string_fun()
{
    char c1[] = "hello world";
    string str("tom");
    str += "jerry";
    return str;
}

void test_rvalue() {
    std::vector<std::string> tokens;
    tokens.push_back("tom");
    tokens.push_back("jerry");
    std::vector<std::string> t = std::move(tokens);
}


int test_return_temp_object() {
    A a = getObj();   //  外部存储单元，但是对象的值没有了，a.m_t和a.m_k是随机值
    A refa = getObjRef();  // 对象的值是有的，refa.m_t==3, refa.m_k=4

    cout << "====================" << endl;
    test_vector();
    cout << "====================" << endl;
    test_container();
    cout << "====================" << endl;
    test_rvalue();

    string s1 = test_string_fun();

    return 0;
}