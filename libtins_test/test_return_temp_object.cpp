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

//  �����������ֵ��һ������Ҫ����return����Ч��  
A getObjRef() {
    /***********  ֱ�ӷ�����ʱ����  *************/
    // ������ֱ�Ӱ���ʱ���󴴽�����ʼ�����ⲿ  
    // �洢��Ԫ(����������ջ֡��)�У�ʡȥ�˿�  
    // ���������Ļ��ѣ������Ч��  
    /*****************************************/
    return A(3, 4);
}
//  �����������ֵ��һ������Ҫ����return����Ч��  
A getObj() {
    /************** ���ر��ض��� ****************/
    /* ��������д��ʵ����ִ��������:
    1. ���챾�ض���a
    2. ���ÿ������죬�����ض���a�������ⲿ�洢��
    3. �������������������ض���a
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
    A a = getObj();   //  �ⲿ�洢��Ԫ�����Ƕ����ֵû���ˣ�a.m_t��a.m_k�����ֵ
    A refa = getObjRef();  // �����ֵ���еģ�refa.m_t==3, refa.m_k=4

    cout << "====================" << endl;
    test_vector();
    cout << "====================" << endl;
    test_container();
    cout << "====================" << endl;
    test_rvalue();

    string s1 = test_string_fun();

    return 0;
}