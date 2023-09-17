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
    unsigned int kernelEndPage = (GET_LD_DATA(kernelEnd) - 0xC0000000) >> 12;
    PhysMem::setSectionPageUsage(256, kernelEndPage - 256, true);
    //除去DRAM以外的其他内存区域标记为已使用(其实为不可用)
    unsigned int unusable = (memUpper + 1) >> 12;
    PhysMem::setSectionPageUsage(unusable, 1048576 - unusable, true);

    //获取可用页测试
    unsigned int tmp0 = PhysMem::getUsablePage();
    if (PhysMem::addrIsUsing((void *) ((tmp0 << 12) + 1)))
        ttyPutStr("gup false\n");//如果页被占用, 输出gup false

    //初始化虚拟页管理器
    //内存分配测试
    char *heap = (char *) malloc(sizeof(testStr));
    for (unsigned int i = 0; i < sizeof(testStr); i++)
        heap[i] = testStr[i];
    ttyPutStr(heap);
}
