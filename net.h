#ifndef XV6_NET_H
#define XV6_NET_H

#include "types.h"

#define ETH_ADDR_LEN 6
#define ETH_HDR_LEN  14
#define ETH_TYPE_IP  0x0800
#define ETH_TYPE_ARP 0x0806

#define NET_MTU       1500
#define NET_MAX_FRAME 1518

struct ethhdr {
    uchar dst[ETH_ADDR_LEN];
    uchar src[ETH_ADDR_LEN];
    ushort type;
};

struct net_stats {
    uint rx_packets;
    uint rx_drops;
    uint tx_packets;
    uint tx_drops;
};

void netinit(void);
void netrx(void*, uint);
void nettx(void*, uint);

const struct net_stats *netstats(void);

#endif  // XV6_NET_H