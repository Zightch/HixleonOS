#include "interrupts/IOAPIC.h"
#include "memManager/virtMem.h"

namespace IOAPIC {
    unsigned char xy = 0x00;

    unsigned char IRQLowIndex[24] = {
            0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E,
            0x20, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E,
            0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E
    };
    unsigned char IRQHighIndex[24] = {
            0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F,
            0x21, 0x23, 0x25, 0x27, 0x29, 0x2B, 0x2D, 0x2F,
            0x31, 0x33, 0x35, 0x37, 0x39, 0x3B, 0x3D, 0x3F
    };

    unsigned char &index() {
        return (*(unsigned char *) (0xFEC00000 | (((unsigned int) xy) << 8)));
    }

    unsigned int &data() {
        return (*(unsigned int *) (0xFEC00010 | (((unsigned int) xy) << 8)));
    }

    void init() {
        //获取PIIX3中xy的值
        //对等映射IOAPIC内存
    }

    bool setIRQ(unsigned char i, unsigned long long val) {
        if (i > 23)
            return false;
        index() = IRQLowIndex[i];
        data() = (unsigned int) val;
        index() = IRQHighIndex[i];
        data() = (unsigned int) (val >> 32);
        return true;
    }

    unsigned long long getIRQ(unsigned char i) {
        if (i > 23)
            return 0;
        unsigned long long tmp;
        index() = IRQHighIndex[i];
        tmp = data();
        tmp <<= 32;
        index() = IRQLowIndex[i];
        tmp |= data();
        return tmp;
    }
}
