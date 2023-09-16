#pragma once

/*
PTD: 寄存器. 用于存储一级页表物理基地址的寄存器
PDE: 一级页表. 也叫页表目录
PTE: 二级页表. 也叫页表表项
*/

#define PE(flags, peAddr)     (((unsigned int)(peAddr) & 0xFFFFF000) | ((flags) & 0xfff))

// 页表低12位flags
#define PE_PRESENT        1
#define PE_WRITE          (1 << 1)
#define PE_ALLOW_USER     (1 << 2)
#define PE_WRITE_THROUGHT (1 << 3)
#define PE_DISABLE_CACHE  (1 << 4)
#define PE_PDE_4MB        (1 << 7)
