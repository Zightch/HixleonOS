#include "int/intFacade.h"

typedef void (*IntFn)(IsrParam*);

IntFn intTable[256] = {
        isr0, 0, 0, 0, 0, 0, 0, 0, // 0 - 7
        0, 0, 0, 0, 0, 0, isr14, 0, // 8 - 15
};

const unsigned int intTableSize = sizeof(intTable) - 1;

void interruptHandler(IsrParam *ipp) {
    if (ipp->vector < intTableSize)
        intTable[ipp->vector](ipp);
}
