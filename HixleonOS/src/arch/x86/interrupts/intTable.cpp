#include "Hixleon/arch/x86/interrupts/intTable.h"
#include "Hixleon/arch/x86/tty.h"

void isr0(IsrParam*) {
    ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, (char*)"The divisor is 0!\n");
}
