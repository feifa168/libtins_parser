#ifndef SNIFF_PROTO_SNIFF_CONFIG_PARSE_H
#define SNIFF_PROTO_SNIFF_CONFIG_PARSE_H
#include <vector>
#include <map>

class SniffConfigParse
{
public:
	SniffConfigParse(std::string json_file);
	bool parse();
	const std::map<std::string, std::vector<int> >& get_sniff_proto();
	void display();
protected:
private:
	std::map<std::string, std::vector<int> > sniff_proto;
	std::string json_file;
};

#endif  // SNIFF_PROTO_SNIFF_CONFIG_PARSE_H