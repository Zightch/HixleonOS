#include "IDT.h"
#include "interrupts/intFacade.h"

#define IDT_ATTR(dpl) ((0x70 << 5) | (dpl & 3) << 13 | 1 << 15)

#define IDT_ENTRY 256

unsigned long long idt[IDT_ENTRY];
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
