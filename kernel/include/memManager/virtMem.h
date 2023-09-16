#pragma once

namespace VirtMem {
    bool addrIsUsing(void *addr);//虚拟地址是否被使用
    bool pageIsUsing(void *page);//虚拟页是否被使用
    void *virtAddrToPhysAddr(void *addr);//虚拟地址转物理地址
    void *virtPageToPhysPage(void *page);//虚拟页转物理页
    unsigned int getUsablePage(unsigned int num = 1);//获取一个可用连续虚拟页
    bool map(void *virtPage, void *physPage);//映射虚拟页到物理页
    bool umap(void *virtPage);//解除虚拟页的映射
}
