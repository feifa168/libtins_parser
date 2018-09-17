#include "sniff_config_parse.h"

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/typeof/typeof.hpp"
#include "boost/optional.hpp"
#include <boost/foreach.hpp>

#include <iostream>

SniffConfigParse::SniffConfigParse(std::string json_file) {
	this->json_file = json_file;
}

const std::map<std::string, std::vector<int> >& SniffConfigParse::get_sniff_proto() {
	return sniff_proto;
}

bool SniffConfigParse::parse() {
	using namespace boost;
	using namespace boost::property_tree;

	boost::property_tree::ptree pt;
	boost::property_tree::read_json(json_file, pt);

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

	return true;
}

void SniffConfigParse::display() {
	using namespace std;
	for (auto proto : sniff_proto) {
		cout << "proto " << proto.first << " port [";
		for (auto port : proto.second) {
			cout << port << ", ";
		}
		cout << "]" << endl;
	}
}