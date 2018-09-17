#include "tins/pdu.h"
#include "tins/config.h"
#include "tins/ethernetII.h"
#include "tins/ip.h"
#include "tins/tcp.h"
#include "tins/sniffer.h"
#include "tins/network_interface.h"
#include "tins/ip_reassembler.h"

#include <boost/lockfree/queue.hpp>
#include <iostream>


using namespace Tins;
using namespace std;

using namespace boost;
using namespace boost::lockfree;


//boost::lockfree::queue<tagMyStruct, fixed_sized<false> > que(0);

int test_capture2() {
    return 0;
}

int test_capture() {
    SnifferConfiguration config;
    config.set_filter("icmp");
    NetworkInterface iface = NetworkInterface::default_interface();
    Sniffer sn(iface.name(), config);

    IPv4Reassembler rebuildv4;

    //typedef int(*PcapSniffingMethod)(pcap_t*, int, pcap_handler, u_char*)
    sn.set_pcap_sniffing_method([](pcap_t * p, int cnt, pcap_handler callback, u_char * user) {        
        int rt = pcap_loop(p, cnt, callback, user);
        cout << "get package " << endl;
        return rt;
    });

    sn.sniff_loop([&](PDU &packet) {
        try
        {
            IPv4Reassembler::PacketStatus pkg_status = rebuildv4.process(packet);
            if (pkg_status == IPv4Reassembler::PacketStatus::REASSEMBLED) {
                PDU::serialization_type buffer = packet.serialize();    // 重组需要用serialize
            }
            
            if (pkg_status != IPv4Reassembler::PacketStatus::FRAGMENTED) {
                EthernetII  &eth = packet.rfind_pdu<Tins::EthernetII>();
                IP          &ip  = packet.rfind_pdu<Tins::IP>();
                //TCP         &tcp = packet.rfind_pdu<Tins::TCP>();

                cout << "  ip flags " << ip.flags() << ", offset " << ip.frag_off() << ", total len " << ip.tot_len() << " ip inner pud len " << ip.inner_pdu()->size() << endl;
            }

            return true;
        }
        catch (const Tins::pdu_not_found &e) {
            cout << e.what() << endl;
        }
        catch (const std::exception &e)
        {
            cout << e.what() << endl;
        }

        return false;
    });
    return 0;
}