#include "interrupts/intTable.h"
#include "tty.h"

void isr0(IsrParam*) {
    ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "The divisor is 0!\n");
    while (true)
        __asm__("hlt");
}
