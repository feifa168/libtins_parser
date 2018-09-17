#include <iostream>
using namespace std;

namespace templateTest {
	//ģ�淺��
	template<typename T>
	class iterator_traits
	{
	public:
		iterator_traits()
		{
			cout << "ģ�淺��" << endl;
		}

		~iterator_traits()
		{
		}
	};

	//ƫ�ػ�
	template<typename T>
	class iterator_traits<T*>
	{
	public:
		iterator_traits()
		{
			cout << "ģ��ƫ�ػ����ػ�����ָ��" << endl;
		}

		~iterator_traits()
		{
		}
	};

	//ƫ�ػ�
	template<typename T>
	class iterator_traits<const T*>
	{
	public:
		iterator_traits()
		{
			cout << "ģ��ƫ�ػ����ػ�constָ��" << endl;
		}

		~iterator_traits()
		{
		}
	};

	//ȫ�ػ�
	template<>
	class iterator_traits<int>
	{
	public:
		iterator_traits()
		{
			cout << "ģ��ȫ�ػ�int����" << endl;
		}

		~iterator_traits()
		{
		}
	};
};

//����
template<class U, class T>
class Test
{
public:
	Test()
	{
		cout << "Test ����" << endl;
	}
};

//ƫ�ػ�
template< class T>
class Test<int, T>
{
public:

	Test()
	{
		cout << "Test ƫ�ػ�" << endl;
	}
};

//ȫ�ػ�
template<>
class Test<int, char>
{
public:

	Test()
	{
		cout << "Test ȫ�ػ�" << endl;
	}
};
template<typename T>
void max(const T& t1, const T & t2)
{
	cout << "ģ�溯������" << endl;
}

//��ʵ����ģ�治����ƫ�ػ�,ֻ��ȫ�ػ�
template<>
void max<int>(const int& t1, const int& t2)
{
	cout << "ģ�溯���ػ�" << endl;
}

void test_template_tehua()
{
	//     ģ��ȫ�ػ�int����
	//     ģ�淺��
	//     ģ��ƫ�ػ����ػ�����ָ��
	//     ģ��ƫ�ػ����ػ�constָ��
	//     Test ƫ�ػ�
	//     Test ����
	//     Test ȫ�ػ�
	//     ģ�溯���ػ�
	//     ģ�溯������

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