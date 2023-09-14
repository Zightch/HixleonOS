#pragma once

/*
PTD: 寄存器. 用于存储一级页表物理基地址的寄存器
PDE: 一级页表. 也叫页表目录
PTE: 二级页表. 也叫页表表项
*/

#define NOT_ALLOC_DATA(class) \
public: \
class() = delete; \
class(const class &) = delete; \
class &operator=(const class &) = delete; \
private: \

#define PE_MAX_NUM 1024
#define PTE_MAX_NUM 5

namespace HHKInit {
#pragma pack(push, 1)

    class PageTable {
        NOT_ALLOC_DATA(PageTable)
    public:
        unsigned int pde[PE_MAX_NUM];//页目录项
        unsigned int pte[PTE_MAX_NUM][PE_MAX_NUM];//页表表项
    };

#pragma pack(pop)
}

#define HHK_INIT_TEXT __attribute__((section(".hhkInitText")))
#define HHK_INIT_BSS __attribute__((section(".hhkInitBss")))

#define PE(flags, peAddr)     (((unsigned int)(peAddr) & 0xFFFFF000) | ((flags) & 0xfff))

// 页表低12位flags
#define PG_PRESENT        1
#define PG_WRITE          (1 << 1)
#define PG_ALLOW_USER     (1 << 2)
#define PG_WRITE_THROUGHT (1 << 3)
#define PG_DISABLE_CACHE  (1 << 4)
#define PG_PDE_4MB        (1 << 7)

#define GET_LD_DATA(var) ((unsigned int)&var)

extern unsigned int hhkInitEnd;
extern unsigned int kernelStart;
extern unsigned int kernelEnd;
extern unsigned int heapStart;
