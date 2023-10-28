#include "crash.h"
#include "tty.h"
#include "cpu.h"

void crash(const char* str) {
    ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, str);
    while (true)
        CPU::hlt();
}
