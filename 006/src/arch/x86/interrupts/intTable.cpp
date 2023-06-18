#include <Hixleon/arch/interrupts/intTable.h>
#include <Hixleon/tty/tty.h>

void isr0(IsrParam*) {
    ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, (char*)"The divisor is 0!\n");
}
