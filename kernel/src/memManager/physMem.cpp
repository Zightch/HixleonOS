#include "memManager/physMem.h"

namespace physMem {
    char physPageUsageTable[131072] = {0};//物理页使用情况表(位图)

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
}
