//#define _XKEYCHECK_H
#define HAVE_REMOTE
#include "pcap.h"
#include "remote-ext.h "
#include<winsock2.h>

#include <boost/threadpool.hpp>
#include <boost/lockfree/queue.hpp>

#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"packet.lib")
#pragma comment(lib,"ws2_32.lib")


void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content);
void ip_protool_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content);
void tcp_protool_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content);
void udp_protool_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content);

//��̫��Э��ͷ
struct ether_header
{
    u_int8_t ether_dhost[6]; //Ŀ��Mac��ַ
    u_int8_t ether_shost[6]; //ԴMac��ַ
    u_int16_t ether_type;    //Э������
};

//IPv4Э��ͷ
struct ip_header
{
#if defined(WORDS_BIENDIAN)
    u_int8_t    ip_version : 4, ip_header_length : 4;
#else
    u_int8_t    ip_header_length : 4, ip_version : 4;
#endif
    u_int8_t    ip_tos;
    u_int16_t   ip_length;
    u_int16_t   ip_id;
    u_int16_t   ip_off;
    u_int8_t    ip_ttl;
    u_int8_t    ip_protocol;
    u_int16_t   ip_checksum;
    struct in_addr ip_souce_address;
    struct in_addr ip_destination_address;
};

//UDPЭ��ͷ
struct udphdr
{
    u_int16_t source_port; /*Դ��ַ�˿�*/
    u_int16_t dest_port;    /*Ŀ�ĵ�ַ�˿�*/
    u_int16_t len;     /*UDP����*/
    u_int16_t check;   /*UDPУ���*/
};

//TCPЭ��ͷ
#define __LITTLE_ENDIAN_BITFIELD
struct tcphdr
{
    u_int16_t   source_port;         /*Դ��ַ�˿�*/
    u_int16_t   dest_port;           /*Ŀ�ĵ�ַ�˿�*/
    u_int32_t   seq;            /*���к�*/
    u_int32_t   ack_seq;        /*ȷ�����к�*/
#if defined(__LITTLE_ENDIAN_BITFIELD)
    u_int16_t res1 : 4,   /*����*/
        doff : 4,				/*ƫ��*/
        fin : 1,              /*�ر����ӱ�־*/
        syn : 1,              /*�������ӱ�־*/
        rst : 1,              /*�������ӱ�־*/
        psh : 1,              /*���շ����콫���ݷŵ�Ӧ�ò��־*/
        ack : 1,              /*ȷ����ű�־*/
        urg : 1,              /*����ָ���־*/
        ece : 1,              /*ӵ����־λ*/
        cwr : 1;              /*ӵ����־λ*/
#elif defined(__BIG_ENDIAN_BITFIELD)
    u_int16_t doff : 4,   /*ƫ��*/
        res1 : 4,             /*����*/
        cwr : 1,              /*ӵ����־λ*/
        ece : 1,              /*ӵ����־λ*/
        urg : 1,              /*����ָ���־*/
        ack : 1,              /*ȷ����ű�־*/
        psh : 1,              /*���շ����콫���ݷŵ�Ӧ�ò��־*/
        rst : 1,              /*�������ӱ�־*/
        syn : 1,              /*�������ӱ�־*/
        fin : 1;              /*�ر����ӱ�־*/
#else
    u_int16_t	flag;
#endif 
    u_int16_t   window;         /*�������ڴ�С*/
    u_int16_t   check;          /*У���*/
    u_int16_t   urg_ptr;        /*�����ֶ�ָ��*/
};

//��̫��Э�����
void ethernet_protocol_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content)
{
    u_short ethernet_type;
    struct ether_header *ethernet_protocol;
    u_char *mac_string;
    static int packet_number = 1;
    printf("----------------------------------------------\n");
    printf("�����%d���������ݰ�\n", packet_number);
    printf("����ʱ��:%d\n", packet_header->ts.tv_sec);
    printf("���ݰ�����:%d\n", packet_header->len);
    printf("---------��̫��Э��---------\n");
    ethernet_protocol = (struct ether_header*)packet_content;//������ݰ�����
    ethernet_type = ntohs(ethernet_protocol->ether_type);//�����̫������
    printf("��̫������:%04x\n", ethernet_type);
    switch (ethernet_type)
    {
    case 0x0800: printf("�ϲ�Э����IPv4Э��\n"); break;
    case 0x0806: printf("�ϲ�Э����ARPЭ��\n"); break;
    case 0x8035: printf("�ϲ�Э����RARPЭ��\n"); break;
    case 0x814C: printf("�ϲ�Э���Ǽ��������Э��SNMP\n"); break;
    case 0x8137: printf("�ϲ�Э������������������IPX��Internet Packet Exchange��\n"); break;
    case 0x86DD: printf("�ϲ�Э����IPv6Э��\n"); break;
    case 0x880B: printf("�ϲ�Э���ǵ�Ե�Э�飨PPP��Point-to-Point Protocol��\n"); break;
    default:break;
    }

    mac_string = ethernet_protocol->ether_shost;
    printf("MAC֡Դ��ַ:%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    mac_string = ethernet_protocol->ether_dhost;
    printf("MAC֡Ŀ�ĵ�ַ:%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
    if (ethernet_type == 0x0800)//��������IPЭ��
    {
        ip_protool_packet_callback(argument, packet_header, packet_content + sizeof(ether_header));
    }
    printf("----------------------------------------------\n");
    packet_number++;
}

//IPЭ�����
void ip_protool_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content)
{
    struct ip_header *ip_protocol;
    u_int header_length = 0;
    u_int offset;
    u_char tos;
    u_int16_t checksum;

    ip_protocol = (struct ip_header *)packet_content;
    checksum = ntohs(ip_protocol->ip_checksum);
    tos = ip_protocol->ip_tos;
    offset = ntohs(ip_protocol->ip_off);
    printf("---------IPЭ��---------\n");
    printf("�汾��:%d\n", ip_protocol->ip_version);
    printf("�ײ�����:%d\n", header_length);
    printf("��������:%d\n", tos);
    printf("�ܳ���:%d\n", ntohs(ip_protocol->ip_length));
    printf("��ʶ:%d\n", ntohs(ip_protocol->ip_id));
    printf("ƫ��:%d\n", (offset & 0x1fff) * 8);
    printf("����ʱ��:%d\n", ip_protocol->ip_ttl);
    printf("Э������:%d\n", ip_protocol->ip_protocol);

    printf("�����:%d\n", checksum);
    printf("ԴIP��ַ:%s\n", inet_ntoa(ip_protocol->ip_souce_address));
    printf("Ŀ�ĵ�ַ:%s\n", inet_ntoa(ip_protocol->ip_destination_address));

    switch (ip_protocol->ip_protocol)
    {
    case 1: printf("�ϲ�Э����ICMPЭ��\n"); break;
    case 2: printf("�ϲ�Э����IGMPЭ��\n"); break;
    case 6:
    {
        //printf("�ϲ�Э����TCPЭ��\n");
        tcp_protool_packet_callback(argument, packet_header, packet_content + sizeof(ip_header));
    }
    break;

    case 17:
    {
        //printf("�ϲ�Э����UDPЭ��\n");
        udp_protool_packet_callback(argument, packet_header, packet_content + sizeof(ip_header));
    }
    break;
    default:break;
    }

}

//TCPЭ�����
void tcp_protool_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content)
{
    struct tcphdr *tcp_protocol;
    u_int header_length = 0;
    u_int offset;
    u_char tos;
    u_int16_t checksum;

    tcp_protocol = (struct tcphdr *) packet_content;
    checksum = ntohs(tcp_protocol->check);

    printf("---------TCPЭ��---------\n");
    printf("Դ�˿�:%d\n", ntohs(tcp_protocol->source_port));
    printf("Ŀ�Ķ˿�:%d\n", ntohs(tcp_protocol->dest_port));
    printf("SEQ:%d\n", ntohl(tcp_protocol->seq));
    printf("ACK SEQ:%d\n", ntohl(tcp_protocol->ack_seq));
    printf("check:%d\n", checksum);


    if (ntohs(tcp_protocol->source_port) == 80 || ntohs(tcp_protocol->dest_port) == 80)
    {
        //httpЭ��
        printf("http data:\n%s\n", packet_content + sizeof(tcphdr));
    }

}

//UDPЭ�����
void udp_protool_packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header, const u_char* packet_content)
{
    struct udphdr *udp_protocol;
    u_int header_length = 0;
    u_int16_t checksum;

    udp_protocol = (struct udphdr *) packet_content;
    checksum = ntohs(udp_protocol->check);

    u_int16_t source_port; /*Դ��ַ�˿�*/
    u_int16_t dest_port;    /*Ŀ�ĵ�ַ�˿�*/
    u_int16_t len;     /*UDP����*/
    u_int16_t check;   /*UDPУ���*/

    printf("---------UDPЭ��---------\n");
    printf("Դ�˿�:%d\n", udp_protocol->source_port);
    printf("Ŀ�Ķ˿�:%d\n", udp_protocol->dest_port);
    printf("len:%d\n", udp_protocol->len);
    printf("check:%d\n", checksum);

}

int main()
{
    //pcap_t* pcap_handle; //winpcap���
    char error_content[PCAP_ERRBUF_SIZE]; //�洢������Ϣ
    bpf_u_int32 net_mask = 0; //�����ַ
    bpf_u_int32 net_ip = 0;  //�����ַ
    char *net_interface;  //����ӿ�
    struct bpf_program bpf_filter;  //BPF���˹���
    char bpf_filter_string[] = "ip"; //���˹����ַ�����ֻ����IPv4�����ݰ�

    pcap_if_t * allAdapters;//�������б�
    pcap_if_t * adapter;
    pcap_t           * adapterHandle;//���������

    char errorBuffer[PCAP_ERRBUF_SIZE];//������Ϣ������
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL,
        &allAdapters, errorBuffer) == -1)
    {	//�����������ӵ���������������
        fprintf(stderr, "Error in pcap_findalldevs_ex function: %s\n", errorBuffer);
        return -1;
    }
    if (allAdapters == NULL)
    {	//�������κ�������
        printf("\nNo adapters found! Make sure WinPcap is installed.\n");
        return 0;
    }
    int crtAdapter = 0;
    for (adapter = allAdapters; adapter != NULL; adapter = adapter->next)
    {	//����������������Ϣ(���ƺ�������Ϣ)
        printf("\n%d.%s ", ++crtAdapter, adapter->name);
        printf("-- %s\n", adapter->description);
    }
    printf("\n");
    //ѡ��Ҫ�������ݰ���������
    int adapterNumber;
    printf("Enter the adapter number between 1 and %d:", crtAdapter);
    scanf_s("%d", &adapterNumber);
    if (adapterNumber < 1 || adapterNumber > crtAdapter)
    {
        printf("\nAdapter number out of range.\n");
        // �ͷ��������б�
        pcap_freealldevs(allAdapters);
        return -1;
    }
    adapter = allAdapters;
    for (crtAdapter = 0; crtAdapter < adapterNumber - 1; crtAdapter++)
        adapter = adapter->next;
    // ��ָ��������
    adapterHandle = pcap_open(adapter->name, // name of the adapter
        65536,         // portion of the packet to capture
                       // 65536 guarantees that the whole 
                       // packet will be captured
        PCAP_OPENFLAG_PROMISCUOUS, // promiscuous mode
        1000,             // read timeout - 1 millisecond
        NULL,          // authentication on the remote machine
        errorBuffer    // error buffer
    );
    if (adapterHandle == NULL)
    {//ָ����������ʧ��
        fprintf(stderr, "\nUnable to open the adapter\n", adapter->name);
        // �ͷ��������б�
        pcap_freealldevs(allAdapters);
        return -1;
    }

    char errbuf[2048];
    pcap_createsrcstr(NULL, 0, NULL, "80", NULL, errbuf);

    printf("\nCapture session started on  adapter %s\n", adapter->name);
    pcap_freealldevs(allAdapters);//�ͷ��������б�

    pcap_compile(adapterHandle, &bpf_filter, bpf_filter_string, 0, net_ip); //������˹���
    pcap_setfilter(adapterHandle, &bpf_filter);//���ù��˹���
    if (pcap_datalink(adapterHandle) != DLT_EN10MB) //DLT_EN10MB��ʾ��̫��
        return 0;
    pcap_loop(adapterHandle, 65536, ethernet_protocol_packet_callback, NULL); //����65536�����ݰ����з���

    pcap_close(adapterHandle);
    return 0;
}
