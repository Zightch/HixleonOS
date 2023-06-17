#include "includes/Hixleon/kernel.h"
#include "includes/Hixleon/tty/tty.h"
#include "includes/Hixleon/arch/GDT.h"

extern "C" {
void kernelInit_() {
    initGDT_();
    // TODO
}

void kernelMain_(void *memoryMax) {
    // TODO
    ttyClear();
    ttyPutStr((char *) "Hello, HixleonOS!\nLine Test1\nLine Test2");
}
}
