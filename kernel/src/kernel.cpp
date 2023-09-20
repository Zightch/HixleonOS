#include "kernel.h"
#include "tty.h"
#include "GDT.h"
#include "IDT.h"
#include "memManager/physMem.h"
#include "memManager/virtMem.h"
#include "memManager/memManager.h"
#include "ld.h"

MemMap *memMap = nullptr;
unsigned short mapSize = 0xFFFF;
void *kpg = nullptr;
unsigned int kpgSize = -1;

const char testStr[] = "This document contains all four volumes of the Intel 64 and IA-32 Architectures Software\n"
"Developer's Manual: Basic Architecture, Order Number 253665; Instruction Set Reference A-Z, Order\n"
"Number 325383; System Programming Guide, Order Number 325384; Model-Specific Registers, Order\n"
"Number 335592. Refer to all four volumes when evaluating your design needs.\n";

void kernelInit(MemMap *mm, unsigned short ms, void *k, unsigned int ks) {
    initGDT();
    initIDT();
    memMap = mm;
    mapSize = ms;
    kpg = k;
    kpgSize = ks;
}

int kernelHeapInit(unsigned int num) {
    //如果num不合法
    if (num >= 1048576)
        return -1;

    //初始化堆区
    //kernelEnd开始向上找连续可用的虚拟页空间
    unsigned int heapStart = GET_LD_DATA(kernelEnd) >> 12;//虚拟kernel结束页号
    {
        unsigned int heapNum = 0;
        while (heapNum < num) {
            if (heapStart >= 1048576)
                return -1;
            if (!VirtMem::pageIsUsing(heapStart))
                heapNum++;
            else {
                heapStart += heapNum;//跳过无法连续的区域
                heapNum = 0;
            }
        }
    }

    if(allocHeap(heapStart, num))
        return heapStart;
    return -1;
}

void kernelMain() {
    ttyClear();
    ttyPutStr("Hello, HixleonOS!\n");

    //寻找1MB开始的DRAM内存
    unsigned int memBase = -1;//内存基地址
    unsigned int memUpper = 0;//最大寻址地址
    unsigned int memSize = 0;//内存长度
    for (unsigned int i = 0; i < mapSize; i++) {
        if (memMap[i].base == 0x100000) {
            memUpper = memMap[i].size;
            if (memMap[i].size > 0xFFF00000)
                memUpper = 0xFFF00000;
            memUpper += 0xFFFFF;
            memBase = 0x100000;
            memSize = memUpper - memBase + 1;
            break;
        }
        //如果未找到, 内存错误
        if (i == mapSize - 1) {
            ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "Memory error, DRAM memory based on 0x100000 not found\n");
            return;
        }
    }

    //开始标记已使用的物理内存页
    //0 ~ 0xFFFFF(1MB, 1048576B)已使用
    PhysMem::setSectionPageUsage(0, 256, true);
    //0x100000 ~ kernelEnd 已使用
    unsigned int physKernelEndPage = (GET_LD_DATA(kernelEnd) - 0xC0000000) >> 12;
    PhysMem::setSectionPageUsage(256, physKernelEndPage - 256, true);
    //除去DRAM以外的其他内存区域标记为已使用(其实为不可用)
    unsigned int unusable = (memUpper + 1) >> 12;
    PhysMem::setSectionPageUsage(unusable, 1048576 - unusable, true);

    //初始化堆区
    //kernelEnd开始向上找连续可用的虚拟页空间, 默认分配4个页(16KB)
    unsigned int heapNum = 4;
    int heap = kernelHeapInit(heapNum);
    if (0 > heap || heap > 1048575) {//如果堆区分配失败
        ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "Memory error, Alloc heap memory fail!\n");
        return;
    }
    //拓展2个页
    if (!expandHeap(heap, 2))
        ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "Memory error, heap expand fail!\n");
    char *testMem = (char *) hlmalloc(heap, sizeof(testStr));
    for (int i = 0; i < sizeof(testStr); i++)
        testMem[i] = testStr[i];
    ttyPutStr(testMem);
    hlfree(heap, testMem);
    freeHeap(heap);
}
