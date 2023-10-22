#pragma once

#include "ACPI.h"

#define ACPI_MADT_APIC_LAPIC 0x0  // Local APIC
#define ACPI_MADT_APIC_IOAPIC 0x1 // I/O APIC
#define ACPI_MADT_APIC_INTSO 0x2  // Interrupt Source Override

namespace ACPI {
    namespace APIC {
        #pragma pack(push, 1)
        typedef struct {
            EADH header;
            unsigned int LICA;// Local Interrupt Controller Address
            unsigned flags;
        } MADT;

        typedef struct {
            unsigned char Type;
            unsigned char Length;
        } Header;
        
        typedef struct {
            Header header;
            unsigned char IOAPIC_ID;
            unsigned char Reserved;
            unsigned int IOAPIC_Address;
            unsigned int GIS_Offset; // The global system interrupt offset for this IOAPIC. (Kind of IRQ offset for a slave IOAPIC)
        } IOAPIC;

        typedef struct {
            Header header;
            unsigned char Bus;
            unsigned char Source;
            unsigned int GSI; // Global System Interrupt
            unsigned short flags;
        } IntSO; // Interrupt Source Override

        typedef struct {
            bool isReady = false;
            IOAPIC ioapic;
            IntSO intSO[24];
            unsigned int intOSSize = 0;
        } Context;
        #pragma pack(pop)

        Context getContext();
    }
}
