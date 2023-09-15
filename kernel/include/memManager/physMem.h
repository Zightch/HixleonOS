#pragma once

namespace physMem {
    bool addrIsUsing(void *addr);//物理地址地址是否被使用
    bool pageIsUsing(void *page);//物理页是否被使用
    bool setPageUsage(void *page, bool isUsing);//设置物理页使用情况
    bool setAddrUsage(void *addr, bool isUsing);//设置物理地址使用情况
}
