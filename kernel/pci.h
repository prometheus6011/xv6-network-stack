#ifndef XV6_PCI_H
#define XV6_PCI_H

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"

#define INTEL_VENDOR_ID 0x8086
#define INTEL_E1000_NIC_ID 0x100E

struct pci_device_id {
    uint16 bus;
    uint8 device;
    uint8 function;
    uint16 vendor_id;
    uint16 device_id;
    // uint8 class_code;
    // uint8 subclass;
    // uint8 prog_if;
    uint32 bar[6];
};

struct pci_device_id pci_devs[32];
extern uint8 pci_devs_ptr;

void pciinit();

#endif
