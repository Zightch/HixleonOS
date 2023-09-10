#include "Hixleon/kernel.h"
#include "Hixleon/arch/x86/tty.h"
#include "Hixleon/arch/x86/GDT.h"
#include "Hixleon/arch/x86/IDT.h"

void kernelInit_(MemoryMap *memoryMap, unsigned short mapSize) {
    initGDT_();
    initIDT_();
}

void kernelMain_() {
    ttyClear();
    ttyPutStr("Hello, HixleonOS!\nLine Test1\nLine Test2\n");
    ttyPutStr(VGA_COLOR_GREEN, VGA_COLOR_BLACK, "Color Test1\n");
    ttyPutStr(VGA_COLOR_BLUE, VGA_COLOR_BLACK, "Color Test2\n");
    ttyPutStr(VGA_COLOR_RED, VGA_COLOR_BLACK, "Color Test3\n");
    asm("int $0");
}
