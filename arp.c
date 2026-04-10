#include "arp.h"

static struct arp_cache_list arp_cache;
uchar local_ip[IP_ADDR_LEN];
uchar local_mac[ETH_ADDR_LEN];

void
arpinit(void)
{
    arp_cache.front = 0;
    arp_cache.back = 0;
}

void
arprx(void* buf, uint len, struct ethhdr* eh)
{
    
}