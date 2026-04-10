# xv6-network-stack

the following changes have to be made to the following files in order for this program to work
this repository is forked from the https://github.com/mit-pdos/xv6-public
my build process on Macbook M-series chips using qemu
```
make clean TOOLPREFIX=i686-elf-
make TOOLPREFIX=i686-elf-
make qemu TOOLPREFIX=i686-elf-
```

Makefile
```
pci.o\
e1000.o\
net.o\
arp.o\

CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -fno-omit-frame-pointer  \\ line 80
QEMUEXTRA += -nic user,model=e1000   \\ line 224
```

main.c
```c
pciinit();       // pci line 34
netinit();       // net
```

defs.c
```c
// pci.c
void            pciinit(void);
extern int      e1000_irq;
void            e1000_intr(void);

// e1000.c
void            e1000_init(uint, uchar);
int             e1000_transmit(void*, uint);
void            e1000_recv(void);

// net.c
void            netinit(void);

// arp.c
void            arpinit(void);
```

x86.h
```c
static inline uint 
inl(ushort port)
{
  uint data;
  asm volatile("inl %1, %0" : "=a"(data) : "d"(port));
  return data;
}

static inline void 
outl(ushort port, uint data)
{
  asm volatile("outl %0, %1" : : "a"(data), "d"(port));
}
```

trap.c
```c
// after default case in void trap();
if (0 <= e1000_irq && e1000_irq < 32 && tf->trapno == T_IRQ0 + e1000_irq) {
  e1000_intr();
  lapiceoi();
  break;
}
```

the rest of the files can be copied as-is

author: Mahi Nuthanapati
