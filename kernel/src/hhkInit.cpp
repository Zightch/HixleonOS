#include "page.h"
#include "ld.h"

/*
内核页表布局
4KB PDE (全局固定)
  0     4KB  0 ~ 0xFFFFF + hhkInit (对等映射)
  1 - 4 16MB kernel(0xC0000000)
*/

#define PE_MAX_NUM 1024
#define PTE_MAX_NUM 5

#pragma pack(push, 1)
struct KernelPageTable {
    unsigned int pde[PE_MAX_NUM];//页目录项
    unsigned int pte[PTE_MAX_NUM][PE_MAX_NUM];//页表表项
};
#pragma pack(pop)

void HHK_INIT_TEXT kernelPageInit(KernelPageTable *ptd) {
    // 对低1MiB空间进行对等映射
    // 设置一级页表(页目录)
    ptd->pde[0] = PE(PE_PRESENT, &ptd->pte[0]);

    for (unsigned int i = 0; i < 256; i++)
        ptd->pte[0][i] = PE(PE_PRESENT | PE_WRITE, (i << 12));

    // 对hhkInit空间行进对等映射
    unsigned int hhkInitPageNum = (GET_LD_DATA(hhkInitEnd) - 0x100000 + 0x1000 - 1) >> 12;//hhk初始化的代码数据所占页大小
    if (hhkInitPageNum >= 1024 - 256)while(true);//如果hhkInit占目表项数量超过1024 - 256, 无法进入内核
    for (unsigned int i = 0; i < hhkInitPageNum; i++)
        ptd->pte[0][256 + i] = PE(PE_PRESENT | PE_WRITE, 0x100000 + (i << 12));

    // 开始映射高半核区域
    unsigned int kernelPDE = (GET_LD_DATA(kernelStart) & 0xFFC00000UL) >> 22;//高半核起始页目录项
    unsigned int kernelPTE = (GET_LD_DATA(kernelStart) & 0x003FF000UL) >> 12;//高半核起始页表表项
    unsigned int kernelPMA = GET_LD_DATA(kernelStart) - 0xC0000000;//高半核起始物理地址
    unsigned int kernelPageEntNum = (GET_LD_DATA(kernelEnd) - GET_LD_DATA(kernelStart) + 0x1000 - 1) >> 12;//高半核所占页表表项大小
    unsigned int kernelPageDirNum = (kernelPageEntNum + 1024 - 1) >> 10;//kernel所占页目录项数量
    if (kernelPageDirNum > PTE_MAX_NUM - 1)while(true);//如果内核所占目录项数量超过4, 无法进入内核
    // 将内核所需要的页表注册进页目录
    for (unsigned i = 0; i < kernelPageDirNum; i++)
        ptd->pde[kernelPDE + i] = PE(PE_PRESENT | PE_WRITE, &ptd->pte[i + 1]);

    // 重映射内核至高半区地址（>=0xC0000000）
    for (unsigned int i = 0; i < kernelPageEntNum; i++)
        ptd->pte[1][kernelPTE + i] = PE(PE_PRESENT | PE_WRITE, kernelPMA + (i << 12));

    // 最后一个entry用于循环映射
    ptd->pde[1023] = PE(PE_PRESENT | PE_WRITE | PE_DISABLE_CACHE, ptd);
}

void HHK_INIT_TEXT hhkInit(void *kPE, unsigned int kPESize) {
    kernelPageInit((KernelPageTable*)kPE);
}
