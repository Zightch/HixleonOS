#include "kernel.h"
#include "tty.h"
#include "GDT.h"
#include "IDT.h"
#include "APIC.h"
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
    //kernelEnd开始向上找连续可用的虚拟页空间, 默认分配4个页(16kiB)
    unsigned int heapNum = 4;
    if (!kernelMemInit(heapNum, dramUpper)) {//如果堆区分配失败
        ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "Memory error, Alloc heap memory fail!\n");
        return;
    }
    initRefCount();//初始化共享指针的引用计数器

    ByteArray endl("\n", 2);
    ttyPutStr("Mem map:  Base              Size              Type\n");
    for (unsigned int i = 0; i < mapSize; i++) {//打印内存地图
        ByteArray index = toByteArray(i, 16, 8);
        ByteArray base = toByteArray(memMap[i].base, 16, 16);
        ByteArray size = toByteArray(memMap[i].size, 16, 16);
        ByteArray type = toByteArray(memMap[i].type, 16, 8);
        ttyPutStr(index + "  " + base + "  " + size + "  " + type + endl);
    }
    float dramSizeGiB = dramSize * 1.0f / 1024.0f / 1024.0f / 1024.0f;
    ttyPutStr("The DRAM size is: " + toByteArray(dramSize) + "Byte (about " + toByteArray(dramSizeGiB, 2) + "GiB)" + endl);

    //初始化APIC
    initAPIC();
}
