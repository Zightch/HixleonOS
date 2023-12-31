#include "int/APIC/APIC.h"
#include "int/APIC/LAPIC/LAPIC.h"
#include "int/APIC/IOAPIC.h"

#include "cpu.h"
#include "tty.h"
#include "crash.h"

#include "int/APIC/LAPIC/timer.h"

namespace APIC {
    void init() {
        {//检查APIC是否存在
            if (CPU::iscpuid()) {
                unsigned int eax, ebx, ecx, edx;
                CPU::cpuid(1, eax, ebx, ecx, edx);
                if (((edx >> 9) & 1) == 0)
                    crash("Error, CPU not carry APIC");
            } else
                ttyPutStr(VGA_COLOR_BROWN, VGA_COLOR_BLACK, "Warning, this processor does not support the CPUID instruction and will not be able to obtain CPU information\n");
        }//检查APIC是否存在

        //禁用8259PIC
        CPU::outb(0xa1, 0xff);
        CPU::outb(0x21, 0xff);

        //硬启用APIC
        auto IA32_APIC_BASE = CPU::rdmsr(0x1b);
        IA32_APIC_BASE |= 0x800;
        CPU::wrmsr(0x1b, IA32_APIC_BASE);

        //初始化LAPIC
        LAPIC::init();

        //初始化IOAPIC
        IOAPIC::init();

        //初始化LAPIC Timer与计算CPU时钟频率
        LAPIC::Timer::init();
    }
}
