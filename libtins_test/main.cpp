#include <string>
#include <iostream>

#include <tins/tins.h>

using std::string;
using Tins::NetworkInterface;

int test_interfaces_info();
int test_defragmenter(std::string &srcfile, std::string &dstfile);
void test_defragmenter_stream(std::string iface, std::string flt, std::string dstfile);
int test_default_iface();
int test_capture();

int test_return_temp_object();
void test_rvalue_string();
int test_lock_free();
int test_custom_capture();

int test_tcp_reassemble(string iface, string port);

void test_ip_defragment_and_tcp_reassmble(std::string &device, std::string filter);

int main(int argc, char **argvs) {
    test_interfaces_info();

    NetworkInterface iface = NetworkInterface::default_interface();
    string sface = iface.name();

    std::string filter = "tcp por 80";
    if (argc == 2) {
        filter = argvs[1];
    }

    std::cout << sface << " with filter " << filter << std::endl;
    //test_defragmenter("icmp_frag.pcap", "icmp_frag_save.pcap");
    //test_defragmenter("icmp_normal.pcap", "icmp_normal_save.pcap");
    //test_defragmenter("icmp50_icmp4000.pcap", "icmp50_icmp4000_save.pcap");
    //test_default_iface();
    //test_defragmenter_stream("{78B23CEC-A149-4394-A066-C83C054E8475}", "port 80", "port80_capture.pcap");

    //test_defragmenter_stream(sface, "icmp", "icmp_capture.pcap");

    //test_capture();
    
    //test_custom_capture();

    //test_return_temp_object();
    //test_rvalue_string();
    //test_lock_free();

    //test_tcp_reassemble(sface, "80");
    test_ip_defragment_and_tcp_reassmble(sface, filter);

    return 0;
}