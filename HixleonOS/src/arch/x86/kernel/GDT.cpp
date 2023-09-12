#include "Hixleon/arch/x86/GDT.h"

#define GDT_ENTRY 5

unsigned long long gdt[GDT_ENTRY];
unsigned short gdtLimit = sizeof(gdt) - 1;

void setGDT(unsigned short index, unsigned int base, unsigned int limit, unsigned int flags) {
    gdt[index] = SEG_BASE_H(base) | flags | SEG_LIM_H(limit) | SEG_BASE_M(base);
    gdt[index] <<= 32;
    gdt[index] |= SEG_BASE_L(base) | SEG_LIM_L(limit);
}

void initGDT() {
    setGDT(0, 0, 0, 0);
    setGDT(1, 0, 0xFFFFF, SEG_R0_CODE);
    setGDT(2, 0, 0xFFFFF, SEG_R0_DATA);
    setGDT(3, 0, 0xFFFFF, SEG_R3_CODE);
    setGDT(4, 0, 0xFFFFF, SEG_R3_DATA);
}
