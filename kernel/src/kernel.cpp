#include "kernel.h"
#include "tty.h"
#include "GDT.h"
#include "IDT.h"
#include "memManager/physMem.h"
#include "ld.h"

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
    for (unsigned int i = 0; i < 256; i++)
        physMem::setPageUsage((void *) i, true);
    //0x100000 ~ kernelEnd 已使用
    unsigned int kernelEndPage = (GET_LD_DATA(kernelEnd) - 1 - 0xC0000000) >> 12;
    for (unsigned int i = 256; i < kernelEndPage; i++)
        physMem::setPageUsage((void *) i, true);
    //除去DRAM以外的其他内存区域标记为已使用(其实为不可用)
    unsigned int unusable = (memUpper + 1) >> 12;
    for (unsigned int i = unusable; i < 1048576; i++)
        physMem::setPageUsage((void *) i, true);

}
