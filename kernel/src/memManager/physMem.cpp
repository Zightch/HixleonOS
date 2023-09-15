#include "memManager/physMem.h"

namespace physMem {
    char physPageUsageTable[131072];//物理页使用情况表(位图)

    //物理地址地址是否被使用
    bool addrIsUsing(void *addr) {
        unsigned int pageBaseAddr = (((unsigned int)addr) >> 12) & 0x000FFFFF;
        return pageIsUsing((void*)pageBaseAddr);
    }
    //物理页是否被使用
    bool pageIsUsing(void *page) {
        unsigned int pageNum = (unsigned int)page;
        if (pageNum < 1048576) {
            unsigned int usageTableEntry = (pageNum >> 3) & 0x0001FFFF;
            unsigned char entryOffset = pageNum & 7;
            return ((physPageUsageTable[usageTableEntry] >> entryOffset) & 1);
        }
        return true;
    }

    //设置物理页使用情况
    bool setPageUsage(void *page, bool isUsing) {
        unsigned int pageNum = (unsigned int)page;
        if (pageNum < 1048576) {
            unsigned int usageTableEntry = (pageNum >> 3) & 0x0001FFFF;
            unsigned char entryOffset = pageNum & 7;
            unsigned char tmp = isUsing;
            tmp <<= entryOffset;
            physPageUsageTable[usageTableEntry] |= tmp;
            return true;
        }
        return false;
    }
    //设置物理地址使用情况
    bool setAddrUsage(void *addr, bool isUsing) {
        unsigned int pageBaseAddr = (((unsigned int)addr) >> 12) & 0x000FFFFF;
        return setPageUsage((void*)pageBaseAddr, isUsing);
    }
}
