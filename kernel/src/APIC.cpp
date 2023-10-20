#include "APIC.h"

#include "interrupts/LAPIC.h"
#include "interrupts/IOAPIC.h"
#include "interrupts/fixedIntNum.h"
#include "tty.h"
#include "cpu.h"
#include "tools/numTools.h"
#include "ByteArray/ByteArray.h"
#include "crash.h"

void initAPIC() {
    {//检查APIC是否存在
        if (iscpuid()) {
            unsigned int eax, ebx, ecx, edx;
            cpuid(1, eax, ebx, ecx, edx);
            if (((edx >> 9) & 1) == 0)
                crash("Error, CPU not carry APIC");
        } else
            ttyPutStr(VGA_COLOR_BROWN, VGA_COLOR_BLACK, "Warning, this processor does not support the CPUID instruction and will not be able to obtain CPU information\n");
    }//检查APIC是否存在

    //禁用8259PIC
    outb(0xa1, 0xff);
    outb(0x21, 0xff);

    //硬启用APIC
    unsigned int IA32_APIC_BASE = rdmsr(0x1b);
    IA32_APIC_BASE |= 0x800;
    wrmsr(0x1b, IA32_APIC_BASE);

    ByteArray endl("\n", 2);
    //ttyPutStr(toByteArray(edx, 16, 8) + toByteArray(eax, 16, 8) + endl);

    LAPIC_REG(080) = 0x20;//设置中断优先级(放行32以上的中断)
    LAPIC_REG(350) = LAPIC_LINT0 | LAPIC_DM(LAPIC_DM_FIXED) | LAPIC_MASK(0);//LINT0
    LAPIC_REG(360) = 0 | LAPIC_DM(LAPIC_DM_NMI) | LAPIC_MASK(1);//LINT1
    LAPIC_REG(370) = LAPIC_ERROR | LAPIC_DM(LAPIC_DM_FIXED) | LAPIC_MASK(0);//Error
    LAPIC_REG(0F0) |= 0x1FF;//软启用LAPIC, 并设置伪中断号为255

//    ttyPutStr("080: " + toByteArray(LAPIC_REG(080), 2, 32) + endl);
//    ttyPutStr("350: " + toByteArray(LAPIC_REG(350), 2, 32) + endl);
//    ttyPutStr("360: " + toByteArray(LAPIC_REG(360), 2, 32) + endl);
//    ttyPutStr("370: " + toByteArray(LAPIC_REG(370), 2, 32) + endl);
//    ttyPutStr("0F0: " + toByteArray(LAPIC_REG(0F0), 2, 32) + endl);

    //初始化IOAPIC
    IOAPIC::init();
}
