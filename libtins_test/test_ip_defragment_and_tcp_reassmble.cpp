#include <iostream>

#include <tins/sniffer.h>
#include <tins/ip_reassembler.h>
#include <tins/tcp_stream.h>
#include <tins/ethernetII.h>
#include <tins/packet_writer.h>
#include <tins/tcp_ip/stream_follower.h>
#include <tins/tcp_ip/stream.h>
#include "tins/ip_address.h"
#include "tins/ipv6_address.h"


#include <sstream>
#include "tins/packet.h"

using std::cout;
using std::cerr;
using std::endl;
using std::bind;
using std::string;
using std::to_string;
using std::ostringstream;
using std::exception;

using Tins::Sniffer;
using Tins::SnifferConfiguration;
using Tins::PDU;
using Tins::TCPIP::StreamFollower;
using Tins::TCPIP::Stream;
using Tins::PacketWriter;
using Tins::IPv4Reassembler;


class TcpTuple {

public:
    // Convert the client endpoint to a readable string
    static string client_endpoint(const Stream& stream) {
        ostringstream output;
        // Use the IPv4 or IPv6 address depending on which protocol the 
        // connection uses
        if (stream.is_v6()) {
            output << stream.client_addr_v6();
        }
        else {
            output << stream.client_addr_v4();
        }
        output << ":" << stream.client_port();
        return output.str();
    }

    // Convert the server endpoint to a readable string
    static string server_endpoint(const Stream& stream) {
        ostringstream output;
        if (stream.is_v6()) {
            output << stream.server_addr_v6();
        }
        else {
            output << stream.server_addr_v4();
        }
        output << ":" << stream.server_port();
        return output.str();
    }

    // Concat both endpoints to get a readable stream identifier
    static string stream_identifier(const Stream& stream) {
        ostringstream output;
        output << client_endpoint(stream) << " - " << server_endpoint(stream);
        return output.str();
    }

    // Whenever there's new client data on the stream, this callback is executed.
    static void on_client_data(Stream& stream) {
        // Construct a string out of the contents of the client's payload
        //string data(stream.client_payload().begin(), stream.client_payload().end());

        // Now print it, prepending some information about the stream
        cout << "client " << client_endpoint(stream) << " >> "
            << server_endpoint(stream) << ": " << endl;// << data << endl;
    }

    // Whenever there's new server data on the stream, this callback is executed.
    // This does the same thing as on_client_data
    static void on_server_data(Stream& stream) {
        //string data(stream.server_payload().begin(), stream.server_payload().end());
        cout << "server " << server_endpoint(stream) << " >> "
            << client_endpoint(stream) << ": " << endl;// << data << endl;
    }

    // When a connection is closed, this callback is executed.
    static void on_connection_closed(Stream& stream) {
        cout << "[+] Connection closed: " << stream_identifier(stream) << endl;
    }

    // When a new connection is captured, this callback will be executed.
    static void on_new_connection(Stream& stream) {
        if (stream.is_partial_stream()) {
            // We found a partial stream. This means this connection/stream had
            // been established before we started capturing traffic.
            //
            // In this case, we need to allow for the stream to catch up, as we
            // may have just captured an out of order packet and if we keep waiting
            // for the holes to be filled, we may end up waiting forever.
            //
            // Calling enable_recovery_mode will skip out of order packets that
            // fall withing the range of the given window size.
            // See Stream::enable_recover_mode for more information
            cout << "[+] New connection partial_stream " << stream_identifier(stream) << endl;

            // Enable recovery mode using a window of 10kb
            stream.enable_recovery_mode(10 * 1024);
        }
        else {
            // Print some information about the new connection
            cout << "[+] New connection " << stream_identifier(stream) << endl;
        }

        // Now configure the callbacks on it.
        // First, we want on_client_data to be called every time there's new client data
        stream.client_data_callback(&TcpTuple::on_client_data);

        // Same thing for server data, but calling on_server_data
        stream.server_data_callback(&TcpTuple::on_server_data);

        // When the connection is closed, call on_connection_closed
        stream.stream_closed_callback(&TcpTuple::on_connection_closed);
    }
};
class ParseIPAndTcp {
public:
    ParseIPAndTcp(std::string &device, Tins::SnifferConfiguration &config, std::string output_file) :
        sniffer_(device, config),
        iface_(device), 
        config_(config),
        writer_(output_file, (PacketWriter::LinkType)sniffer_.link_type())
    {
        init();
    }
    
    void init() {
        follower_.new_stream_callback(&TcpTuple::on_new_connection);
        follower_.stream_termination_callback([](Stream &stream, StreamFollower::TerminationReason tr) {
            cout << "[-] stream is termination, reason is " << tr << "" << TcpTuple::client_endpoint(stream) << endl;
        });

        // Allow following partial TCP streams (e.g. streams that were
        // open before the sniffer started running)
        follower_.follow_partial_streams(true);
    }


    void parse_packet() {
        Tins::Packet packet;

        // Read packets and keep going until there's no more packets to read
        while (packet = sniffer_.next_packet()) {
            Tins::PDU &pdu = *packet.pdu();
            IPv4Reassembler::PacketStatus status = ipv4_reassmbler.process(pdu);

            if (status != Tins::IPv4Reassembler::PacketStatus::FRAGMENTED) {
                if (status == Tins::IPv4Reassembler::PacketStatus::REASSEMBLED) {
                    PDU::serialization_type buffer = pdu.serialize();
                    auto start = buffer.begin();
                    std::shared_ptr<Tins::PDU> ptr_pdu(new Tins::EthernetII(&(*start), buffer.size()));
                    pdu = std::move(*ptr_pdu.get());
                }
                writer_.write(packet);
                follower_.process_packet(packet);
            }
        }
    }
    void sniff_loop() {
        sniffer_.sniff_loop([this](Tins::PDU &pdu) {
            Tins::IPv4Reassembler::PacketStatus status = ipv4_reassmbler.process(pdu);
            if (status != Tins::IPv4Reassembler::PacketStatus::FRAGMENTED) {
                if (status == Tins::IPv4Reassembler::PacketStatus::REASSEMBLED) {
                    PDU::serialization_type buffer = pdu.serialize();
                    auto start = buffer.begin();
                    std::shared_ptr<Tins::PDU> ptr_pdu(new Tins::EthernetII(&(*start), buffer.size()));
                    pdu = std::move(*ptr_pdu.get());
                }
                writer_.write(pdu);
                follower_.process_packet(pdu);
            }
            return true;
        });
    }
    void run() {
        parse_packet();
    }
private:
    Tins::Sniffer sniffer_;
    Tins::SnifferConfiguration config_;
    Tins::IPv4Reassembler ipv4_reassmbler;
    std::string iface_;

    Tins::PacketWriter writer_;
    Tins::TCPIP::StreamFollower follower_;
};

void test_ip_defragment_and_tcp_reassmble(std::string &device, std::string filter) {
    Tins::SnifferConfiguration config;
    config.set_filter(filter);
    ParseIPAndTcp parser(device, config, "icmp_and_http.pcap");
    parser.run();
}
