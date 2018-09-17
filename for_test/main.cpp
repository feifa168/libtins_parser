#include <iostream>
#include "sniff_config_parse.h"

int test_unordered_map();
void test_unordered_map2();
void test_template_tehua();
void test_atomic();
int test_noexcept();
void test_std_bind();
void test_template_moree_args();

void test_boost_xml();
void test_boost_json();
void test_my_json();

void test_boost() {
	//test_boost_xml();
	//test_boost_json();
	test_my_json();
}

void main() {
	//test_template_tehua();

//     test_unordered_map2();
	//test_unordered_map();
	//test_atomic();
	//test_noexcept();

//     test_std_bind();
//     test_template_moree_args();

	//test_boost();

	SniffConfigParse sniff_config("sniff_proto.json");
	sniff_config.parse();
	sniff_config.display();

	system("pause");
}