#include "IDT.h"
#include "int/intFacade.h"

#define IDT_ATTR(dpl) ((0x70 << 5) | (dpl & 3) << 13 | 1 << 15)

extern "C" void asmIsr0();
extern "C" void asmIsr14();

unsigned long long idt[256];
unsigned short idtLimit = sizeof(idt) - 1;

void setIDT(unsigned short vector, unsigned short segSelector, void(*isr)(), unsigned char dpl) {
    unsigned int offset = ((unsigned int) isr);
    idt[vector] = ((offset & 0xFFFF0000 | IDT_ATTR(dpl)));
    idt[vector] <<= 32;
    idt[vector] |= ((segSelector << 16) | (offset & 0x0000FFFF));
}

void initIDT() {
    setIDT(0, 0x08, asmIsr0, 0);
    setIDT(14, 0x08, asmIsr14, 0);
}
