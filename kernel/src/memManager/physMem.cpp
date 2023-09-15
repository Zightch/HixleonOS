#include "memManager/physMem.h"

namespace physMem {
    char physPageUsageTable[131072] = {0};//物理页使用情况表(位图,  一位表示一页的占用情况)
    unsigned int usablePageIndexLast = 0;//可用页上一次查询的下标(20位)
    bool UPIDir = false;//可用页下标迭代方向, false: 向上, true: 向下

    bool setBit(char &ch, unsigned char bit, bool tf) {
        if (0 <= bit && bit <= 7) {
            unsigned char tmp = tf;
            tmp <<= bit;
            ch = tf ? (ch | tmp) : (ch & (~tmp));
            return true;
        }
        return false;
    }

    //物理地址地址是否被使用
    bool addrIsUsing(void *addr) {
        unsigned int pageBaseAddr = (((unsigned int) addr) >> 12) & 0x000FFFFF;
        return pageIsUsing((void *) pageBaseAddr);
    }

    //物理页是否被使用
    bool pageIsUsing(void *page) {
        unsigned int pageNum = (unsigned int) page;
        if (pageNum < 1048576) {
            unsigned int usageTableEntry = (pageNum >> 3) & 0x0001FFFF;
            unsigned char entryOffset = pageNum & 7;
            return ((physPageUsageTable[usageTableEntry] >> entryOffset) & 1);
        }
        return true;
    }

    //设置物理页使用情况
    bool setPageUsage(void *page, bool isUsing) {
        unsigned int pageNum = (unsigned int) page;
        if (pageNum < 1048576) {
            unsigned int entryIndex = (pageNum >> 3) & 0x0001FFFF;
            unsigned char entryOffset = pageNum & 7;
            setBit(physPageUsageTable[entryIndex], entryOffset, isUsing);
            return true;
        }
        return false;
    }

    //设置一段物理页使用情况(批量设置)
    bool setSectionPageUsage(void *begin, unsigned int size, bool isUsing) {
        unsigned int pageBegin = (unsigned int) begin;
        if (pageBegin < 1048576 && pageBegin + size <= 1048576) {
            unsigned int entryIndex = (pageBegin >> 3) & 0x0001FFFF;
            unsigned char entryOffset = pageBegin & 7;

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
        return false;
    }

    //设置一段物理页使用情况(批量设置), 不包括end所在页
    bool setSectionPageUsage(void *begin, void *end, bool isUsing) {
        unsigned int pageBegin = (unsigned int) begin;
        unsigned int pageEnd = (unsigned int) end;
        return setSectionPageUsage(begin, pageEnd - pageBegin, isUsing);
    }

    //获取一个可用页
    void *getUsablePage() {
        //保存上一次查询的下标
        int tmp = usablePageIndexLast >> 3;
        bool isSecond = false;//用于表示本次查找中第二次改变查询方向
        //循环查找可用页
        while (true) {
            //如果当前4个字节都是0xFF, 说明这32个页都被占用, 跳过这32个页
            if (*((int *) &physPageUsageTable[tmp]) == -1) {
                UPIDir ? tmp += 4 : tmp -= 4;
                //再次检查下标是否在合法范围内
                if (tmp >= 131072 || tmp < 0) {
                    if (!isSecond) {
                        //如果不在合法范围内, 改变迭代方向, 并回到上一次查询的下标
                        UPIDir = !UPIDir;
                        tmp = usablePageIndexLast;
                        isSecond = true;
                    } else return nullptr;//返回nullptr
                }
            } else {
                //如果当前4个字节不都是0xFF, 说明有至少一个可用页, 遍历这32个页找到它
                for (unsigned char i = 0; i < 32; i++) {
                    //计算当前页号
                    unsigned int pageNum = ((tmp << 3) + i) & 0x000FFFFF;
                    //如果当前页未被占用, 返回它, 并更新usablePageIndexLast的值
                    if (!pageIsUsing((void *) pageNum)) {
                        usablePageIndexLast = (tmp << 3) + i;
                        return (void *) (pageNum << 12);
                    }
                }
            }
        }
    }
}
