## 简介
>抓取网络数据包并做二次分析，使用开源库libtins。
* IDE：VS2017
* 依赖库
    * libtins: [协议分析库](https://github.com/mfontanini/libtins)
    * boost：[boost1.67](https://www.boost.org/)
    * c++11: [c++11标准](https://zh.cppreference.com)

## 模块
* 配置文件解析
* 插件
* 二次分析

----------
### 配置文件解析
> #### 配置文件格式使用json格式，包括协议类型和端口号
```json
{
    "proto": {
        "name": "rdp",
        "port": [3389]
    }
}
```
> #### 解析
使用boost库提供的库boost/property_tree/json_parser.hpp，得到结构体
```c++
struct SniffProtoConfig {
    std::string name;
    std::vector<unsigned short> ports;
};
```
> #### 接口函数
```c++
extern "C" __declspec(dllexport) void* create_parse_config;
```

### 插件
插件为了实现各种协议的支持，可动态扩展，以动态库方式提供，导出一个接口类指针，每个插件对应一个动态库和配置文件
* plugins
    * SniffPluginHttp
        * config.json
        * SniffPluginHttp.dll
    * SniffPluginRdp
        * config.json
        * SniffPluginRdp.dll
    * SniffPluginSsh
        * config.json
        * SniffPluginSsh.dll
    * SniffPluginTelnet
        * config.json
        * SniffPluginTelnet.dll
    * ...


### 插件接口
插件接口是纯虚函数类，提供parse_config用于解析配置文件，do_parse用于解析对应数据包(尚未实现)，所有的插件都继承自该接口
```c++
class ISniffPlugin {
public:
    VIRTUAL_METHOD void CALL_METHOD do_parse() PURE_VIRTUAL;
    VIRTUAL_METHOD bool CALL_METHOD parse_config() PURE_VIRTUAL;
    VIRTUAL_METHOD std::string CALL_METHOD get_name() PURE_VIRTUAL;
    VIRTUAL_METHOD std::vector<unsigned short> CALL_METHOD get_ports() PURE_VIRTUAL;

    VIRTUAL_METHOD const char*  CALL_METHOD get_error_message() PURE_VIRTUAL;
};
```

### 插件API
```c++
BOOST_DLL_ALIAS(sniff_proto::SniffPluginHttp::create_sniff_plugin, create_plugin)
```
***相当于导出C函数，不是C++函数***
```c++
const void * create_parse_config = reinterpret_cast<const void*>(reinterpret_cast<intptr_t>(&sniff_proto::SniffPluginHttp::create_sniff_plugin));
extern "C" __declspec(dllexport) void* create_plugin;
```

### 加载过程
#### 1. 加载配置文件解析模块
* 模块：sniff_config_parse
* 加载方法:
```c++
typedef boost::shared_ptr<ISniffConfig>(create_config_parser)(std::string);
boost::function<create_config_parser> config_creator = boost::dll::import_alias<create_config_parser>(
            dll_path,
            "create_parse_config",
            boost::dll::load_mode::append_decorations
            );  // @1
```
得到动态库句柄config_creator，@1 ***这个操作是因为import_alias会加载动态库，使用的是shared_ptr指针，析构时会释放加载的动态库内存，所以必须使其值生命周期至少持续到插件解析配置文件之后***

#### 2. 加载所有插件
**从plugins目录遍历所有文件**
* 扩展名为.json则使用sniff_config_parse解析，得到对应的协议和端口号
* 扩展名为.dll或.so则加载插件SniffPluginXXX.YY，得到插件实例化指针和动态库句柄
```c++
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
            plugin_creators.push_back(creator); // @2
            boost::shared_ptr<ISniffPlugin> plugin = creator(config_file, config_creator);
            plugin->parse_config();
            plugins[plugin->get_name()] = plugin;
        }
    }
    catch (boost::system::system_error &ec) {
        cout << ec.what() << endl;
    }
```
@2 ***这个操作是因为import_alias会加载动态库，使用的是shared_ptr指针，析构时会释放加载的动态库内存，所以必须使其值生命周期至少持续到插件起作用的整个生命周期之后，也就是整个进程生命周期***
* 解析之后得到一个map，key是协议名，value是ISniffPlugin指针
```c++
std::map<std::string, boost::shared_ptr<ISniffPlugin> > plugins;
```
得到插件的插件的map有另个地方用到
1. 抓包任务启动前构造pcap的抓包过滤条件，最终得到的过滤条件类似这样 tcp port 80 or tcp port 8080 or tcp port 3389 or tcp port 22 or tcp port 23
```c++
    // 构造过滤条件    
    std::string filter;
    int plugin_num = plugins.size() - 1;
    int cur_num = 0;
    char buf[32] = "";
    for (auto plugin : plugins) {
        boost::shared_ptr<ISniffPlugin> sniff_plugin = plugin.second;
        std::vector<unsigned short> ports = sniff_plugin->get_ports();
        for (auto port : ports) {
            if (cur_num > 0) {
                filter += " or ";
            }
            cur_num++;
            itoa(port, buf, 10);
            filter.append("tcp port ").append(buf);
        }
    }
```
2. 抓取到数据包，分析出是对应的插件类型协议时，调用该插件的解析函数
