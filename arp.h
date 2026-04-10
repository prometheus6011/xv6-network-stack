#ifndef XV6_ARP_H
#define XV6_ARP_H

#include "types.h"
#include "net.h"

#define IP_ADDR_LEN   4

struct arp_frame {
    ushort hardware_type;
    ushort prot_type;
    uchar hard_size;
    uchar prot_size;
    ushort op;
    uchar sender_hw_address[ETH_ADDR_LEN];
    uchar sender_ip_addr[IP_ADDR_LEN];
    uchar target_hw_address[ETH_ADDR_LEN];
    uchar target_ip_addr[IP_ADDR_LEN];
} __attribute__((packed));

struct arp_cache_entry {
    uchar ip_addr[IP_ADDR_LEN];
    uchar hw_address[ETH_ADDR_LEN];
    uchar flags;
    uchar valid;
};

struct arp_cache_list_node {
    struct arp_cache_entry entry;
    struct arp_cache_list_node* next; 
};

struct arp_cache_list {
    struct arp_cache_list_node* front;
    struct arp_cache_list_node* back;
};

struct arp_cache_list arp_cache;
extern uchar local_ip[IP_ADDR_LEN];
extern uchar local_mac[ETH_ADDR_LEN];

void arpinit(void);
void arprx(void*, uint, struct ethhdr*);

#endif  // XV6_ARP_H