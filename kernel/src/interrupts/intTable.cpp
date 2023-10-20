#include "interrupts/intTable.h"
#include "tty.h"
#include "cpu.h"

void isr0(IsrParam*) {
    ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "The divisor is 0\n");
    hlt();
}
