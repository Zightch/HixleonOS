#include "interrupts/intTable.h"
#include "crash.h"

void isr0(IsrParam *) {
    crash("The divisor is 0\n");
}

void isr14(IsrParam *) {
    crash("Page fault\n");
}
