#include "pci.h"

static uint16
read16(uint8 bus, uint8 device, uint8 function, uint8 offset)
{
    uint64 address = PCI_ECAM
                     + ((uint64)bus << 20)
                     + ((uint64)device << 15)
                     + ((uint64)function << 12)
                     + (offset & ~3);
    uint32 val = *(volatile uint32*)address;
    return (val >> ((offset & 2) * 8)) & 0xFFFF;
}

static uint16
getVendorId(uint16 bus, uint8 device)
{
    return read16(bus, device, 0, 0);
}

static uint16
getDevice(uint16 bus, uint8 device)
{
    return read16(bus, device, 0, 2);
}

static void
checkDevice(uint16 bus, uint8 device)
{
    uint16 vendorID = getVendorId(bus, device);
    if (vendorID == 0xFFFF) {
        return;
    }
    uint16 deviceID = getDevice(bus, device);
    printf("pci: bus %d device %d vendor %x device %x.\n", bus, device, vendorID, deviceID);
}

void
pciinit()
{
    printf("pci process initiated.\n");
    uint16 bus;
    uint8 device;

    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            checkDevice(bus, device);
        }
    }
}
