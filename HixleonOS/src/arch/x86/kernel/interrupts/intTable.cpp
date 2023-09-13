#include "arch/x86/kernel/interrupts/intTable.h"
#include "arch/x86/kernel/tty.h"

void isr0(IsrParam*) {
    ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, (char*)"The divisor is 0!\n");
}
