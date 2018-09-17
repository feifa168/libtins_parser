#include <boost/dll/import.hpp>
#include <boost/dll/alias.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "../include/sniff_plugin.h"
#include "../include/sniff_config_parse .h"

namespace sniff_proto {
    
    class SniffPluginHttp : public ISniffPlugin {
    public:
        static const std::string default_name;
        static const unsigned short default_port;

        SniffPluginHttp(std::string config, boost::function<create_config_parser> &parse_dll) : config_(config), parse_dll_(parse_dll) {}

        VIRTUAL_METHOD void CALL_METHOD do_parse() {
        }

        VIRTUAL_METHOD bool CALL_METHOD parse_config() {
            namespace fs = boost::filesystem;
            fs::path lib_path(config_, fs::native);
            if (!fs::exists(lib_path)) {
                message_ = lib_path.string() + "file is not exist";
                return false;
            }
            if (!fs::is_directory(lib_path)) {

                boost::shared_ptr<ISniffConfig> config_parser = parse_dll_(lib_path.string());
                if (config_parser->parse()) {
                    const SniffProtoConfig &proto = config_parser->get_sniff_proto();
                    name_ = proto.name;
                    ports_ = proto.ports;
                }
                else {
                    message_ = config_parser->get_error_message();
                    return false;
                }
            }

            return true;
        }

        VIRTUAL_METHOD std::string CALL_METHOD get_name() {
            if (name_.empty()) {
                name_ = SniffPluginHttp::default_name;
            }
            return name_;
        }

        VIRTUAL_METHOD std::vector<unsigned short> CALL_METHOD get_ports() {
            if (ports_.empty()) {
                ports_.push_back(SniffPluginHttp::default_port);
            }
            return ports_;
        }

        VIRTUAL_METHOD const char*  CALL_METHOD get_error_message() { return message_.c_str(); }

        virtual ~SniffPluginHttp() {}

    static boost::shared_ptr<SniffPluginHttp> create_sniff_plugin(std::string config, boost::function<create_config_parser> &parse_dll) {
        return boost::shared_ptr<SniffPluginHttp>(new sniff_proto::SniffPluginHttp(config, parse_dll));
    }

    private:
        std::string config_;
        std::string name_;
        std::vector<unsigned short> ports_;
        boost::function<create_config_parser> &parse_dll_;
        std::string message_;
    };

    const std::string SniffPluginHttp::default_name = "http";
    const unsigned short SniffPluginHttp::default_port = 80;

} // namespace sniff_proto


BOOST_DLL_ALIAS(sniff_proto::SniffPluginHttp::create_sniff_plugin, create_plugin)

