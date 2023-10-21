#include "ACPI/ACPI.h"
#include "ACPI/APIC.h"
#include "tty.h"
#include "kernel.h"
#include "cpu.h"
#include "memManager/virtMem.h"
#include "memManager/physMem.h"
#include "memManager/kernelMem.h"
#include "crash.h"

#define INVALID_REDP (RSDP*)0xFFFFFFFF

namespace ACPI {
    RSDP *rsdp = INVALID_REDP;
    APIC::Context apicContext;

    void findRSDP() {
        for (unsigned short i = 0; i < memMapSize; i++) {
            if (memMap[i].type != 2) continue;
            if (memMap[i].base >= 0x1000000) break;
            unsigned int base = memMap[i].base;
            unsigned int size = memMap[i].size;
            for (unsigned int j = base; j < base + size; j += 16) {
                RSDP *tmp = (RSDP *) j;
                if (tmp->Signature == ACPI_RSDP_SIG)
                    rsdp = tmp;
            }
        }
    }

    RSDP getRSDP() {
        if (rsdp == INVALID_REDP)
            return {};
        return *rsdp;
    }

    void init() {
        findRSDP();

        if (rsdp == INVALID_REDP) crash("Error, ACPI_RSDP not fount\n");

        auto virtPage0 = VirtMem::getUsablePage(2);//默认分配两个页(确保内存完整)
        if (0 > virtPage0 || virtPage0 > 1048575)//如果虚拟页分配失败
            crash("Error, ACPI_RSDP Virt page alloc fail  Code: 0x00\n");

        auto physPage = rsdp->RsdtAddress >> 12;
        if (!VirtMem::map(virtPage0, physPage))//建立映射
            crash("Error, ACPI_RSDP Virt page alloc fail  Code: 0x01\n");//映射失败
        if (!VirtMem::map(virtPage0 + 1, physPage + 1))//建立映射
            crash("Error, ACPI_RSDP Virt page alloc fail  Code: 0x02\n");//映射失败

        auto rsdt = (RSDT * )((virtPage0 << 12) | (rsdp->RsdtAddress & 0x00000FFF));
        unsigned int entrySize = (rsdt->header.Length - sizeof(EADH)) >> 2;
        for (unsigned int i = 0; i < entrySize; i++) {
            auto tmp = (unsigned int) ((EADH * *) & rsdt->LICA)[i];
            auto virtPage1 = virtPage0;//默认等于virtPage0
            if ((tmp >> 12) != physPage) {//如果与RSDP不在同一个页
                virtPage1 = VirtMem::getUsablePage(2);//获取两个页(确保内存完整)
                if (0 > virtPage1 || virtPage1 > 1048575)//如果虚拟页分配失败
                    crash("Error, ACPI_RSDT Virt page alloc fail  Code: 0x00\n");
                if (!VirtMem::map(virtPage1, tmp >> 12))//建立映射
                    crash("Error, ACPI_RSDT Virt page alloc fail  Code: 0x01\n");//映射失败
                if (!VirtMem::map(virtPage1 + 1, (tmp >> 12) + 1))//建立映射
                    crash("Error, ACPI_RSDT Virt page alloc fail  Code: 0x02\n");//映射失败
            }
            auto eadh = (EADH * )((virtPage1 << 12) | (tmp & 0x00000FFF));
            switch (eadh->Signature) {
                case ACPI_MADT_SIG_APIC: {
                    auto madt = (MADT *) eadh;
                    unsigned int icsIterator = (unsigned int) madt + sizeof(MADT);
                    unsigned int icsEnd = (unsigned int) madt + madt->header.Length;
                    while (icsIterator < icsEnd) {
                        auto item = (APIC::Header *) icsIterator;
                        switch (item->Type) {
                            case ACPI_MADT_APIC_LAPIC: {
                                break;
                            }
                            case ACPI_MADT_APIC_IOAPIC: {
                                auto ioapic = (APIC::IOAPIC *) item;
                                apicContext.ioapic = *ioapic;
                                break;
                            }
                            case ACPI_MADT_APIC_INTSO: {
                                auto intSO = (APIC::IntSO *) item;
                                apicContext.intSO[apicContext.intOSSize++] = *intSO;
                                break;
                            }
                            default:
                                break;
                        }
                        icsIterator += item->Length;
                    }
                    apicContext.isReady = true;
                    break;
                }
                default:
                    break;
            }
            if (virtPage1 != virtPage0) {//如果不相等说明是新建立的映射
                VirtMem::unmap(virtPage1);//解除映射
                VirtMem::unmap(virtPage1 + 1);//解除映射
            }
        }
        VirtMem::unmap(virtPage0);//解除映射
        VirtMem::unmap(virtPage0 + 1);//解除映射
    }

    APIC::Context APIC::getContext() {
        return apicContext;
    }
}
