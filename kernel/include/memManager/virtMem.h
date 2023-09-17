#pragma once

namespace VirtMem {

    /**
     * @brief 虚拟地址是否被使用
     * 
     * @param addr: void* 虚拟地址
     * @return bool: 被占用true, 没有被占用false
     */
    bool addrIsUsing(void *addr);

    /**
     * @brief 虚拟页是否被使用
     * 
     * @param page: unsigned int 虚拟页号
     * @return bool: 被占用true, 没有被占用false
     */
    bool pageIsUsing(unsigned int page);

    /**
     * @brief 虚拟地址转物理地址
     * 
     * @param addr: void* 虚拟地址
     * @return void*: 物理地址, 如果成功返回低12位相同的合法物理地址, 如果失败返回低12位不同的不合法地址
     */
    void *virtAddrToPhysAddr(void *addr);

    /**
     * @brief 虚拟页转物理页
     * 
     * @param page: unsigned int 虚拟页号
     * @return int: 物理页号, 如果成功会返回0 ~ 1048575之间的合法数字, 如果失败回返回不合法数字比如-1
     */
    int virtPageToPhysPage(unsigned int page);

    /**
     * @brief 获取一个可用连续虚拟页
     * 
     * @param num: unsigned int 页数量
     * @return int: 可用起始页号, 如果成功会返回0 ~ 1048575之间的合法数字, 如果失败回返回不合法数字比如-1
     */
    int getUsablePage(unsigned int num = 1);

    /**
     * @brief 映射虚拟页到物理页
     * 
     * @param virtPage: unsigned int 虚拟页号
     * @param physPage: unsigned int 物理页号
     * @return bool: true映射成功, false映射失败
     */
    bool map(unsigned int virtPage, unsigned int physPage);

    /**
     * @brief 解除虚拟页的映射
     * 
     * @param virtPage: unsigned int 虚拟页号
     * @return bool: true解除成功, false解除失败
     */
    bool unmap(unsigned int virtPage);//解除虚拟页的映射
}
