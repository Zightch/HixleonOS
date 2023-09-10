#include "Hixleon/arch/x86/interrupts/intFacade.h"

typedef void (*IntFn)(IsrParam*);

IntFn intTable[1] = {isr0};

unsigned int intTableSize = sizeof(intTable);

void interruptHandler(IsrParam *ipp) {
    if (ipp->vector < intTableSize)
        intTable[ipp->vector](ipp);
}
