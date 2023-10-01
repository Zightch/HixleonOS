#include "kernel.h"
#include "tty.h"
#include "GDT.h"
#include "IDT.h"
#include "memManager/kernelMem.h"
#include "ByteArray/ByteArray.h"
#include "tools/numTools.h"

//为了让通过编译的
extern "C" void __gxx_personality_v0(){}
extern "C" void _Unwind_Resume(){}

MemMap *memMap = nullptr;
unsigned short mapSize = 0xFFFF;
void *kpg = nullptr;
unsigned int kpgSize = -1;

void kernelInit(MemMap *mm, unsigned short ms, void *k, unsigned int ks) {
    initGDT();
    initIDT();
    memMap = mm;
    mapSize = ms;
    kpg = k;
    kpgSize = ks;
}

void kernelMain() {
    ttyClear();
    ttyPutStr("Hello, HixleonOS!\n");

    //寻找1MB开始的DRAM内存
    unsigned int dramBase = 0x100000;//内存基地址
    unsigned int dramUpper = 0;//最大寻址地址
    unsigned int dramSize = 0;//内存长度
    for (unsigned int i = 0; i < mapSize; i++) {
        if (memMap[i].base == dramBase) {
            dramUpper = dramSize = memMap[i].size;
            if (memMap[i].size > 0xFFF00000)
                dramUpper = 0xFFF00000;
            dramUpper += 0xFFFFF;
            break;
        }
        //如果未找到, 内存错误
        if (i == mapSize - 1) {
            ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "Memory error, DRAM memory based on 0x100000 not found\n");
            return;
        }
    }

    //初始化内存
    //kernelEnd开始向上找连续可用的虚拟页空间, 默认分配4个页(16KB)
    unsigned int heapNum = 4;
    if (!kernelMemInit(heapNum, dramUpper)) {//如果堆区分配失败
        ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "Memory error, Alloc heap memory fail!\n");
        return;
    }
    initRefCount();//初始化共享指针的引用计数器

    ByteArray endl("\n", 2);
/*    int cr0, cr4;
    asm volatile ("movl %%cr0, %0": "=r" (cr0));
    asm volatile ("movl %%cr4, %0": "=r" (cr4));
    ttyPutStr("cr0: " + toByteArray(cr0, 2, 32) + endl);
    ttyPutStr("cr4: " + toByteArray(cr4, 2, 32) + endl);*/
    float dramSizeGiB = dramSize * 1.0f / 1024.0f / 1024.0f / 1024.0f;
    //ttyPutStr(toByteArray(*((int *) (&dramSizeGiB)), 16) + endl);
    ttyPutStr("The memory size is: " + toByteArray(dramSize) + "Byte (about " + toByteArray(dramSizeGiB, 2) + "GiB)" + endl);
    ttyPutStr("Kernel done.");
}
