#include "memManager/memManager.h"
#include "memManager/physMem.h"
#include "memManager/virtMem.h"

int heap(unsigned int size) {
    //分配可用的虚拟页和物理页
    int pageStart = VirtMem::getUsablePage(size);//获取连续可用虚拟页的起始页号
    if (!(0 <= pageStart && pageStart <= 1048575))//如果虚拟页不合法
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
    return pageStart;
}

void *malloc(unsigned int size) {
    if (size == 0)return nullptr;

    unsigned int pageNum = (size + 0xFFF) >> 12;//获取所用的页数量(向上取整)
    int pageStart = heap(pageNum);

    if (!(0 <= pageStart && pageStart <= 1048575))
        return nullptr;

    //页表分配成功, 开始规划Implicit free list
    unsigned int *alloc = (unsigned int *) (pageStart << 12);//获取内存地址
    unsigned int realLoadSize = (((size + 3) & 0xFFFFFFFC) + 4) >> 2;//向上4字节对齐(包含头尾部数据)
    alloc[0] = (realLoadSize << 2) | 1;//填入头部信息

    //接下来是空闲部分
    unsigned int allocSize = pageNum << 10;//获取分配的总大小
    alloc[realLoadSize] = (allocSize - realLoadSize) << 2;//空闲头部
    alloc[allocSize - 1] = alloc[realLoadSize];//空闲尾部=空闲头部
    return ((char *) alloc) + 4;//返回可用空间首地址
}

void free(void *) {

}
