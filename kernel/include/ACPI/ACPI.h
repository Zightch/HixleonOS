#pragma once

#define ACPI_RSDP_SIG 0x2052545020445352ull // "RSD PTR "

#define ACPI_MADT_SIG_APIC 0x43495041 // "APIC"

#include "APIC.h"

namespace ACPI {
    #pragma pack(push, 1)
    typedef struct {
        unsigned long long Signature;
        unsigned char      Checksum;
        unsigned char      OemId[6];
        unsigned char      Revision;
        unsigned int       RsdtAddress;
        unsigned int       Length;
        unsigned long long XsdtAddress;
        unsigned char      ExtendedChecksum;
        unsigned char      Reserved[3];
    } RSDP;
    
    typedef struct {
        unsigned int       Signature;
        unsigned int       Length;
        unsigned char      Revision;
        unsigned char      Checksum;
        unsigned char      OemId[6];
        unsigned long long OemTableId;
        unsigned int       OemRevision;
        unsigned int       CreatorId;
        unsigned int       CreatorRevision;
    } EADH; // EFI_ACPI_DESCRIPTION_HEADER
    
    typedef struct {
        EADH header;
        unsigned int LICA;// Local Interrupt Controller Address
    } RSDT;
    
    /* 64位版本
    typedef struct {
        EADH Header;
        unsigned long long LICA;// Local Interrupt Controller Address
    } XSDT;
    */

   typedef struct {
        EADH header;
        unsigned int LICA;// Local Interrupt Controller Address
        unsigned flags;
    } MADT;
    #pragma pack(pop)

    void init();
    RSDP getRSDP();
}
