#include "includes/Hixleon/arch/GDT.h"

#define GDT_ENTRY 5

unsigned long long gdt_[GDT_ENTRY];
unsigned short gdtLimit_ = sizeof(gdt_);

void setGDTEntry(unsigned int index, unsigned int base, unsigned int limit, unsigned int flags) {
    gdt_[index] = SEG_BASE_H(base) | flags | SEG_LIM_H(limit) | SEG_BASE_M(base);
    gdt_[index] <<= 32;
    gdt_[index] |= SEG_BASE_L(base) | SEG_LIM_L(limit);
}

void initGDT_() {
    setGDTEntry(0, 0, 0, 0);
    setGDTEntry(1, 0, 0xFFFFF, SEG_R0_CODE);
    setGDTEntry(2, 0, 0xFFFFF, SEG_R0_DATA);
    setGDTEntry(3, 0, 0xFFFFF, SEG_R3_CODE);
    setGDTEntry(4, 0, 0xFFFFF, SEG_R3_DATA);
}
