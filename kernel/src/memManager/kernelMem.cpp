#include "memManager/kernelMem.h"

#include "ld.h"
#include "memManager/physMem.h"
#include "memManager/virtMem.h"
#include "memManager/memManager.h"

int kernelHeap = -1;//内核堆区起始页号

bool kernelMemInit(unsigned int num, unsigned int dramUpper) {
    if (kernelHeap != -1)
        return false;

    //开始标记已使用的物理内存页
    //0 ~ 0xFFFFF(1MB, 1048576B)已使用
    PhysMem::setSectionPageUsage(0, 256, true);
    //0x100000 ~ kernelEnd 已使用
    unsigned int physKernelEndPage = (GET_LD_DATA(kernelEnd) - 0xC0000000) >> 12;
    PhysMem::setSectionPageUsage(256, physKernelEndPage - 256, true);
    //除去DRAM以外的其他内存区域标记为已使用(其实为不可用)
    unsigned int unusable = (dramUpper + 1) >> 12;
    PhysMem::setSectionPageUsage(unusable, 1048576 - unusable, true);

    //对等映射0xFEE00000内存(为APIC使用)
    VirtMem::map(0xFEE00, 0xFEE00);

    //如果num不合法
    if (num >= 1048576)
        return false;

    //初始化堆区
    //kernelEnd开始向上找连续可用的虚拟页空间
    kernelHeap = GET_LD_DATA(kernelEnd) >> 12;//虚拟kernel结束页号
    {
        unsigned int heapNum = 0;
        while (heapNum < num) {
            if (kernelHeap >= 1048576)
                return false;
            if (!VirtMem::pageIsUsing(kernelHeap))
                heapNum++;
            else {
                kernelHeap += heapNum;//跳过无法连续的区域
                heapNum = 0;
            }
        }
    }

    return allocHeap(kernelHeap, num);
}

void *hlmalloc(unsigned int size) {
    if (size == 0)
        return nullptr;

    void *addr = kmalloc(kernelHeap, size);//尝试分配内存
    if (addr == nullptr) {//如果失败
        unsigned int pageNum = (size + 0xFFF) >> 12;//获取size所占页数量
        if (!expandHeap(kernelHeap, pageNum))//尝试拓展堆空间
            return nullptr;//拓展失败
        addr = kmalloc(kernelHeap, size);//再次尝试分配内存
    }
    return addr;//返回地址
}

bool hlfree(void *addr) {
    return kfree(kernelHeap, addr);
}

void *operator new(long unsigned int size) {
    return hlmalloc(size);
}

void operator delete(void *addr, long unsigned int) {
    hlfree(addr);
}

void *operator new[](long unsigned int size) {
    return hlmalloc(size);
}

void operator delete[](void *addr) {
    hlfree(addr);
}
