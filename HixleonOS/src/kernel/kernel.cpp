#include "kernel/kernel.h"
#include "kernel/tty.h"
#include "kernel/GDT.h"
#include "kernel/IDT.h"

void kernelInit(MemoryMap *memoryMap, unsigned short mapSize, void *kpg) {
    initGDT();
    initIDT();
}

void kernelMain() {
    ttyClear();
    ttyPutStr("Hello, HixleonOS!\nLine Test1\nLine Test2\n");
    ttyPutStr(VGA_COLOR_GREEN, VGA_COLOR_BLACK, "Color Test1\n");
    ttyPutStr(VGA_COLOR_BLUE, VGA_COLOR_BLACK, "Color Test2\n");
    ttyPutStr(VGA_COLOR_RED, VGA_COLOR_BLACK, "Color Test3\n");
    asm("int $0");
}
