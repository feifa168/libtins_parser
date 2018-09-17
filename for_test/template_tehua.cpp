#include <iostream>
using namespace std;

namespace templateTest {
	//模版泛化
	template<typename T>
	class iterator_traits
	{
	public:
		iterator_traits()
		{
			cout << "模版泛化" << endl;
		}

		~iterator_traits()
		{
		}
	};

	//偏特化
	template<typename T>
	class iterator_traits<T*>
	{
	public:
		iterator_traits()
		{
			cout << "模版偏特化，特化常规指针" << endl;
		}

		~iterator_traits()
		{
		}
	};

	//偏特化
	template<typename T>
	class iterator_traits<const T*>
	{
	public:
		iterator_traits()
		{
			cout << "模版偏特化，特化const指针" << endl;
		}

		~iterator_traits()
		{
		}
	};

	//全特化
	template<>
	class iterator_traits<int>
	{
	public:
		iterator_traits()
		{
			cout << "模版全特化int类型" << endl;
		}

		~iterator_traits()
		{
		}
	};
};

//泛化
template<class U, class T>
class Test
{
public:
	Test()
	{
		cout << "Test 泛化" << endl;
	}
};

//偏特化
template< class T>
class Test<int, T>
{
public:

	Test()
	{
		cout << "Test 偏特化" << endl;
	}
};

//全特化
template<>
class Test<int, char>
{
public:

	Test()
	{
		cout << "Test 全特化" << endl;
	}
};
template<typename T>
void max(const T& t1, const T & t2)
{
	cout << "模版函数泛化" << endl;
}

//其实函数模版不存在偏特化,只有全特化
template<>
void max<int>(const int& t1, const int& t2)
{
	cout << "模版函数特化" << endl;
}

void test_template_tehua()
{
	//     模版全特化int类型
	//     模版泛化
	//     模版偏特化，特化常规指针
	//     模版偏特化，特化const指针
	//     Test 偏特化
	//     Test 泛化
	//     Test 全特化
	//     模版函数特化
	//     模版函数泛化

	templateTest::iterator_traits<int> t1;
	templateTest::iterator_traits<float> t2;
	templateTest::iterator_traits<int *> t3;
	templateTest::iterator_traits<const int *> t4;
	Test<int, int> t5;
	Test<float, int> t6;
	Test<int, char> t7;
	max(5, 10);
	max(5.5, 10.5);
}