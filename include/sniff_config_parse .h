#ifndef SNIFF_PROTO_SNIFF_CONFIG_PARSE_H
#define SNIFF_PROTO_SNIFF_CONFIG_PARSE_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "sniff_cross_platform.h"

#ifdef SNIFF_CONFIG_PARSE_EXPORTS
#define SNIFF_CONFIG_PARSE_EXPORTS_API DLL_EXPORT
#else
#define SNIFF_CONFIG_PARSE_EXPORTS_API DLL_IMPORT
#endif

struct SniffProtoConfig {
    std::string name;
    std::vector<unsigned short> ports;
};

class ISniffConfig
{
public:
    VIRTUAL_METHOD bool  CALL_METHOD parse() PURE_VIRTUAL;
    VIRTUAL_METHOD const SniffProtoConfig CALL_METHOD get_sniff_proto() PURE_VIRTUAL;
    VIRTUAL_METHOD const char*  CALL_METHOD get_error_message() PURE_VIRTUAL;
};


typedef boost::shared_ptr<ISniffConfig>(create_config_parser)(std::string);

#ifdef __cplusplus
extern "C" {
#endif

    //SNIFF_CONFIG_PARSE_EXPORTS_API ISniffConfig* CALL_METHOD get_sniff_config(std::string config_file);
    //SNIFF_CONFIG_PARSE_EXPORTS_API void CALL_METHOD release_sniff_config(ISniffConfig** pp_config);

#ifdef __cplusplus
}
#endif

#endif  // SNIFF_PROTO_SNIFF_CONFIG_PARSE_H