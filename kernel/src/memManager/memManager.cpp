#include "memManager/memManager.h"
#include "memManager/physMem.h"
#include "memManager/virtMem.h"

/*
堆区内存布局
                   31  20 19 <----------> 0 位
                   +-----+----------------+
              [0]: | FFF |      size      |
                   +-----+----------------+
                   |                      |
                   |        可用区        |
                   |                      |
                   +-----+----------------+
[size * 1024 - 1]: | FFF |     -size      |
                   +-----+----------------+
size: 当前堆区所占页数量, 单位: 页

Implicit free list布局
31 <------------> 1  0 位
+------------------+---+  ----
|       size       | a |   ^
+------------------+---+   |
|                      |   |
|       有效负载       |  size
|                      |   |
+------------------+---+   |
|       size       | a |   v
+------------------+---+  ----
a: 1为已被占用, 0为未被占用
size: 4字节对齐, 也就是低2位总为0(第0位被a占用)
*/

int allocHeap(unsigned int size) {
    //如果size不合法
    if (size > 1048575)
        return -1;

    //分配可用的虚拟页和物理页
    int pageStart = VirtMem::getUsablePage(size);//获取连续可用虚拟页的起始页号
    if (0 > pageStart || pageStart > 1048575)//如果虚拟页不合法
        return -1;

    if(allocHeap(pageStart, size))
        return pageStart;
    return -1;
}

bool allocHeap(unsigned int pageStart, unsigned int size) {
    //如果size和pageStart有任意一个不合法
    if (size > 1048575 || pageStart > 1048575)
        return false;

    {//验证给定的空间是否可用
        unsigned int heapNum = 0;
        while (heapNum < size) {
            if (VirtMem::pageIsUsing(pageStart))
                return false;
            heapNum++;
        }
    }

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
        return false;
    }

    //堆区分配完毕, 页表分配成功
    unsigned int *heap = (unsigned int *) (pageStart << 12);//获取地址
    unsigned int uint32Num = size * 1024;

    //全填空闲
    //堆区头部数据(高12位固定为0xFFF, 低20位为堆区占用多少个页)
    heap[0] = 0xFFF00000 | size;
    //尾部数据(高12位固定为0xFFF, 低20位为当前堆区向前多少个页为首页)
    heap[uint32Num - 1] = 0xFFF00000 | (-((int) size));

    //接下来是Implicit free list
    unsigned int freeSize = uint32Num - 2;
    heap[1] = freeSize << 2;
    heap[freeSize] = heap[1];

    return true;
}

void *hlmalloc(unsigned int page, unsigned int size) {
    return nullptr;
}

void hlfree(unsigned int page, void *addr) {

}

bool expandHeap(unsigned int page, unsigned int size) {
    //如果size和page有任意一个不合法
    if (size > 1048575 || page > 1048575)
        return false;

    unsigned int *addr = (unsigned int *) (page << 12);//获取地址

    //检查合法性
    unsigned int head = addr[0];//获取堆区头部数据
    if (((head >> 20) & 0x0FFF) != 0xFFF)//验证其高12位合法性
        return false;
    int pageNum = head & 0x000FFFFF;//获取该堆所占页数量
    unsigned int oldUint32Num = pageNum * 1024;//拼接前堆中以4个字节为一个单位有多少个元素
    unsigned int tail = addr[oldUint32Num - 1];//获取尾部数据
    if (((tail >> 20) & 0x0FFF) != 0xFFF)//验证其高12位合法性
        return false;
    int tailPageNum = tail & 0x000FFFFF;
    if (((-pageNum) & 0x000FFFFF) != tailPageNum)
        return false;

    //开始分配
    unsigned int newHeap = page + pageNum;//获取新堆起始页号
    if (!allocHeap(page + pageNum, size))//向后分配连续的堆不成功
        return false;
    //开始拼接新的堆
    unsigned int total = size + pageNum;//计算合并后的堆所占页大小
    unsigned int totalUint32Num = total * 1024;//拼接后堆中以4个字节为一个单位有多少个元素
    addr[0] = 0xFFF00000 | total;//修改堆头部
    addr[totalUint32Num - 1] = 0xFFF00000 | (-((int) total));//修改堆尾部

    //接下来拼接Implicit free list
    /*
        内存布局示意图
           | <----- old -----> | <--- new ---> |
           ^                  ^~~~~~~~~~~~~~~~~^
        head                  free             tail
    */
    unsigned int newUint32Num = size * 1024;//新分配的堆中以4个字节为一个单位有多少个元素
    addr[oldUint32Num - 1] = newUint32Num << 2;//所以这里要把旧堆的尾部改成空闲区的头部
    addr[totalUint32Num - 2] = addr[oldUint32Num - 1];//更新空闲区尾部

    coalesce(page);//合并空闲内存

    return true;//完成拓展
}

void freeHeap(unsigned int page) {

}

void coalesce(unsigned int page) {
    //检查页号是否合法
    if (page > 1048575)
        return;
    unsigned int *addr = (unsigned int *) (page << 12);//获取地址
    //检查合法性
    unsigned int head = addr[0];//获取堆区头部数据
    if (((head >> 20) & 0x0FFF) != 0xFFF)//验证其高12位合法性
        return;
    int pageNum = head & 0x000FFFFF;//获取该堆所占页数量
    unsigned int uint32Num = pageNum * 1024;//堆中以4个字节为一个单位有多少个元素
    unsigned int tail = addr[uint32Num - 1];//获取尾部数据
    if (((tail >> 20) & 0x0FFF) != 0xFFF)//验证其高12位合法性
        return;
    int tailPageNum = tail & 0x000FFFFF;
    if (((-pageNum) & 0x000FFFFF) != tailPageNum)
        return;

    //开始遍历Implicit free list
    unsigned int i = 1;//当前遍历到的位置
    while (i < uint32Num - 1) {//如果没有到达尾部
        unsigned int size = addr[i] >> 2;//获取当前块的大小
        bool a = addr[i] & 1;//获取当前块的有效位
        if (!a) {//如果当前块是空闲的
            unsigned int next = i + size;//获取下一个块的位置
            if (next < uint32Num - 1) {//如果没有越界
                unsigned int nextSize = addr[next] >> 2;//获取下一个块的大小
                bool nextA = addr[next] & 1;//获取下一个块的有效位
                if (!nextA) {//如果下一个块也是空闲的
                    //合并两个空闲块, 更新头部和尾部的大小和有效位
                    unsigned int newSize = size + nextSize;
                    addr[i] = (newSize << 2);
                    addr[i + newSize - 1] = addr[i];
                    //继续遍历, 不需要移动i的位置
                    continue;
                }
            }
        }
        //如果当前块是占用的或者不能合并, 移动i到下一个块的位置
        i += size;
    }
}
