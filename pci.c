#include "types.h"
#include "x86.h"
#include "defs.h"

#define CONFIG_ADDRESS 0xCF8  // specifies config address that is required to be accesses
#define CONFIG_DATA    0xCFC  // generate the configu access and will transfer the data to or from the CONFIG_DATA register

#define NETWORK_VENDOR_ID 0x8086
#define NETWORK_DEVICE_ID 0x100e

#define PCI_VENDOR_ID      0x00
#define PCI_DEVICE_IF      0x02
#define PCI_COMMAND        0x04
#define PCI_BAR0           0x10
#define PCI_INTERRUPT_LINE 0x3c

#define PCI_CMD_MEM        0x2
#define PCI_CMD_MASTER     0x4

extern int ncpu;
int e1000_irq = -1;

extern void e1000_init(uint, uchar);

struct pci_func {
    uchar bus, dev, func;
    ushort vendor, device;
    uint bar0;
    uchar irqline;
};

static uint
pci_config_read_dword(uchar bus, uchar slot, uchar func, uchar offset)
{
    uint val;
    val = ((uint)bus << 16) | ((uint)slot << 11) | ((uint)func << 8) | (offset & 0xFC) | 0x80000000;
    outl(CONFIG_ADDRESS, val);
    return inl(CONFIG_DATA);
}

static ushort
pci_config_read_word(uchar bus, uchar slot, uchar func, uchar offset)
{
    uint val = pci_config_read_dword(bus, slot, func, offset);
    return (ushort)((val >> ((offset & 2) * 8)) & 0xFFFF);
}

static uchar
pci_config_read_byte(uchar bus, uchar slot, uchar func, uchar offset)
{
    uint val = pci_config_read_dword(bus, slot, func, offset);
    return (uchar)(val >> ((offset & 3) * 8)) & 0xFF;
}

static void
pci_config_write_word(uchar bus, uchar slot, uchar func, uchar offset, ushort data)
{
    uint val = 0x80000000
        | ((uint)bus << 16)
        | ((uint)slot << 11)
        | ((uint)func << 8)
        | (offset & 0xFC);

    uint old_val = pci_config_read_dword(bus, slot, func, offset);
    uint shift = (offset & 2) * 8;
    uint mask = 0xFFFF << shift;
    uint new_val = (old_val & ~mask) | ((uint)data << shift);

    outl(CONFIG_ADDRESS, val);
    outl(CONFIG_DATA, new_val);
}

static uchar
check_function(uchar bus, uchar device, uchar function)
{
    ushort  vendor_id = pci_config_read_word(bus, device, function, 0x00);
    ushort  device_id = pci_config_read_word(bus, device, function, 0x02);

    if (vendor_id == NETWORK_VENDOR_ID && device_id == NETWORK_DEVICE_ID) {
        return 1;
    }

    return 0;
}

static uchar
check_device(uchar bus, uchar device)
{
    uchar   function = 0;
    ushort  vendor_id;

    vendor_id = pci_config_read_word(bus, device, function, 0x00);
    if (vendor_id == 0xFFFF) {
        return 0;
    }

    return check_function(bus, device, function);
}

void
check_all_buses(void)
{
    ushort           bus;
    uchar            device;
    struct pci_func  f;

    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            if (check_device((uchar)bus, device) == 1) {
                f.bus = bus;
                f.dev = device;
                f.func = 0;
                f.vendor = NETWORK_VENDOR_ID;
                f.device = NETWORK_DEVICE_ID;
                f.bar0 = pci_config_read_dword(bus, device, 0, PCI_BAR0);
                f.bar0 &= ~0xF;
                f.irqline = pci_config_read_byte(bus, device, 0, PCI_INTERRUPT_LINE);
                e1000_irq = f.irqline;

                ushort cmd = pci_config_read_word(bus, device, 0, PCI_COMMAND);
                cmd |= PCI_CMD_MEM | PCI_CMD_MASTER;
                pci_config_write_word(bus, device, 0, PCI_COMMAND, cmd);

                if (e1000_irq < 32)
                    ioapicenable(e1000_irq, ncpu - 1);

                e1000_init(f.bar0, f.irqline);
                return;
            }
        }
    }
}

void
pciinit(void)
{
    check_all_buses();
}
