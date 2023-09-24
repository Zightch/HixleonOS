#pragma once

/**
 * @brief 初始化内核内存, 包括物理页的初始化, 堆区的初始化
 * 
 * @param size: unsigned int 堆区所需页数量
 * @param dramUpper: unsigned int DRAM最大寻址
 * @return bool: true分配成功, false分配失败
 */
bool kernelMemInit(unsigned int num, unsigned int dramUpper);

/**
 * @brief 内核堆区malloc
 * 
 * @param size: unsigned int 堆区所需页数量
 * @return void *: 可用内存起始地址, 如果分配失败, 返回nullptr, 如果成功, 返回 > 0xC0000000的内存地址
 */
void *hlmalloc(unsigned int size);

/**
 * @brief 内核堆区free
 * 
 * @param addr: void *内存地址
 * @return void *: 可用内存起始地址, 如果分配失败, 返回nullptr, 如果成功, 返回 > 0xC0000000的内存地址
 */
bool hlfree(void *addr);