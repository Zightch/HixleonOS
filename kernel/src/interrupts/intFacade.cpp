#include "interrupts/intFacade.h"

typedef void (*IntFn)(IsrParam*);

IntFn intTable[255] = {
        isr0, 0, 0, 0, 0, 0, 0, 0, // 0 - 7
        0, 0, 0, 0, 0, 0, isr14, 0, // 8 - 15
};

unsigned int intTableSize = sizeof(intTable);

void interruptHandler(IsrParam *ipp) {
    if (ipp->vector < intTableSize)
        intTable[ipp->vector](ipp);
}
