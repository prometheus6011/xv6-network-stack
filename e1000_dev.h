#ifndef E1000_DEV_H
#define E1000_DEV_H

#define E1000_CTRL    (0x00000/4)
#define E1000_STATUS  (0x00008/4)
#define E1000_ICR     (0x000C0/4)
#define E1000_IMS     (0x000D0/4)

#define E1000_RCTL   (0x00100/4)
#define E1000_TCTL   (0x00400/4)
#define E1000_TIPG   (0x00410/4)

#define E1000_RDBAL  (0x02800/4)
#define E1000_RDBAH  (0x02804/4)
#define E1000_RDLEN  (0x02808/4)
#define E1000_RDH    (0x02810/4)
#define E1000_RDT    (0x02818/4)

#define E1000_TDBAL  (0x03800/4)
#define E1000_TDBAH  (0x03804/4)
#define E1000_TDLEN  (0x03808/4)
#define E1000_TDH    (0x03810/4)
#define E1000_TDT    (0x03818/4)

#define E1000_TCTL_EN    0x00000002
#define E1000_TCTL_PSP   0x00000008
#define E1000_TCTL_CT    0x00000ff0
#define E1000_TCTL_COLD  0x003ff000

#define E1000_RCTL_EN     0x00000002
#define E1000_RCTL_BAM    0x00008000
#define E1000_RCTL_SECRC  0x04000000
#define E1000_RCTL_SZ_2048 0x00000000

#define E1000_TXD_CMD_EOP  0x01
#define E1000_TXD_CMD_RS   0x08
#define E1000_TXD_STAT_DD  0x01

#define E1000_RXD_STAT_DD  0x01

#define TX_RING_SIZE 16
#define RX_RING_SIZE 16
#define E1000_PKT_BUF_SZ 2048

#define E1000_ICR_TXDW   0x00000001
#define E1000_ICR_LSC    0x00000004
#define E1000_ICR_RXDMT0 0x00000010
#define E1000_ICR_RXO    0x00000040
#define E1000_ICR_RXT0   0x00000080

#define E1000_IMS_TXDW   0x00000001
#define E1000_IMS_LSC    0x00000004
#define E1000_IMS_RXDMT0 0x00000010
#define E1000_IMS_RXO    0x00000040
#define E1000_IMS_RXT0   0x00000080

struct tx_desc {
    unsigned long long addr;
    ushort length;
    uchar cso;
    uchar cmd;
    uchar status;
    uchar css;
    ushort special;
} __attribute__((packed));

struct rx_desc {
    unsigned long long addr;
    ushort length;
    ushort csum;
    uchar status;
    uchar errors;
    ushort special;
} __attribute__((packed));

#endif
