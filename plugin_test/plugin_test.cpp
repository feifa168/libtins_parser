#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <boost/dll/import.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include <boost/function.hpp>
#include <exception>

#include "../include/sniff_plugin.h"
#include "../include/sniff_config_parse .h"

using std::cout;
using std::endl;

bool get_config_parse(std::string dll_path, std::string dll_name, boost::function<create_config_parser> &create) {
    boost::filesystem::path lib_path(dll_path);
    lib_path /= dll_name;

    try {
        create = boost::dll::import_alias<create_config_parser>(
            lib_path,
            "create_parse_config",
            boost::dll::load_mode::append_decorations
            );
    }
    catch (boost::system::system_error &ec) {
        return false;
    }

    return true;
}

void display_config(SniffProtoConfig &config) {
    cout << config.name << " ";
    for (int port : config.ports) {
        cout << port << ", ";
    }
    cout << endl;
}
void test_config(boost::function<create_config_parser> &creator) {
    if (get_config_parse(R"(E:\cpp\WinpcapTest\bin\Debug)", "sniff_config_parse", creator)) {

        boost::shared_ptr<ISniffConfig> config_parser;
        SniffProtoConfig config;

        config_parser = creator(R"(E:\cpp\WinpcapTest\bin\Debug\plugins\SniffPluginHttp\config.json)");
        if (config_parser->parse()) {
            config = config_parser->get_sniff_proto();
            display_config(config);
        }
        else {
            cout << config_parser->get_error_message() << endl;
        }

        config_parser = creator(R"(E:\cpp\WinpcapTest\bin\Debug\plugins\SniffPluginRdp\config.json)");
        if (config_parser->parse()) {
            config = config_parser->get_sniff_proto();
            display_config(config);
        }
        else {
            cout << config_parser->get_error_message() << endl;
        }

        config_parser = creator(R"(E:\cpp\WinpcapTest\bin\Debug\plugins\SniffPluginSsh\config.json)");
        if (config_parser->parse()) {
            config = config_parser->get_sniff_proto();
            display_config(config);
        }
        else {
            cout << config_parser->get_error_message() << endl;
        }

        config_parser = creator(R"(E:\cpp\WinpcapTest\bin\Debug\plugins\SniffPluginTelnet\config.json)");
        if (config_parser->parse()) {
            config = config_parser->get_sniff_proto();
            display_config(config);
        }
        else {
            cout << config_parser->get_error_message() << endl;
        }
    }
}


bool get_plugin_parser(std::string dll_path, std::string dll_name, boost::function<create_plugin_parser> &create) {
    boost::filesystem::path lib_path(dll_path);
    lib_path /= dll_name;

    try {
        create = boost::dll::import_alias<create_plugin_parser>(
            lib_path,
            "create_plugin",
            boost::dll::load_mode::append_decorations
            );
    }
    catch (boost::system::system_error &ec) {
        return false;
    }

    return true;
}

void test_plugin(
        std::string dll_path, 
        std::string dll_name, 
        std::string config_name, 
        boost::function<create_plugin_parser> &plugin_creator, 
        boost::function<create_config_parser> &config_creator, 
        std::map<std::string, boost::shared_ptr<ISniffPlugin> > &plugins
) {
    if (get_plugin_parser(dll_path, dll_name, plugin_creator)) {
        std::string config_full_path = dll_path + "/" + config_name;
        boost::shared_ptr<ISniffPlugin> plugin = plugin_creator(config_full_path, config_creator);
        if (plugin->parse_config()) {
            plugins[plugin->get_name()] = plugin;
        }
        else {
            cout << plugin->get_error_message() << endl;
        }
    }
}

void display_plugin(std::map<std::string, boost::shared_ptr<ISniffPlugin> > &plugins) {
    cout << "=====================" << endl;
    for (auto plugin : plugins) {
        boost::shared_ptr<ISniffPlugin> sniff_plugin = plugin.second;
        cout << plugin.first << " | " << sniff_plugin->get_name() << " port[";
        std::vector<unsigned short> ports = sniff_plugin->get_ports();
        for (auto port : ports) {
            cout << port << ", ";
        }
        cout << "]" << endl;
    }
}

void test_plugin(boost::function<create_config_parser> &config_creator) {

    static boost::function<create_plugin_parser> http_creator;
    std::map<std::string, boost::shared_ptr<ISniffPlugin> > plugins;

    test_plugin(R"(E:\cpp\WinpcapTest\bin\Debug\plugins\SniffPluginHttp)", "SniffPluginHttp", "config.json", http_creator, config_creator, plugins);
    display_plugin(plugins);

    static boost::function<create_plugin_parser> rdp_creator;
    test_plugin(R"(E:\cpp\WinpcapTest\bin\Debug\plugins\SniffPluginRdp)", "SniffPluginRdp", "config.json", rdp_creator, config_creator, plugins);
    display_plugin(plugins);

    static boost::function<create_plugin_parser> ssh_creator;
    test_plugin(R"(E:\cpp\WinpcapTest\bin\Debug\plugins\SniffPluginSsh)", "SniffPluginSsh", "config.json", ssh_creator, config_creator, plugins);
    display_plugin(plugins);

    static boost::function<create_plugin_parser> telnet_creator;
    test_plugin(R"(E:\cpp\WinpcapTest\bin\Debug\plugins\SniffPluginTelnet)", "SniffPluginTelnet", "config.json", telnet_creator, config_creator, plugins);
    display_plugin(plugins);
}

void get_all_plugins(std::string plugin_path, boost::function<create_config_parser> &config_creator, 
            std::map<std::string, boost::shared_ptr<ISniffPlugin> > &plugins, 
            std::vector<boost::function<create_plugin_parser> > &plugin_creators) {
    namespace fs = boost::filesystem;
    fs::path path_1(plugin_path, fs::native);
    fs::directory_iterator iter_end;
    
    if (fs::is_directory(path_1)) {
        fs::directory_iterator tree_1(path_1);
        for (auto path_2 : tree_1) {
            if (fs::is_directory(path_2)) {
                fs::directory_iterator tree_2(path_2);
                
                std::string config_file;
                std::string dll_file;
                for (auto path_3 : tree_2) {
                    if (!fs::is_directory(path_3)) {
                        std::string file_name = path_3.path().string();
                        if (file_name.rfind(".json") != std::string::npos) {
                            config_file = file_name;
                        }
                        else if ((file_name.rfind(".dll") != std::string::npos) || (file_name.rfind(".so") != std::string::npos)) {
                            dll_file = file_name;
                        }
                    }
                }

                try {
                    if ((!dll_file.empty() && (!config_file.empty()))) {
                        boost::function<create_plugin_parser> creator = boost::dll::import_alias<create_plugin_parser>(
                            dll_file
                            , "create_plugin"
                            //,boost::dll::load_mode::append_decorations
                            );
                        plugin_creators.push_back(creator);
                        boost::shared_ptr<ISniffPlugin> plugin = creator(config_file, config_creator);
                        plugin->parse_config();
                        plugins[plugin->get_name()] = plugin;
                    }
                }
                catch (boost::system::system_error &ec) {
                    cout << ec.what() << endl;
                }
            }
        }
    }
}

void test_get_all_plugins(std::string plugin_path, 
            boost::function<create_config_parser> &config_creator, 
            std::map<std::string, boost::shared_ptr<ISniffPlugin> > &plugins,
            std::vector<boost::function<create_plugin_parser> > &plugin_creators
    ) {
    get_all_plugins(plugin_path, config_creator, plugins, plugin_creators);

    display_plugin(plugins);
}

void main() {

    static boost::function<create_config_parser> config_creator;
    test_config(config_creator);

    //static boost::function<create_plugin_parser> http_creator;
    //test_plugin(config_creator);

    std::map<std::string, boost::shared_ptr<ISniffPlugin> > plugins;
    static std::vector<boost::function<create_plugin_parser> > plugin_creators;
    test_get_all_plugins(R"(E:\cpp\WinpcapTest\bin\Debug\plugins)", config_creator, plugins, plugin_creators);
}