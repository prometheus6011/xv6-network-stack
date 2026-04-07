#include "types.h"
#include "defs.h"
#include "spinlock.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "e1000_dev.h"

static volatile uint *regs;
static struct spinlock e1000_lock;

static struct tx_desc tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));
static struct rx_desc rx_ring[RX_RING_SIZE] __attribute__((aligned(16)));

static char *tx_bufs[TX_RING_SIZE];
static char *rx_bufs[RX_RING_SIZE];

static inline uint
e1000_reg_read(int idx)
{
    return regs[idx];
}

static inline void
e1000_reg_write(int idx, uint val)
{
    regs[idx] = val;
}

static void
e1000_init_tx(void)
{
    int i;
    for (i = 0; i < TX_RING_SIZE; i++) {
        tx_ring[i].addr = 0;
        tx_ring[i].length = 0;
        tx_ring[i].cso = 0;
        tx_ring[i].cmd = 0;
        tx_ring[i].status = E1000_TXD_STAT_DD;
        tx_ring[i].css = 0;
        tx_ring[i].special = 0;
        tx_bufs[i] = 0;
    }

    e1000_reg_write(E1000_TDBAL, V2P(tx_ring));
    e1000_reg_write(E1000_TDBAH, 0);
    e1000_reg_write(E1000_TDLEN, sizeof(tx_ring));
    e1000_reg_write(E1000_TDH, 0);
    e1000_reg_write(E1000_TDT, 0);

    e1000_reg_write(E1000_TCTL,
        E1000_TCTL_EN |
        E1000_TCTL_PSP |
        (0x10 << 4) |
        (0x40 << 12));

    e1000_reg_write(E1000_TIPG, 10 | (8 << 10) | (6 << 20));
}

static void
e1000_init_rx(void)
{
    int i;
    for (i = 0; i < RX_RING_SIZE; i++) {
        rx_bufs[i] = kalloc();
        if (rx_bufs[i] == 0) panic("e1000 rx kalloc");

        rx_ring[i].addr = V2P(rx_bufs[i]);
        rx_ring[i].length = 0;
        rx_ring[i].csum = 0;
        rx_ring[i].status = 0;
        rx_ring[i].errors = 0;
        rx_ring[i].special = 0;
    }

    e1000_reg_write(E1000_RDBAL, V2P(rx_ring));
    e1000_reg_write(E1000_RDBAH, 0);
    e1000_reg_write(E1000_RDLEN, sizeof(rx_ring));
    e1000_reg_write(E1000_RDH, 0);
    e1000_reg_write(E1000_RDT, RX_RING_SIZE - 1);

    e1000_reg_write(E1000_RCTL,
        E1000_RCTL_EN |
        E1000_RCTL_BAM |
        E1000_RCTL_SECRC |
        E1000_RCTL_SZ_2048);
}

void
e1000_init(uint bar0, uchar irqline)
{
    regs = (volatile uint*)bar0;
    initlock(&e1000_lock, "e1000");

    e1000_reg_read(E1000_ICR);

    e1000_init_tx();
    e1000_init_rx();

    e1000_reg_read(E1000_ICR);
    e1000_reg_write(E1000_IMS, 
        E1000_ICR_RXT0 | E1000_ICR_RXDMT0 | E1000_ICR_RXO | 
        E1000_ICR_TXDW | E1000_ICR_LSC);
    cprintf("e1000_init: tx/rx rings ready\n");
}

int
e1000_transmit(void* buf, uint len)
{
    acquire(&e1000_lock);

    uint t = e1000_reg_read(E1000_TDT);
    if ((tx_ring[t].status & E1000_TXD_STAT_DD) == 0) {
        release(&e1000_lock);
        return -1;
    }

    if (tx_bufs[t] != 0) {
        kfree(tx_bufs[t]);
    }

    tx_bufs[t] = (char*)buf;
    tx_ring[t].addr = V2P(buf);
    tx_ring[t].length = len;
    tx_ring[t].cso = 0;
    tx_ring[t].cmd = E1000_TXD_CMD_EOP | E1000_TXD_CMD_RS;
    tx_ring[t].status = 0;
    tx_ring[t].css = 0;
    tx_ring[t].special = 0;

    e1000_reg_write(E1000_TDT, (t + 1) % TX_RING_SIZE);

    release(&e1000_lock);
    return 0;
}

void
e1000_recv_locked(void)
{
    for (;;) {
        uint r = (e1000_reg_read(E1000_RDT) + 1) % RX_RING_SIZE;

        if ((rx_ring[r].status & E1000_RXD_STAT_DD) == 0) {
            break;
        }

        char *buf = rx_bufs[r];
        uint len = rx_ring[r].length;

        cprintf("e1000_recv: packet len=%d idx=%d\n", len, r);

        rx_bufs[r] = kalloc();
        if (rx_bufs[r] == 0) {
            panic("e1000 recv refill");
        }

        rx_ring[r].addr = V2P(rx_bufs[r]);
        rx_ring[r].length = 0;
        rx_ring[r].csum = 0;
        rx_ring[r].status = 0;
        rx_ring[r].errors = 0;
        rx_ring[r].special = 0;

        kfree(buf);
        e1000_reg_write(E1000_RDT, r);
    }
}

void 
e1000_recv(void)
{
    acquire(&e1000_lock);
    e1000_recv_locked();
    release(&e1000_lock);
}

void
e1000_intr(void)
{
    acquire(&e1000_lock);
    uint icr = e1000_reg_read(E1000_ICR);
    if (icr & (E1000_ICR_RXT0 | E1000_ICR_RXDMT0 | E1000_ICR_RXO)) {
        e1000_recv_locked();
    }
    release(&e1000_lock);
}