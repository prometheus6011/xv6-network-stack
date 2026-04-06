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
pci.o\   \\ line 30
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -fno-omit-frame-pointer  \\ line 80
QEMUEXTRA += -nic user,model=e1000   \\ line 224
```

main.c
```c
pciinit();       // pci line 34
```

defs.c
```c
// pci.c
void            pciinit(void);
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

the rest of the files can be copied as-is

author: Mahi Nuthanapati
