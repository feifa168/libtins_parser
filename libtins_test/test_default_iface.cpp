#include <tins/tins.h>
#include <cassert>
#include <iostream>
#include <string>

using namespace Tins;

int test_default_iface() {
    // Construct a packet
    EthernetII packet = EthernetII() / IP() / TCP() / RawPDU("hello");
    
    // Now serialize it. This is a std::vector<uint8_t>.
    PDU::serialization_type buffer = packet.serialize();

    // We'll use the default interface(default gateway)
    NetworkInterface iface = NetworkInterface::default_interface();

    /* Retrieve this structure which holds the interface's IP,
    * broadcast, hardware address and the network mask.
    */
    NetworkInterface::Info info = iface.addresses();

    /* Create an Ethernet II PDU which will be sent to
    * 77:22:33:11:ad:ad using the default interface's hardware
    * address as the sender.
    */
    EthernetII eth("77:22:33:11:ad:ad", info.hw_addr);

    /* Create an IP PDU, with 192.168.0.1 as the destination address
    * and the default interface's IP address as the sender.
    */
    eth /= IP("192.168.0.1", info.ip_addr);

    /* Create a TCP PDU using 13 as the destination port, and 15
    * as the source port.
    */
    eth /= TCP(13, 15);

    /* Create a RawPDU containing the string "I'm a payload!".
    */
    eth /= RawPDU("I'm a payload!");

    // The actual sender
    PacketSender sender;

    // Send the packet through the default interface
    sender.send(eth, iface);
    
    return 0;
}