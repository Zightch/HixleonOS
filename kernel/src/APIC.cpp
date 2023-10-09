#include "APIC.h"

#include "interrupts/LAPIC.h"
#include "interrupts/IOAPIC.h"
#include "interrupts/fixedIntNum.h"
#include "tty.h"
#include "cpu.h"
#include "tools/numTools.h"
#include "ByteArray/ByteArray.h"

void initAPIC() {
    {//检查APIC是否存在
        if (iscpuid()) {
            unsigned int eax, ebx, ecx, edx;
            cpuid(1, eax, ebx, ecx, edx);
            if (((edx >> 9) & 1) == 0) {
                ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "Error, CPU not carry APIC");
                while (true)
                    __asm__("hlt");
            }
        } else
            ttyPutStr("Warning, this processor does not support the CPUID instruction and will not be able to obtain CPU information\n");
    }//检查APIC是否存在

    __asm__(
            "movl $0xff, %eax\n"
            "outb %al, $0xa1\n"
            "outb %al, $0x21"
            );//禁用8259PIC

    unsigned int eax, edx;
    __asm__(
            "movl $0x1b, %%ecx\n"
            "rdmsr\n"
            "orl $0x800, %%eax\n"
            "wrmsr\n"
            "movl %%eax, %0\n" // 将eax寄存器值保存到eax变量中
            "movl %%edx, %1\n" // 将edx寄存器值保存到edx变量中
            : "=r" (eax), "=r" (edx) // 输出操作数列表
            : // 输入操作数列表
            : "%ecx", "%eax", "%edx" // 破坏描述符列表
            );
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
