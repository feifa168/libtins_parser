#include <cstdio>
#include <string>
#include <iostream>
using std::string;
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/typeof/typeof.hpp"
#include "boost/optional.hpp"
#include <boost/foreach.hpp>
using namespace boost::property_tree;

// <?xml version="1.0" encoding="UTF-8"?>
// <conf><!--conf comment-->
//     <gui>0</gui>
//     <theme id="001">matrix</theme>
//     <urls><!--urls comment-->
//         <url>http:://www.url1.com</url>
//         <url>http:://www.url2.com</url>
//         <url>http:://www.url3.com</url>
//         <url></url>
//     </urls>
//     <clock_style>24.35</clock_style>
// </conf>
void parse_boost_xml(std::istringstream &ss, ptree &pt) {
	//std::stringstream ss(xml);
	//read_xml("conf.xml", pt);
	read_xml(ss, pt);

	boost::optional<int> op = pt.get_optional<int>("conf.gui");//使用get_optional()获得节点，不存在则op为空
	if (op)
	{
		int i = op.get();
	}

	string str1 = pt.get<string>("conf.theme", "");//使用get()获得节点，不存在则返回""

	boost::optional<ptree&> child_null_test = pt.get_child_optional("conf.urls");
	if (child_null_test)
	{
		BOOST_AUTO(child, pt.get_child("conf.urls"));
		for (BOOST_AUTO(pos, child.begin()); pos != child.end(); ++pos)
		{
			string str = pos->second.get_value<string>();
			std::cout << str << std::endl;
		}
	}

	string str = pt.get("conf.clock_style", "");
}

void add_boost_xml(std::ostringstream &ss, ptree &pt) {
	pt.add("conf.urls.url", "http://www.url4.com");
	write_xml(ss, pt);
}
void update_boost_xml(std::ostringstream &ss, ptree &pt) {
	pt.put("conf.gui", 99);
	pt.put("conf.gui.<xmlattr>.com", "comment");
	write_xml(ss, pt);
}
void clear_boost_xml(std::ostringstream &ss, ptree &pt) {
	//pt.put("conf.gui", 0);
	pt.erase("conf.gui");
	write_xml(ss, pt);
}
void boost_xml_attr(ptree &pt) {
	using std::string;
	using std::cout;
	using std::endl;
	using namespace boost;

	BOOST_FOREACH(ptree::value_type &v, pt.get_child("conf.urls")) {
		ptree pitem;
		string name;
		if (v.first == "url") {
			pitem = v.second;
			name = pitem.get<string>("<xmlattr>.id");
			cout << name << endl;
		}
	}

	boost::optional<string> op = pt.get_optional<string>("conf.<xmlcomment>");
	if (op)
	{
		string strCommentConf = op.get();
		cout << strCommentConf << endl;
	}
	op = pt.get_optional<string>("conf.urls.<xmlcomment>");
	if (op)
	{
		string strCommentUrls = op.get();
		cout << strCommentUrls << endl;
	}
	op = pt.get_optional<string>("conf.theme.<xmlattr>.id");
	if (op)
	{
		string strAttr = op.get();
		cout << strAttr << endl;
	}
}

void test_boost_xml() {
	using namespace std;
	std::string xml = R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <conf><!--conf comment-->
            <gui>0</gui>
            <theme id="001">matrix</theme>
            <urls><!--urls comment-->
                <url id="1">http:://www.url1.com</url>
                <url id="2">http:://www.url2.com</url>
                <url id="3">http:://www.url3.com</url>
                <url id="4"></url>
            </urls>
            <clock_style>24.35</clock_style>
        </conf>
    )";
	std::istringstream ss(xml);
	ptree pt;
	parse_boost_xml(ss, pt);
	//cout << ss.str() << endl;

	std::ostringstream os1;
	add_boost_xml(os1, pt);
	//cout << os1.str() << endl;

	std::ostringstream os2;
	update_boost_xml(os2, pt);
	//cout << os2.str() << endl;

	std::ostringstream os3;
	clear_boost_xml(os3, pt);
	cout << os3.str() << endl;

	boost_xml_attr(pt);
}

#include "boost/property_tree/json_parser.hpp"
using namespace std;

void parse_boost_json(std::istringstream &ss, ptree &pt) {
	read_json(ss, pt);
	int i = pt.get<int>("conf.gui");
	cout << R"(get<int>(conf.gui) = )" << i << endl;

	boost::optional<int> op = pt.get_optional<int>("conf.gui");
	if (op) {
		i = op.get();
		cout << R"(get_optional<int>("conf.gui") = )" << i << endl;
	}

	boost::property_tree::ptree child_linktype = pt.get_child("conf.urls");
	BOOST_FOREACH(boost::property_tree::ptree::value_type &vt, child_linktype) {
		if (vt.first == "url") {
			cout << vt.second.data() << "|" << vt.second.get_value<string>() << ",";
		}
	}
	cout << endl;
}

void add_boost_json(std::ostringstream &ss, ptree &pt) {
	pt.add("conf.test", "this is for test");
	//     ptree subpt;
	//     subpt.put("url", "http://www.url4.com");
	//     pt.add_child("conf.urls", subpt);
	write_json(ss, pt);
}
void read_boost_json_arr(std::ostringstream &ss, ptree &pt) {
	ptree child = pt.get_child("conf.port");
	cout << R"(get_child("conf.port") = [)";
	for (ptree::value_type &vt : child) {
		string s1 = vt.first;
		auto port = vt.second.data();
		cout << port << ",";
	}
	cout << "]" << endl;
}
void add_boost_child_json(std::ostringstream &ss, ptree &pt) {
	boost::property_tree::ptree subpt = pt.get_child("conf.urls");
	subpt.add("url", "http://www.url4.com");
	pt.put_child("conf.urls", subpt);
	write_json(ss, pt);
}
void update_boost_json(std::ostringstream &ss, ptree &pt) {
	pt.put("conf.gui", 99);
	write_json(ss, pt);
}
void del_boost_json(std::ostringstream &ss, ptree &pt) {
	pt.put("conf.gui", 0);
	write_json(ss, pt);
}
void test_boost_json() {
	using namespace std;
	string json = R"(
        {
            "conf":
            {
                "gui": 1,
                "port": [80, 8080],
                "theme": "matrix",
                "urls":
                {
                    "url": "http://www.url1.com",
                    "url": "http://www.url2.com",
                    "url": "http://www.url3.com"
                },
                "clock_style": 24
            }
        }
    )";
	std::istringstream ss(json);
	ptree pt;
	parse_boost_json(ss, pt);
	cout << ss.str() << endl;

	std::ostringstream so;
	add_boost_json(so, pt);
	cout << so.str() << endl;
	so.clear();

	std::ostringstream so1;
	add_boost_child_json(so1, pt);
	cout << so1.str() << endl;

	std::ostringstream so5;
	read_boost_json_arr(so5, pt);
	cout << so5.str() << endl;

	std::ostringstream so2;
	update_boost_json(so2, pt);
	cout << so2.str() << endl;

	std::ostringstream so3;
	del_boost_json(so3, pt);
	cout << so3.str() << endl;
}

#include <vector>
#include <map>

void test_my_json() {
	string json = R"(
        {
            "protos": {
                "proto": {
                    "proto": "http",
                    "port": [80, 8080]
                },
                "proto": {
                    "proto": "mysql",
                    "port": [3306]
                },
                "proto": {
                    "proto": "rdp",
                    "port": [3309]
                },
                "proto": {
                    "proto": "ssh",
                    "port": [22]
                },
                "proto": {
                    "proto": "telnet",
                    "port": [23]
                }
            }
        }
    )";

	boost::property_tree::ptree pt;
	std::istringstream iss(json);
	//boost::property_tree::read_json(iss, pt);
	boost::property_tree::read_json("sniff_proto.json", pt);

	std::map<std::string, std::vector<int> > sniff_proto;

	boost::property_tree::ptree protos = pt.get_child("protos");
	boost::property_tree::ptree ports;
	for (boost::property_tree::ptree::value_type vt : protos) {
		if (vt.first == "proto") {
			std::vector<int> vtport;
			boost::property_tree::ptree &pr_proto = vt.second;
			std::string proto = pr_proto.get<std::string>("proto");
			ports = pr_proto.get_child("port");
			for (boost::property_tree::ptree::value_type sub_vt : ports) {
				vtport.push_back(sub_vt.second.get_value<int>());
			}
			sniff_proto.insert(std::pair<std::string, std::vector<int> >(proto, vtport));
		}
	}

	for (auto proto : sniff_proto) {
		cout << "proto " << proto.first << " port [";
		for (auto port : proto.second) {
			cout << port << ", ";
		}
		cout << "]" << endl;
	}
}