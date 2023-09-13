#include "arch/x86/kernel/IDT.h"
#include "arch/x86/kernel/interrupts/intFacade.h"

#define IDT_ENTRY 32

unsigned long long idt[32];
unsigned short idtLimit = sizeof(idt) - 1;

void setIDT(unsigned short vector, unsigned short segSelector, void(*isr)(), unsigned char dpl) {
    unsigned int offset = ((unsigned long) isr);
    idt[vector] = ((offset & 0xFFFF0000 | IDT_ATTR(dpl)));
    idt[vector] <<= 32;
    idt[vector] |= ((segSelector << 16) | (offset & 0x0000FFFF));
}

void initIDT() {
    setIDT(FAULT_DIVISION_ERROR, 0x08, asmIsr0, 0);
}
