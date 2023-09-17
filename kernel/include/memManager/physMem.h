#pragma once

namespace PhysMem {
    /**
     * @brief 物理地址地址是否被使用
     * 
     * @param addr: void* 物理地址
     * @return bool: true被占用, false没有被占用
     */
    bool addrIsUsing(void *addr);

    /**
     * @brief 物理页是否被使用
     * 
     * @param page: unsigned int 物理页号
     * @return bool: true被占用, false没有被占用
     */
    bool pageIsUsing(unsigned int page);

    /**
     * @brief 设置物理页使用情况
     * 
     * @param page: unsigned int 物理页号
     * @param isUsing: bool 被占用true, 没有被占用false
     * @return bool: true设置成功, false设置失败
     */
    bool setPageUsage(unsigned int page, bool isUsing);

    /**
     * @brief 设置一段物理页使用情况(批量设置)
     * 
     * @param begin: unsigned int 物理起始页号
     * @param size: unsigned int 物理页长度
     * @param isUsing: bool 被占用true, 没有被占用false
     * @return bool: true设置成功, false设置失败
     */
    bool setSectionPageUsage(unsigned int begin, unsigned int size, bool isUsing);

    /**
     * @brief 获取一个可用页
     * 
     * @return int: 物理页号, 如果获取成功会返回0 ~ 1048575之间的合法数字, 如果失败回返回不合法数字比如-1
     */
    int getUsablePage();//获取一个可用页
}
