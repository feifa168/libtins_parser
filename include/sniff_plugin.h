#ifndef SNIFF_PLUGIN_H
#define SNIFF_PLUGIN_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "sniff_cross_platform.h"
#include "sniff_config_parse .h"

#ifdef SNIFFPLUGINHTTP_EXPORTS
#define SNIFF_PLUGIN_EXPORT_API DLL_EXPORT
#else
#define SNIFF_PLUGIN_EXPORT_API DLL_IMPORT
#endif


class ISniffPlugin {
public:
    VIRTUAL_METHOD void CALL_METHOD do_parse() PURE_VIRTUAL;
    VIRTUAL_METHOD bool CALL_METHOD parse_config() PURE_VIRTUAL;
    VIRTUAL_METHOD std::string CALL_METHOD get_name() PURE_VIRTUAL;
    VIRTUAL_METHOD std::vector<unsigned short> CALL_METHOD get_ports() PURE_VIRTUAL;

    VIRTUAL_METHOD const char*  CALL_METHOD get_error_message() PURE_VIRTUAL;
};

typedef boost::shared_ptr<ISniffPlugin>(create_plugin_parser)(std::string config, boost::function<create_config_parser> &parse_dll);

#ifdef __cplusplus
extern "C" {
#endif

    //SNIFF_PLUGIN_EXPORT_API boost::shared_ptr<ISniffPlugin> CALL_METHOD create_sniff_plugin(std::string config, std::string parse_dll);
    //SNIFF_PLUGIN_EXPORT_API void          CALL_METHOD release_sniff_plugin(ISniffPlugin **pp_sniff_plugin);

#ifdef __cplusplus
}
#endif

#endif  // SNIFF_PLUGIN_H
