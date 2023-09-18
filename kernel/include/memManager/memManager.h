#pragma once

/**
 * @brief 分配堆空间
 * 
 * @param size: unsigned int 页数量
 * @return int: 起始虚拟页号, 如果分配成功会返回0 ~ 1048575之间的合法数字, 如果失败回返回不合法数字比如-1
 */
int heap(unsigned int size);

/**
 * @brief 从指定的堆中分配size个byte的内存
 *
 * @param page: unsigned int 堆起始页号
 * @param size: unsigned int 长度
 * @return void *: 内存起始地址
 */
void *hlmalloc(unsigned int page, unsigned int size);

/**
 * @brief 从指定的堆中释放已分配的内存
 *
 * @param page: unsigned int 堆起始页号
 * @param addr: void *内存地址
 */
void hlfree(unsigned int page, void *addr);
