#include "stdio.h"
#include <string>
#include <iostream>
using namespace std;

class MyString
{
public:
    void init() {
        m_pData = NULL;
        m_nLen = 0;
    }
    MyString() :m_pData(NULL), m_nLen(0)
    {
        cout << "MyString()" << endl;
    }
    MyString(const char *pStr)   // ������ʽת��
    {
        cout << "MyString(const char *pStr)" << endl;
        m_nLen = strlen(pStr);
        CopyData(pStr);
    }
    MyString(const MyString& other)
    {
        cout << "MyString(const MyString& other)" << endl;
        if (this != &other)
        {
            init();
            m_nLen = other.m_nLen;
            DeleteData();
            CopyData(other.m_pData);
        }
    }
    MyString& operator=(const MyString& other)
    {
        cout << "MyString& operator=(const MyString& other)" << endl;
        if (this != &other)
        {
            init();
            m_nLen = other.m_nLen;
            DeleteData();
            CopyData(other.m_pData);
        }

        return *this;
    }

    MyString(MyString&& other)
    {
        cout << "MyString(MyString&& other)" << endl;
        m_nLen = other.m_nLen;
        m_pData = other.m_pData;
        other.m_pData   = NULL;
        other.m_nLen    = 0;
    }

    MyString& operator=(MyString&& other)
    {
        cout << "MyString& operator=(const MyString&& other)" << endl;
        if (this != &other)
        {
            m_nLen = other.m_nLen;
            m_pData = other.m_pData;
            other.m_pData = NULL;
        }

        return *this;
    }

    ~MyString()
    {
        DeleteData();
    }

private:
    void CopyData(const char *pData)
    {
        if (pData)
        {
            m_pData = new char[m_nLen + 1];
            memcpy(m_pData, pData, m_nLen);
            m_pData[m_nLen] = '\0';
        }
    }

    void DeleteData()
    {
        if (m_pData != NULL)
        {
            delete[] m_pData;
            m_pData = NULL;
        }
    }

private:
    char *m_pData;
    size_t m_nLen;
};

MyString Fun()
{
    MyString str = "hello world";
    return str;
}
MyString Fun2() {
    return MyString("tom");
}
void test_rvalue_string()
{
//     MyString str1 = "hello";
//     MyString str2(str1);
    MyString str3 = Fun();
    MyString str4 = std::move(str3);
    MyString str5 = Fun2();
}