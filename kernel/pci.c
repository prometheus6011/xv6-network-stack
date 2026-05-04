#include "pci.h"

uint8 pci_devs_ptr = 0;

static uint32
read32(uint64 bus, uint64 device, uint64 function, uint8 offset)
{
    uint64 address = PCI_ECAM
                     + (bus << 20)
                     + (device << 15)
                     + (function << 12)
                     + (offset & ~3);
    uint32 val = *(volatile uint32*)address;
    return val;
}

/*
static void
write32(uint64 bus, uint64 device, uint64 function, uint8 offset, uint32 val)
{
    uint64 address = PCI_ECAM
                     + (bus << 20)
                     + (device << 15)
                     + (function << 12)
                     + (offset & ~3);
    *(volatile uint32*)address = val;
}
*/

static uint16
read16(uint64 bus, uint64 device, uint64 function, uint8 offset)
{
    uint64 address = PCI_ECAM
                     + (bus << 20)
                     + (device << 15)
                     + (function << 12)
                     + (offset & ~3);
    uint32 val = *(volatile uint32*)address;
    return (val >> ((offset & 2) * 8)) & 0xFFFF;
}

static uint8
read8(uint64 bus, uint64 device, uint64 function, uint8 offset)
{
    uint64 address = PCI_ECAM   
                     + (bus << 20)
                     + (device << 15)
                     + (function << 12)
                     + (offset & ~3);
    uint32 val = *(volatile uint32*)address;
    return (val >> ((offset & 3) * 8)) & 0xFF;
}

static uint16
get_vendor_id(uint16 bus, uint8 device)
{
    return read16(bus, device, 0, 0);
}

static uint16
get_device(uint16 bus, uint8 device)
{
    return read16(bus, device, 0, 2);
}

/*
static void
convert_raw_bar(uint32* bar_value)
{
    if (*bar_value & 1) {
        *bar_value = *bar_value & ~0x3;
    } else {
        *bar_value = *bar_value & ~0xF;
    }
}
*/

static uint8
read_device(uint16 bus, uint8 device, struct pci_device_id* pci_dev)
{
    uint16 vendorID = get_vendor_id(bus, device);
    if (vendorID == 0xFFFF) {
        return 0;
    }
    uint16 deviceID = get_device(bus, device);

    pci_dev->vendor_id = vendorID;
    pci_dev->device_id = deviceID;
    pci_dev->bus = bus;
    pci_dev->device = device;
    pci_dev->function = 0;

    if (read8(pci_dev->bus, pci_dev->device, pci_dev->function, 0xE) != 0x00) 
        return 1;
    pci_dev->bar[0] = read32(pci_dev->bus, pci_dev->device, pci_dev->function, 0x10);
    pci_dev->bar[1] = read32(pci_dev->bus, pci_dev->device, pci_dev->function, 0x14);
    pci_dev->bar[2] = read32(pci_dev->bus, pci_dev->device, pci_dev->function, 0x18);
    pci_dev->bar[3] = read32(pci_dev->bus, pci_dev->device, pci_dev->function, 0x1C);
    pci_dev->bar[4] = read32(pci_dev->bus, pci_dev->device, pci_dev->function, 0x20);
    pci_dev->bar[5] = read32(pci_dev->bus, pci_dev->device, pci_dev->function, 0x24);
    // for (int i = 0; i < 6; i++) {
    //     convert_raw_bar(&pci_dev->bar[i]);
    // }

    return 1;
}

void
pciinit()
{
    for (uint16 bus = 0; bus < 256; bus++) {
        for (uint8 device = 0; device < 32; device++) {
            pci_devs_ptr += read_device(bus, device, &pci_devs[pci_devs_ptr]);
            if (pci_devs_ptr >= 32) {
                goto done;
            }
        }
    }

done:
    for (uint8 i = 0; i < pci_devs_ptr; i++) {
        if (pci_devs[i].vendor_id == INTEL_VENDOR_ID && pci_devs[i].device_id == INTEL_E1000_NIC_ID) {
            e1000_init(&pci_devs[i]);
        }
    }
}
