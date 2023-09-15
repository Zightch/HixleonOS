#pragma once

namespace physMem {
    bool addrIsUsing(void *addr);//物理地址地址是否被使用
    bool pageIsUsing(void *page);//物理页是否被使用
    bool setPageUsage(void *page, bool isUsing);//设置物理页使用情况
    bool setSectionPageUsage(void *begin, unsigned int size, bool isUsing);//设置一段物理页使用情况(批量设置)
    bool setSectionPageUsage(void *begin, void *end, bool isUsing);//设置一段物理页使用情况(批量设置), 不包括end所在页
    void *getUsablePage();//获取一个可用页
}
