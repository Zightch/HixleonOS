#pragma once

#define IOAPIC_DM_FIXED 0b000
#define IOAPIC_DM_NMI 0b100

#define IOAPIC_IM(isIM) ((isIM << 16) & 0x10000)//是否屏蔽
#define IOPAIC_DELMOD(mod) ((mod << 8) & 0x700)//递送模式
#define IOAPIC_DEST(dest) ((((unsigned long long)dest) << 56) & 0xFF00000000000000ull)//递送目的地
#define IOAPIC_DESTMOD(mod) ((mod << 11) & 0x800)//递送目的地格式

#define IOAPIC_P(pin) ((pin << 13) & 0x2000)//高低电平
#define IOAPIC_T(mode) ((mode << 15) & 0x8000)//触发模式

namespace IOAPIC {
    void init();
    bool setIRQ(unsigned char i, unsigned long long val);
    unsigned long long getIRQ(unsigned char i);
}
