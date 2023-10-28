#include "kernel.h"
#include "tty.h"
#include "GDT.h"
#include "IDT.h"
#include "int/APIC/APIC.h"
#include "ACPI/ACPI.h"
#include "memManager/kernelMem.h"
#include "memManager/virtMem.h"
#include "memManager/physMem.h"
#include "ByteArray/ByteArray.h"
#include "tools/numTools.h"
#include "crash.h"

//为了让通过编译的
extern "C" {
    void __gxx_personality_v0() {}
    void _Unwind_Resume() {}
    void __cxa_throw_bad_array_new_length() {}
}

MemMap *memMap = nullptr;
unsigned short memMapSize = 0xFFFF;
void *kpg = nullptr;
unsigned int kpgSize = -1;

void kernelInit(MemMap *mm, unsigned short ms, void *k, unsigned int ks) {
    initGDT();
    initIDT();
    memMap = mm;
    memMapSize = ms;
    kpg = k;
    kpgSize = ks;
}

void kernelMain() {
    ttyClear();

    //寻找1MB开始的DRAM内存
    unsigned int _1MiBBase = 0x100000;//内存基地址
    unsigned int _1MiBUpper = 0;//最大寻址地址
    unsigned int _1MiBSize = 0;//内存长度
    for (unsigned int i = 0; i < memMapSize; i++) {
        if (memMap[i].base == _1MiBBase) {
            _1MiBUpper = _1MiBSize = memMap[i].size;
            if (memMap[i].size > 0xFFF00000)
                _1MiBUpper = 0xFFF00000;
            _1MiBUpper += 0xFFFFF;
            break;
        }
        //如果未找到, 内存错误
        if (i == memMapSize - 1) {
            crash("Memory error, DRAM memory based on 0x100000 not found\n");
            return;
        }
    }

    //初始化内存
    //kernelEnd开始向上找连续可用的虚拟页空间, 默认分配4个页(16kiB)
    if (!kernelMemInit(4, _1MiBUpper)) {//如果堆区分配失败
        crash("Memory error, Alloc heap memory fail!\n");
        return;
    }
    ACPI::init();//初始化ACPI
    VirtMem::enableNullptr();//启用nullptr指针
    initRefCount();//初始化共享指针的引用计数器

    ByteArray endl("\n", 2);
    auto dramSize = _1MiBSize;
    auto memMapTmp = new MemMap[memMapSize];
    ttyPutStr("Index  Base              Size              Type\n");
    for (unsigned int i = 0; i < memMapSize; i++) {//打印内存地图
        memMapTmp[i] = memMap[i];
        ByteArray index = toByteArray(i, 10, 5, ' ');
        ByteArray base = toByteArray(memMap[i].base, 16, 16);
        ByteArray size = toByteArray(memMap[i].size, 16, 16);
        ByteArray type = toByteArray(memMap[i].type, 16, 8);
        //顺便找到除1MB以外的type==1的内存段将其物理页使用设置为false
        if (memMap[i].base != 0x100000 && memMap[i].type == 1 && memMap[i].base < 0x100000000ull) {
            auto pageNum = (memMap[i].size >> 12);
            if (pageNum > 0x00100000) pageNum = 0x00100000;
            PhysMem::setSectionPageUsage((memMap[i].base + 0xFFF) >> 12, pageNum, false);
            dramSize += (pageNum << 12) - (memMap[i].base & 0xFFF);
        }
        ttyPutStr(index + "  " + base + "  " + size + "  " + type + endl);
    }
    memMap = memMapTmp;//更新memMap存储地址
    float dramSizeGiB = dramSize * 1.0f / 1024.0f / 1024.0f / 1024.0f;
    ttyPutStr("Addressable DRAM size: " + toByteArray(dramSize) + "Byte (about " + toByteArray(dramSizeGiB, 2) + "GiB)" + endl);
    //至此内存管理器全部初始化完成

    //初始化APIC
    APIC::init();
    (*(int *) 0) = 0;
}
