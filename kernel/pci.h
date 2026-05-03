#ifndef XV6_PCI_H
#define XV6_PCI_H

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"

struct pci_device_id {
    uint32 vendor, device;
    uint32 subvendor, subdevice;
    uint32 class, class_mask;
    uint64 driver_data;
    uint32 override_only;
};

void pciinit();

#endif
