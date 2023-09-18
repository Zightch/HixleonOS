#include "memManager/physMem.h"

namespace PhysMem {
    char physPageUsageTable[131072] = {0};//物理页使用情况表(位图,  一位表示一页的占用情况)
    unsigned int usablePageIndexLast = 0;//可用页上一次查询的下标(20位)
    bool upiDir = false;//可用页下标迭代方向, false: 向上, true: 向下

    bool setBit(char &ch, unsigned char bit, bool tf) {
        if (bit > 7)
            return false;
        unsigned char tmp = 1;
        tmp <<= bit;
        ch = tf ? (ch | tmp) : (ch & (~tmp));
        return true;
    }

    //物理地址地址是否被使用
    bool addrIsUsing(void *addr) {
        unsigned int pageBaseAddr = (((unsigned int) addr) >> 12) & 0x000FFFFF;
        return pageIsUsing(pageBaseAddr);
    }

    //物理页是否被使用
    bool pageIsUsing(unsigned int page) {
        if (page >= 1048576)
            return true;
        unsigned int usageTableEntry = (page >> 3) & 0x0001FFFF;
        unsigned char entryOffset = page & 7;
        return ((physPageUsageTable[usageTableEntry] >> entryOffset) & 1);
    }

    //设置物理页使用情况
    bool setPageUsage(unsigned int page, bool isUsing) {
        if (page >= 1048576)
            return false;
        unsigned int entryIndex = (page >> 3) & 0x0001FFFF;
        unsigned char entryOffset = page & 7;
        setBit(physPageUsageTable[entryIndex], entryOffset, isUsing);
        return true;
    }

    //设置一段物理页使用情况(批量设置)
    bool setSectionPageUsage(unsigned int begin, unsigned int size, bool isUsing) {
        if (!(begin < 1048576 && begin + size <= 1048576))
            return false;
        unsigned int entryIndex = (begin >> 3) & 0x0001FFFF;
        unsigned char entryOffset = begin & 7;

        for (unsigned char i = entryOffset; i < 8; i++)
            setBit(physPageUsageTable[entryIndex], i, isUsing);

        size -= (8 - entryOffset);
        entryIndex++;
        unsigned int sectionNum = (size >> 3) + entryIndex;

        for (unsigned int i = entryIndex; i < sectionNum; i++)
            physPageUsageTable[i] = isUsing ? 0xFF : 0x00;

        size -= ((sectionNum - entryIndex) << 3);
        entryIndex = sectionNum;

        for (unsigned char i = 0; i < size; i++)
            setBit(physPageUsageTable[entryIndex], i, isUsing);

        return true;
    }

    //获取一个可用页
    int getUsablePage() {
        //保存上一次查询的下标
        int tmp = ((usablePageIndexLast) >> 3) & 0x0001FFF8;
        bool isSecond = false;//用于表示本次查找中第二次改变查询方向
        //循环查找可用页
        while (true) {
            //如果当前8个字节都是0xFF, 说明这64个页都被占用, 跳过这64个页
            if (*((unsigned long long *) &physPageUsageTable[tmp]) == 0xFFFFFFFFFFFFFFFF) {
                upiDir ? tmp -= 8 : tmp += 8;
                //再次检查下标是否在合法范围内
                if (tmp >= 131072 || tmp < 0) {
                    if (!isSecond) {
                        //如果不在合法范围内, 改变迭代方向, 并回到上一次查询的下标
                        upiDir = !upiDir;
                        tmp = ((usablePageIndexLast) >> 3) & 0x0001FFF8;
                        isSecond = true;
                    } else return -1;//返回-1
                }
            } else {
                //如果当前8个字节不都是0xFF, 说明有至少一个可用页, 遍历这64个页找到它
                for (unsigned char i = 0; i < 64; i++) {
                    //计算当前页号
                    unsigned int pageNum = ((tmp << 3) + i) & 0x000FFFFF;
                    //如果当前页未被占用, 返回它, 并更新usablePageIndexLast的值
                    if (!pageIsUsing(pageNum)) {
                        usablePageIndexLast = pageNum;
                        return pageNum;
                    }
                }
            }
        }
    }
}
