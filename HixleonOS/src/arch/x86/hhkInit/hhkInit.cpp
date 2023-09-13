#include "arch/x86/hhkInit/hhkInit.h"

namespace HHKInit {
    void HHK_INIT_TEXT

    hhkInit(PageTable *ptd, unsigned int kpgSize) {
        // 对低1MiB空间进行对等映射
        // 设置一级页表(页目录)
        ptd->pde[0] = PE(PG_PRESENT, &ptd->pte[0]);

        for (unsigned int i = 0; i < 256; i++)
            ptd->pte[0][i] = PE(PG_PRESENT | PG_WRITE, (i << 12));

        // 对hhkInit空间行进对等映射
        unsigned int hhkInitPageNum = (GET_LD_DATA(hhkInitEnd) - 0x100000 + 0x1000 - 1) >> 12;//hhk初始化的代码数据所占页大小
        for (unsigned int i = 0; i < hhkInitPageNum; i++)
            ptd->pte[0][256 + i] = PE(PG_PRESENT | PG_WRITE, 0x100000 + (i << 12));

        // 开始映射高半核区域
        unsigned int kernelPDE = (GET_LD_DATA(kernelStart) & 0xFFC00000UL) >> 22;//高半核所在页目录项
        unsigned int kernelPTE = (GET_LD_DATA(kernelStart) & 0x003FF000UL) >> 12;//高半核所在页表表项
        unsigned int kernelPageNum = (GET_LD_DATA(kernelEnd) - GET_LD_DATA(kernelStart) + 0x1000 - 1) >> 12;//高半核所占页大小
        unsigned int kernelPageDirNum = (kernelPageNum + 1024 - 1) >> 10;//kernel所占页目录数量
        unsigned int kernelPMA = GET_LD_DATA(kernelStart) - 0xC0000000;//高半核物理地址
        // 将内核所需要的页表注册进页目录
        for (unsigned i = 0; i < kernelPageDirNum; i++)
            ptd->pde[kernelPDE + i] = PE(PG_PRESENT | PG_WRITE, &ptd->pte[i + 1]);

        // 重映射内核至高半区地址（>=0xC0000000）
        for (unsigned int i = 0; i < kernelPageNum; i++)
            ptd->pte[1][kernelPTE + i] = PE(PG_PRESENT | PG_WRITE, kernelPMA + (i << 12));

        // 最后一个entry用于循环映射
        ptd->pde[1023] = PE(PG_PRESENT | PG_WRITE | PG_DISABLE_CACHE, ptd);
    }
}
