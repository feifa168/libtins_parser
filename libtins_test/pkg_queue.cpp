
#include <boost/threadpool.hpp>
#include <boost/lockfree/queue.hpp>

// struct pcap_info {
//     pcap_pkthdr head;
//     u_char      *pdata;
// };
// 
// boost::lockfree::queue<pcap_info, fixed_sized<false> > pcap_pkt_que(0);
