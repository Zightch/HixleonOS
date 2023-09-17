#pragma once

/**
 * @brief 分配堆空间
 * 
 * @param size: unsigned int 页数量
 * @return int: 起始虚拟页号, 如果分配成功会返回0 ~ 1048575之间的合法数字, 如果失败回返回不合法数字比如-1
 */
int heap(unsigned int size);



void *malloc(unsigned int size);

void free(void *);
