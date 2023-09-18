#include "kernel.h"
#include "tty.h"
#include "GDT.h"
#include "IDT.h"
#include "memManager/physMem.h"
#include "memManager/virtMem.h"
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
    //获取物理页, 建立映射
    for (int i = 0; i < num; i++) {
        int pp = PhysMem::getUsablePage();//获取一个可用物理页
        PhysMem::setPageUsage(pp, true);//标记为已使用
        if (0 <= pp && pp <= 1048575)//如果物理页合法
            if (VirtMem::map(heapStart + i, pp))//如果映射成功, 就继续执行
                continue;
        //否则说明内存已满
        PhysMem::setPageUsage(pp, false);//还原状态
        for (int j = i - 1; j >= 0; j--) {//释放已经建立映射的页
            int pp = VirtMem::virtPageToPhysPage(heapStart + j);
            VirtMem::unmap(heapStart + j);
            PhysMem::setPageUsage(pp, false);
        }
        return -1;
    }

    //堆区分配完毕, 页表分配成功
    unsigned int *heap = (unsigned int *) (heapStart << 12);//获取地址
    unsigned int heapLast4ByteOffset = (heapNum * 1024) - 1;

    //全填空闲
    //堆区头部数据(高12位固定为0xFFF, 低20位为堆区占用多少个页)
    heap[0] = 0xFFF00000 | heapNum;
    //尾部数据(高12位固定为0xFFF, 低20位为当前堆区向前多少个页为首页)
    heap[heapLast4ByteOffset] = 0xFFF00000 | (-((int) heapNum));

    //接下来是Implicit free list
    unsigned int freeSize = heapLast4ByteOffset - 1;
    heap[1] = freeSize << 2;
    heap[freeSize] = heap[1];
    return heapStart;
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
    int heapStart = kernelHeapInit(heapNum);
    if (0 > heapStart || heapStart > 1048575) {//如果堆区分配失败
        ttyPutStr(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK, "Memory error, Alloc heap memory fail!\n");
        return;
    }

}
