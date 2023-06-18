#include <Hixleon/arch/IDT.h>
#include <Hixleon/arch/interrupts/intFacade.h>

#define IDT_ENTRY 32

unsigned long long idt_[32];
unsigned short idtLimit_ = sizeof(idt_) - 1;

void setIDT(unsigned short vector, unsigned short segSelector, void(*isr)(), unsigned char dpl) {
    unsigned int offset = ((unsigned long) isr);
    idt_[vector] = ((offset & 0xFFFF0000 | IDT_ATTR(dpl)));
    idt_[vector] <<= 32;
    idt_[vector] |= ((segSelector << 16) | (offset & 0x0000FFFF));
}

void initIDT_() {
    setIDT(FAULT_DIVISION_ERROR, 0x08, asmIsr0, 0);
}
