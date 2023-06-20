#include <Hixleon/kernel.h>
#include <Hixleon/tty/tty.h>
#include <Hixleon/arch/GDT.h>
#include <Hixleon/arch/IDT.h>

void kernelInit_(MemoryMap *memoryMap, unsigned short mapSize) {
    initGDT_();
    initIDT_();
}

void kernelMain_() {
    ttyClear();
    ttyPutStr((char *) "Hello, HixleonOS!\nLine Test1\nLine Test2\n");
    ttyPutStr(VGA_COLOR_GREEN, VGA_COLOR_BLACK, (char *) "Color Test1\n");
    ttyPutStr(VGA_COLOR_BLUE, VGA_COLOR_BLACK, (char *) "Color Test2\n");
    ttyPutStr(VGA_COLOR_RED, VGA_COLOR_BLACK, (char *) "Color Test3\n");
}
