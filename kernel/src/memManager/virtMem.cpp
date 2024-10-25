#include "memManager/virtMem.h"
#include "memManager/physMem.h"
#include "page.h"
#include "cpu.h"

namespace VirtMem {
    unsigned int usablePageIndexLast = 0;//可用页上一次查询的下标(具体页号)
    bool upiDir = false;//可用页下标迭代方向, false: 向上, true: 向下
    bool null = false;
    /*根据循环映射, 0xFFFFF000地址为PDE地址, 长度1024
      根据循环映射, 0xFFC00000地址为第一个PTD地址, 长度1024
        同理, 0xFFC01000为第二个PTD地址, 长度为1024, 以此类推
        直到0xFFFFE000, 因为到0xFFFFF000就变成PDE的地址了
    */
    //获取pde元素
    unsigned int pdeEntry(unsigned short pde) {
        if (pde < 1024)
            return ((unsigned int *) 0xFFFFF000)[pde];
        return 0xFFFFFFFE;
    }

    //获取pte元素
    unsigned int pteEntry(unsigned short pde, unsigned short pte) {
        if (pde < 1023 && pte < 1024 && (((unsigned int *) 0xFFFFF000)[pde] & 1)) {
            unsigned int ptBase = 0xFFC00000;
            ptBase |= (pde << 12) & 0x003FF000;
            return ((unsigned int *) ptBase)[pte];
        }
        return 0xFFFFFFFE;
    }

    //设置pde元素
    bool setPDEEntry(unsigned short pde, unsigned int val) {
        if (pde < 1024) {
            ((unsigned int *) 0xFFFFF000)[pde] = val;
            return true;
        }
        return false;
    }

    //设置pte元素
    bool setPTEEntry(unsigned short pde, unsigned short pte, unsigned int val) {
        if (pde < 1023 && pte < 1024 && (((unsigned int *) 0xFFFFF000)[pde] & 1)) {
            unsigned int ptBase = 0xFFC00000;
            ptBase |= (pde << 12) & 0x003FF000;
            ((unsigned int *) ptBase)[pte] = val;
            return true;
        }
        return false;
    }

    //虚拟地址地址是否被使用
    bool addrIsUsing(void *addr) {
        unsigned int page = (((unsigned int) addr) >> 12) & 0x000FFFFF;
        return pageIsUsing(page);
    }

    //虚拟页是否被使用
    bool pageIsUsing(unsigned int page) {
        if (page >= 1048576)
            return true;
        if (null && page == 0)
            return true;
        unsigned short pdeIndex = ((page) & 0x000FFC00) >> 10;
        unsigned short pteIndex = ((page) & 0x000003FF);
        if ((pdeEntry(pdeIndex) & 1) == 0)
            return false;
        if ((pteEntry(pdeIndex, pteIndex) & 1) == 0)
            return false;
        return true;
    }

    //虚拟地址转物理地址
    void *virtAddrToPhysAddr(void *addr) {
        //获取虚拟地址的高20位, 作为虚拟页号
        unsigned int page = (((unsigned int) addr) >> 12) & 0x000FFFFF;
        //获取虚拟地址的低12位, 作为页内偏移
        unsigned int pageOffset = ((unsigned int) addr) & 0x00000FFF;
        //根据虚拟页号, 获取对应的物理页号
        int physPageNum = virtPageToPhysPage(page);
        //如果物理页号合法, 说明存在映射关系
        if (0 <= physPageNum && physPageNum <= 1048575) {
            //将物理页号左移12位, 得到物理页的基地址
            unsigned int physPageBaseAddr = physPageNum << 12;
            //将物理页的基地址和页内偏移相加, 得到物理地址
            unsigned int physAddr = physPageBaseAddr + pageOffset;
            //返回物理地址
            return (void *) physAddr;
        } else {
            //如果物理页号为-1, 说明不存在映射关系, 返回不合法的数据
            if (pageOffset != 0)return nullptr;
            return (void *) 0x00000FFF;
        }
    }

    //虚拟页转物理页
    int virtPageToPhysPage(unsigned int page) {
        //获取虚拟页号
        auto pageNum = (unsigned int) page;
        //检查虚拟页号是否在合法范围内
        if (pageNum >= 1048576)//如果虚拟页号不在合法范围内, 返回-1
            return -1;

        //计算页目录表和页表的索引
        unsigned short pdeIndex = ((pageNum) & 0x000FFC00) >> 10;
        unsigned short pteIndex = ((pageNum) & 0x000003FF);
        //获取对应的页目录项和页表项的值
        unsigned int pdeValue = pdeEntry(pdeIndex);
        unsigned int pteValue = pteEntry(pdeIndex, pteIndex);
        //检查页目录项和页表项是否有效(存在位是否为1)
        if (((pdeValue & 1) == 0) || ((pteValue & 1) == 0))
            //如果无效, 返回-1
            return -1;
        //如果有效, 获取页表项的高20位, 作为物理页号
        unsigned int physPageNum = (pteValue >> 12) & 0x000FFFFF;
        //返回物理页号
        return (int)physPageNum;
    }

    //映射虚拟页到物理页
    bool map(unsigned int virtPage, unsigned int physPage) {
        //检查虚拟页号与物理页号是否在合法范围内
        if (virtPage >= 1048576 || physPage >= 1048576)
            return false;
        if (null && virtPage == 0)
            return false;

        //计算页目录表和页表的索引
        unsigned short pdeIndex = ((virtPage) & 0x000FFC00) >> 10;
        unsigned short pteIndex = ((virtPage) & 0x000003FF);
        if ((pdeEntry(pdeIndex) & 1) == 0) {//如果页目录未被使用
            int physPTE = PhysMem::getUsablePage();//获取一个可用物理页
            if (physPTE < 0 || physPage > 1048575)//内存已满
                return false;
            PhysMem::setPageUsage(physPage, true);//标记为已使用
            setPDEEntry(pdeIndex, PE(PE_PRESENT | PE_WRITE, (physPTE << 12)));//设置PDE的页表物理地址
            for (unsigned int i = 0; i < 1024; i++)
                setPTEEntry(pdeIndex, i, 0);//清空新分配的表, 防止有垃圾
        }
        setPTEEntry(pdeIndex, pteIndex, PE(PE_PRESENT | PE_WRITE, physPage << 12));//建立映射关系
        CPU::invplg(virtPage);//刷新TLB
        return true;
    }

    //获取连续可用虚拟页
    int getUsablePage(unsigned int num) {
        //检查参数是否合法
        if (num == 0 || num > 1048576)
            return -1;
        //定义连续可用虚拟页的数量
        unsigned int count = 0;
        //定义连续可用虚拟页的基页号
        unsigned int base = 0;
        //定义当前查找的虚拟页号
        int tmp = (int)usablePageIndexLast;
        //定义是否已经改变过一次迭代方向
        bool isSecond = false;
        //循环查找连续可用虚拟页
        while (true) {
            //检查当前查找的虚拟页号是否在合法范围内
            if (tmp >= 1048576 || tmp < 0) {
                //如果不在合法范围内
                if (isSecond)
                    //如果已经改变过迭代方向, 说明没有找到任何连续可用虚拟页, 返回-1
                    return -1;
                //如果没有改变过迭代方向, 改变迭代方向, 并回到上一次查询的虚拟页号
                upiDir = !upiDir;
                tmp = (int)usablePageIndexLast;
                isSecond = true;
                continue;
            }
            //检查当前查找的虚拟页是否未被使用
            if (!pageIsUsing(tmp)) {
                //如果未被使用
                if (count == 0 || upiDir)
                    //如果连续可用虚拟页的数量为0, 将基页号设置为当前查找的虚拟页号
                    base = tmp;
                //将连续可用虚拟页的数量增加1
                count++;
                //如果连续可用虚拟页的数量等于参数指定的数量, 返回基页号
                if (count == num)
                    return (int)base;
            } else {
                //如果已被使用, 将连续可用虚拟页的数量和基页号重置为0
                count = 0;
                base = 0;
            }
            //根据迭代方向移动当前查找的虚拟页号
            upiDir ? tmp-- : tmp++;
        }
    }

    //解除虚拟页的映射
    bool unmap(unsigned int virtPage) {
        //检查虚拟页号是否在合法范围内
        if (virtPage >= 1048576)
            return false;
        //计算页目录表和页表的索引
        unsigned short pdeIndex = ((virtPage) & 0x000FFC00) >> 10;
        unsigned short pteIndex = ((virtPage) & 0x000003FF);
        if ((pdeEntry(pdeIndex) & 1) == 0)//如果是无效页表
            return false;
        setPTEEntry(pdeIndex, pteIndex, 0);
        unsigned short i;
        for (i = 0; i < 1024; i++)
            if (pteEntry(pdeIndex, i) & 1)break;
        if (i == 1024) {//如果是个空PTE
            unsigned int physPTE = (pdeEntry(pdeIndex) >> 12) & 0x000FFFFF;
            PhysMem::setPageUsage(physPTE, false);//归还PTE空间
            setPDEEntry(pdeIndex, 0);//解除PDE对他的引用
        }
        CPU::invplg(virtPage);//刷新TLB
        return true;
    }

    void enableNullptr() {
        unmap(0);
        null = true;
    }
}
