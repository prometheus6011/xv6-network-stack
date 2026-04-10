#include "types.h"
#include "defs.h"
#include "net.h"

static struct net_stats stats;

static int
valid_frame(void* buf, uint len)
{
    if (buf == 0)
        return 0;
    if (len < ETH_HDR_LEN || len > NET_MAX_FRAME)
        return 0;
    return 1;
}

static struct ethhdr*
parse_eth(void* buf, uint len)
{
    if (!valid_frame(buf, len))
        return 0;
    return (struct ethhdr*)buf;
}

static void
handle_arp(void* buf, uint len, struct ethhdr* ethhdr)
{
    arprx(buf, len, ethhdr);
}

static void
handle_ip(void* buf, uint len, struct ethhdr* ethhdr)
{

}

void
netinit(void)
{
    memset(&stats, 0, sizeof(stats));
    arpinit();
}

void
netrx(void* buf, uint len)
{
    struct ethhdr *eh;
    ushort type;

    eh = parse_eth(buf, len);
    if (eh == 0) {
        stats.rx_drops++;
        return;
    }

    stats.rx_packets++;
    type = ((eh->type & 0x00FF) << 8) | ((eh->type & 0xFF00) >> 8);


    switch (type) {
        case ETH_TYPE_ARP:    handle_arp(buf, len, eh);
                              break;
        case ETH_TYPE_IP:     handle_ip(buf, len, eh);
                              break;
        default:              break;
    }
}

void
nettx(void* buf, uint len)
{
    if (!valid_frame(buf, len)) {
        stats.tx_drops++;
        if (buf != 0)
            kfree((char*)buf);
        return;
    }

    if (e1000_transmit(buf, len) < 0) {
        stats.tx_drops++;
        kfree((char*)buf);
        return;
    }

    stats.tx_packets++;
}

const struct net_stats*
netstats(void)
{
    return &stats;
}