/*
* Copyright (c) 2017, Matias Fontanini
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
* * Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above
*   copyright notice, this list of conditions and the following disclaimer
*   in the documentation and/or other materials provided with the
*   distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include <iostream>
#include <string>
#include <stdexcept>
#include "tins/ip.h"
#include "tins/ip_reassembler.h"
#include "tins/sniffer.h"
#include "tins/packet_writer.h"
#include "tins/pdu.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::exception;

using Tins::IPv4Reassembler;
using Tins::IP;
using Tins::Packet;
using Tins::FileSniffer;
using Tins::PacketWriter;
using Tins::DataLinkType;
using Tins::Sniffer;
using Tins::SnifferConfiguration;
using Tins::PDU;

// This example reads packets from a pcap file and writes them to a new file.
// If any IPv4 fragmented packets are found in the input file, then they will
// be reassembled before writing them, so instead of the individual fragments
// it will write the whole packet.

class Defragmenter {
public:
    // Construct the sniffer and the packet writer using the sniffer's
    // data link type
    Defragmenter(const string& input_file, const string& output_file)
        : sniffer_(input_file),
        writer_(output_file, (PacketWriter::LinkType)sniffer_.link_type()),
        total_reassembled_(0) {

    }

    void run() {
        Packet packet;
        // Read packets and keep going until there's no more packets to read
        while (packet = sniffer_.next_packet()) {
            // Try to reassemble the packet
            IPv4Reassembler::PacketStatus status = reassembler_.process(*packet.pdu());

            // If we did reassemble it, increase this counter
            if (status == IPv4Reassembler::REASSEMBLED) {
                total_reassembled_++;
            }

            // Regardless, we'll write it into the output file unless it's fragmented
            // (and not yet reassembled) 
            if (status != IPv4Reassembler::FRAGMENTED) {
                writer_.write(packet);
            }
        }
    }

    uint64_t total_packets_reassembled() const {
        return total_reassembled_;
    }
private:
    FileSniffer sniffer_;
    IPv4Reassembler reassembler_;
    PacketWriter writer_;
    uint64_t total_reassembled_;
};


class DefragmenterStream {
public:
    // Construct the sniffer and the packet writer using the sniffer's
    // data link type
    DefragmenterStream(const string &iface, const string &flt, const string &output_file)
        : sniffer_(iface, 65536, false, flt),
        writer_(output_file, (PacketWriter::LinkType)sniffer_.link_type()),
        total_reassembled_(0) 
    {
    }

    void run() {
        Packet packet;
        int i = 0;
        // Read packets and keep going until there's no more packets to read
        while (packet = sniffer_.next_packet()) {
            PDU *pdu = packet.pdu();
            IP *pip = pdu->find_pdu<IP>();
            // Try to reassemble the packet
            IPv4Reassembler::PacketStatus status = reassembler_.process(*packet.pdu());

            // If we did reassemble it, increase this counter
            if (status == IPv4Reassembler::REASSEMBLED) {
                total_reassembled_++;
            }

            // Regardless, we'll write it into the output file unless it's fragmented
            // (and not yet reassembled) 
            if (status != IPv4Reassembler::FRAGMENTED) {
                writer_.write(packet);
            }
            if (++i > 6)
                break;
        }
    }

    uint64_t total_packets_reassembled() const {
        return total_reassembled_;
    }
private:
    Sniffer sniffer_;
    IPv4Reassembler reassembler_;
    PacketWriter writer_;
    uint64_t total_reassembled_;
};

void test_defragmenter_stream(std::string iface, std::string flt, std::string dstfile) {
    try
    {
        DefragmenterStream deStream(iface, flt, dstfile);
        deStream.run();
        cout << "Done" << endl;
        cout << "Reassembled: " << deStream.total_packets_reassembled()
            << " packet(s)" << endl;
    }
    catch (exception& ex) {
        cerr << "Error: " << ex.what() << endl;
    }
}
int test_defragmenter(std::string &srcfile, std::string &dstfile) {
    try {
        // Build the defragmented
        Defragmenter defragmenter(srcfile, dstfile);
        cout << "Processing " << srcfile << endl;
        cout << "Writing results to " << dstfile << endl;

        // Run!
        defragmenter.run();
        cout << "Done" << endl;
        cout << "Reassembled: " << defragmenter.total_packets_reassembled()
            << " packet(s)" << endl;
    }
    catch (exception& ex) {
        cerr << "Error: " << ex.what() << endl;
    }

    return 0;
}

