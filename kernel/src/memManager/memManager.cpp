#include "memManager/memManager.h"
#include "memManager/physMem.h"
#include "memManager/virtMem.h"

int heap(unsigned int size) {
    //如果size不合法
    if (size >= 1048576)
        return -1;

    //分配可用的虚拟页和物理页
    int pageStart = VirtMem::getUsablePage(size);//获取连续可用虚拟页的起始页号
    if (0 > pageStart || pageStart > 1048575)//如果虚拟页不合法
        return -1;
    for (int i = 0; i < size; i++) {
        int pp = PhysMem::getUsablePage();//获取一个可用物理页
        PhysMem::setPageUsage(pp, true);//标记为已使用
        if (0 <= pp && pp <= 1048575)//如果物理页合法
            if (VirtMem::map(pageStart + i, pp))//如果映射成功, 就继续执行
                continue;
        //否则说明内存已满
        PhysMem::setPageUsage(pp, false);//还原状态
        for (int j = i - 1; j >= 0; j--) {//释放已经建立映射的页
            int pp = VirtMem::virtPageToPhysPage(pageStart + j);
            VirtMem::unmap(pageStart + j);
            PhysMem::setPageUsage(pp, false);
        }
        return -1;
    }

    //堆区分配完毕, 页表分配成功
    unsigned int *heap = (unsigned int *) (pageStart << 12);//获取地址
    unsigned int heapLast4ByteOffset = (size * 1024) - 1;

    //全填空闲
    //堆区头部数据(高12位固定为0xFFF, 低20位为堆区占用多少个页)
    heap[0] = 0xFFF00000 | size;
    //尾部数据(高12位固定为0xFFF, 低20位为当前堆区向前多少个页为首页)
    heap[heapLast4ByteOffset] = 0xFFF00000 | (-((int) size));

    //接下来是Implicit free list
    unsigned int freeSize = heapLast4ByteOffset - 1;
    heap[1] = freeSize << 2;
    heap[freeSize] = heap[1];

    return pageStart;
}

void *hlmalloc(unsigned int page, unsigned int size) {
    return nullptr;
}

void hlfree(unsigned int page, void *addr) {

}
