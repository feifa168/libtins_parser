#include <iostream>
#include <unordered_map>
#include <map>
#include <string>
using namespace std;

int test_unordered_map()
{
	//注意：VS2012及以下版本不支持{}赋值
	unordered_map<int, string> myMap = { { 5, "张大" },{ 6, "李五" } };//使用{}赋值
	myMap[2] = "李四";  //使用[ ]进行单个插入，若已存在键值2，则赋值修改，若无则插入。
	myMap[2] = "张飞";
	myMap.insert(pair<int, string>(3, "陈二"));//使用insert和pair插入
	myMap[3] = "陈二两";
	myMap.insert(pair<int, string>(3, "陈三两"));//使用insert和pair插入
	unordered_map<int, string>::hasher fn = myMap.hash_function();
	fn(4);

	//遍历输出+迭代器的使用
	auto iter = myMap.begin();//auto自动识别为迭代器类型unordered_map<int,string>::iterator
	while (iter != myMap.end())
	{
		cout << iter->first << "," << iter->second << endl;
		++iter;
	}

	//查找元素并输出+迭代器的使用
	auto iterator = myMap.find(2);//find()返回一个指向2的迭代器
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