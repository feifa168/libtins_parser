#include <iostream>
#include <unordered_map>
#include <map>
#include <string>
using namespace std;

int test_unordered_map()
{
	//ע�⣺VS2012�����°汾��֧��{}��ֵ
	unordered_map<int, string> myMap = { { 5, "�Ŵ�" },{ 6, "����" } };//ʹ��{}��ֵ
	myMap[2] = "����";  //ʹ��[ ]���е������룬���Ѵ��ڼ�ֵ2����ֵ�޸ģ���������롣
	myMap[2] = "�ŷ�";
	myMap.insert(pair<int, string>(3, "�¶�"));//ʹ��insert��pair����
	myMap[3] = "�¶���";
	myMap.insert(pair<int, string>(3, "������"));//ʹ��insert��pair����
	unordered_map<int, string>::hasher fn = myMap.hash_function();
	fn(4);

	//�������+��������ʹ��
	auto iter = myMap.begin();//auto�Զ�ʶ��Ϊ����������unordered_map<int,string>::iterator
	while (iter != myMap.end())
	{
		cout << iter->first << "," << iter->second << endl;
		++iter;
	}

	//����Ԫ�ز����+��������ʹ��
	auto iterator = myMap.find(2);//find()����һ��ָ��2�ĵ�����
	if (iterator != myMap.end())
		cout << endl << iterator->first << "," << iterator->second << endl;
	iterator = myMap.find(50);
	if (iterator == myMap.end()) {
		cout << "find end" << endl;
	}

	return 0;
}

struct KEY
{
	int first;
	int second;
	int third;

	KEY(int f, int s, int t) : first(f), second(s), third(t) {}
};

struct HashFunc
{
	std::size_t operator()(const KEY &key) const
	{
		using std::size_t;
		using std::hash;

		return ((hash<int>()(key.first)
			^ (hash<int>()(key.second) << 1)) >> 1)
			^ (hash<int>()(key.third) << 1);
	}
};

struct EqualKey
{
	bool operator () (const KEY &lhs, const KEY &rhs) const
	{
		return lhs.first == rhs.first
			&& lhs.second == rhs.second
			&& lhs.third == rhs.third;
	}
};
#include <functional>

void test_unordered_map2() {
	unordered_map<KEY, string, HashFunc, EqualKey> hashmap =
	{
		{ { 01, 02, 03 }, "one" },
		{ { 11, 12, 13 }, "two" },
		{ { 21, 22, 23 }, "three" },
	};

	KEY key(11, 12, 13);

	auto it = hashmap.find(key);

	if (it != hashmap.end())
	{
		cout << it->second << endl;
	}
}