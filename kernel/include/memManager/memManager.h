#pragma once

/**
 * @brief 自动分配堆空间
 * 
 * @param size: unsigned int 页数量
 * @return int: 起始虚拟页号, 如果分配成功会返回0 ~ 1048575之间的合法数字, 如果失败回返回不合法数字比如-1
 */
int allocHeap(unsigned int size);

/**
 * @brief 从指定的起始页号向上分配size个页的堆空间
 * 
 * @param page: unsigned int 起始页号
 * @param size: unsigned int 页数量
 * @return bool: true分配成功, false分配失败
 */
bool allocHeap(unsigned int page, unsigned int size);

/**
 * @brief 从指定的堆向上拓展size个页
 *
 * @param page: unsigned int 堆起始页号
 * @param size: unsigned int 拓展页数量
 * @return bool: true拓展成功, false拓展失败
 */
bool expandHeap(unsigned int page, unsigned int size);

/**
 * @brief 释放指定的堆空间
 *
 * @param page: unsigned int 堆起始页号
 * @return bool: true释放成功, false释放失败
 */
bool freeHeap(unsigned int page);

/**
 * @brief 为指定的堆区进行合并空闲的内存
 *
 * @param page: unsigned int 堆起始页号
 */
void coalesce(unsigned int page);

/**
 * @brief 获取堆大小, 同时判定头尾数据合法性
 *
 * @param page: unsigned int 堆起始页号
 * @return int: 如果获取成功会返回1 ~ 1048576之间的合法长度数字, 如果失败回返回不合法数字比如-1
 */
int heapSize(unsigned int page);

/**
 * @brief 从指定的堆中分配size个字节的内存
 *
 * @param page: unsigned int 堆起始页号
 * @param size: unsigned int 长度
 * @return void *: 内存起始地址
 */
void *kmalloc(unsigned int page, unsigned int size);

/**
 * @brief 从指定的堆中释放已分配的内存
 *
 * @param page: unsigned int 堆起始页号
 * @param addr: void *内存地址
 * @return bool: true释放成功, false释放失败
 */
bool kfree(unsigned int page, void *addr);
