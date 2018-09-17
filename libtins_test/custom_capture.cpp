#define HAVE_REMOTE
#include "pcap.h"
#include "remote-ext.h "
#include<winsock2.h>

#include <boost/threadpool.hpp>
#include <boost/lockfree/queue.hpp>

#include <iostream>
#include <string>

#include <tins/packet.h>
#include <tins/sniffer.h>
#include <tins/pdu.h>
#include <tins/ethernetII.h>
#include <tins/ip.h>
#include <tins/tcp.h>
#include <tins/udp.h>
#include <tins/ip_reassembler.h>

using namespace std;

using namespace boost;
using namespace boost::threadpool;

using namespace Tins;

#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"packet.lib")
#pragma comment(lib,"ws2_32.lib")


void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content);
void ip_protool_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content);
void tcp_protool_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content);
void udp_protool_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content);

struct pcap_info {
    pcap_pkthdr head;
    const u_char      *pdata;
};

boost::lockfree::queue<pcap_info, boost::lockfree::fixed_sized<false> > pcap_pkt_que(0);

void protocol_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content) {
    pcap_info pkg_info;
    pkg_info.head = *packet_header;
    int caplen = pkg_info.head.caplen;
    pkg_info.pdata = new u_char[caplen];
    memcpy((void *)pkg_info.pdata, packet_content, pkg_info.head.caplen);
    //pkg_info.pdata = packet_content;
    bool bRet = pcap_pkt_que.push(pkg_info);
    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //boost::thread::sleep(boost::get_system_time() + boost::posix_time::millisec(1));
}

#ifdef _WIN32
#define TINS_PREFIX_INTERFACE(x) ("\\Device\\NPF_" + x)
#else // _WIN32
#define TINS_PREFIX_INTERFACE(x) (x)
#endif // _WIN32
void thread_add_pkg(string iface, string filter_string) {
    pcap_t    *adapterHandle;//适配器句柄
    char      errorBuffer[PCAP_ERRBUF_SIZE] = "";

    do
    {
        adapterHandle = pcap_open(iface.c_str(), // name of the adapter
            65536,         // portion of the packet to capture
                           // 65536 guarantees that the whole 
                           // packet will be captured
            PCAP_OPENFLAG_PROMISCUOUS, // promiscuous mode
            1000,             // read timeout - 1 millisecond
            NULL,          // authentication on the remote machine
            errorBuffer    // error buffer
        );
        if (adapterHandle == NULL)
        {//指定适配器打开失败
            fprintf(stderr, "\nUnable to open the adapter\n", iface.c_str());
            break;
        }

        bpf_u_int32 ip, if_mask;
        if (0 != pcap_lookupnet(TINS_PREFIX_INTERFACE(iface).c_str(), &ip, &if_mask, errorBuffer)) {
            break;
        }

        struct bpf_program bpf_filter;  //BPF过滤规则
        if (0 != pcap_compile(adapterHandle, &bpf_filter, filter_string.c_str(), 0, if_mask)) { //编译过滤规则
            break;
        }
        if (0 != pcap_setfilter(adapterHandle, &bpf_filter)) { //设置过滤规则
            break;
        }
        if (pcap_datalink(adapterHandle) != DLT_EN10MB) { //DLT_EN10MB表示以太网
            break;
        }

        pcap_loop(adapterHandle, -1, protocol_packet_callback, NULL); //-1，一直捕获，直到退出

    } while (0);


    pcap_close(adapterHandle);
}

void thread_get_pkg() {
    IPv4Reassembler rebuildv4;
    while (1) {
        pcap_info pkg_info;
        if (pcap_pkt_que.pop(pkg_info)) {
            //cout << "" << pkg_info.head.caplen << "  " << hex << (int)pkg_info.pdata << endl;
            if (pkg_info.pdata) {
                sniff_data data;
                pcap_handler handler = &sniff_loop_eth_handler;
                // keep calling pcap_loop until a well-formed packet is found.
                while (data.pdu == 0 && data.packet_processed) {
                    data.packet_processed = false;
                    handler((u_char *)&data, &pkg_info.head, pkg_info.pdata);
                }
                Packet pk = PtrPacket(data.pdu, data.tv);
                PDU &pdu = *(pk.pdu());

                IPv4Reassembler::PacketStatus pkg_status = rebuildv4.process(pdu);
                if (pkg_status == IPv4Reassembler::PacketStatus::REASSEMBLED) {
                    PDU::serialization_type buffer = pdu.serialize();    // 重组需要用serialize
                }

                if (pkg_status != IPv4Reassembler::PacketStatus::FRAGMENTED) {
                    EthernetII  *peth = pdu.find_pdu<Tins::EthernetII>();
                    Tins::IP    *pip  = pdu.find_pdu<Tins::IP>();
                    Tins::TCP   *ptcp = pdu.find_pdu<Tins::TCP>();
                    Tins::UDP   *pudp = pdu.find_pdu<Tins::UDP>();
                    if (pip) {
                        Tins::IP &ip = *pip;
                        cout << "  ip flags " << ip.flags() << ", offset " << ip.frag_off() << ", total len " << ip.tot_len() << " ip inner pud len " << ip.inner_pdu()->size() << endl;
                    }
                }

                delete[] pkg_info.pdata;
                pkg_info.pdata = NULL;
            }
        }
        boost::thread::sleep(boost::get_system_time() + boost::posix_time::millisec(1));
    }
}

int begin_capture_and_anasy_thread() {

    pcap_if_t * allAdapters;//适配器列表
    pcap_if_t * adapter;
    int err = 0;
    string filter_string = "icmp";

    char errorBuffer[PCAP_ERRBUF_SIZE];//错误信息缓冲区
    do
    {
        if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL,
            &allAdapters, errorBuffer) == -1)
        {	//检索机器连接的所有网络适配器
            fprintf(stderr, "Error in pcap_findalldevs_ex function: %s\n", errorBuffer);
            err = -1;
            break;
        }
        if (allAdapters == NULL)
        {	//不存在任何适配器
            printf("\nNo adapters found! Make sure WinPcap is installed.\n");
            err = -2;
            break;
        }

        int adapter_num = 0;
        for (adapter = allAdapters; adapter != NULL; adapter = adapter->next) {
            adapter_num += 1;
        }

        const int GET_PKG_THRED_NUM = 2;

        pool p(adapter_num + GET_PKG_THRED_NUM);
        for (adapter = allAdapters; adapter != NULL; adapter = adapter->next) {
            p.schedule(std::bind(&thread_add_pkg, adapter->name, filter_string));
        }

        for (int i = 0; i < GET_PKG_THRED_NUM; i++) {
            p.schedule(std::bind(&thread_get_pkg));
        }
        p.wait();
    } while (0);

    return err;
}

int test_custom_capture()
{
    return begin_capture_and_anasy_thread();
}
