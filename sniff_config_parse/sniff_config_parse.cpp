#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
//#include "boost/typeof/typeof.hpp"
//#include "boost/optional.hpp"
//#include <boost/foreach.hpp>
#include <boost/dll/alias.hpp>

#include "../include/sniff_config_parse .h"

namespace sniff_proto {

    class SniffConfigParse : public ISniffConfig {
    public:
        SniffConfigParse(std::string config_file) : config_file_(config_file) {}
        
        VIRTUAL_METHOD bool  CALL_METHOD parse() {
            using namespace boost;
            using namespace boost::property_tree;

            boost::property_tree::ptree pt;

            try {
                boost::property_tree::read_json(config_file_, pt);

                boost::property_tree::ptree proto = pt.get_child("proto");
                boost::property_tree::ptree ports;

                std::string name = proto.get<std::string>("name");
                std::vector<unsigned short> vtport;
                ports = proto.get_child("port");
                for (boost::property_tree::ptree::value_type port : ports) {
                    vtport.push_back(port.second.get_value<int>());
                }
                sniff_proto_config.name = name;
                sniff_proto_config.ports = vtport;
            }
            catch (json_parser_error &ec) {
                message_ = ec.what();
                return false;
            }
            catch (ptree_bad_path &ec) {
                message_ = ec.what();
                return false;
            }
            catch (std::exception &ec) {
                message_ = ec.what();
                return false;
            }
            catch (...) {
                message_ = "unknown error!";
                return false;
            }

            return true;
        }

        VIRTUAL_METHOD const SniffProtoConfig CALL_METHOD get_sniff_proto() { return sniff_proto_config; }

        VIRTUAL_METHOD const char*  CALL_METHOD get_error_message() { return message_.c_str(); }

        static boost::shared_ptr<sniff_proto::SniffConfigParse> get_sniff_config(std::string config_file) {
            return boost::shared_ptr<sniff_proto::SniffConfigParse>(new sniff_proto::SniffConfigParse(config_file));
        }

    private:
        SniffProtoConfig sniff_proto_config;
        std::string config_file_;
        std::string message_;
    };
}   // end of namespace sniff_proto

BOOST_DLL_ALIAS(
    sniff_proto::SniffConfigParse::get_sniff_config,                        // <-- this function is exported with...
    create_parse_config                                       // <-- ...this alias name
)


