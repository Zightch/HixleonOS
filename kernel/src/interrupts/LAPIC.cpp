#include "interrupts/LAPIC.h"

#include "memManager/virtMem.h"
#include "interrupts/fixedIntNum.h"

namespace LAPIC {
    unsigned int errAddrReg;

    unsigned int &reg(unsigned short addr) {
        if (addr <= 0x0FFF)
            return *(unsigned int *) (0x1000 + addr);
        return errAddrReg;
    }

    void init() {
        //映射LAPIC内存到1(为APIC使用)
        VirtMem::map(1, 0xFEE00);

        reg(LAPIC_TPR) = 0x20;//设置中断优先级(放行32以上的中断)
        reg(LAPIC_LVT_LINT0) = LAPIC_LINT0 | LAPIC_DM(LAPIC_DM_FIXED) | LAPIC_MASK(0);//LINT0
        reg(LAPIC_LVT_LINT1) = 0 | LAPIC_DM(LAPIC_DM_NMI) | LAPIC_MASK(1);//LINT1
        reg(LAPIC_LVT_ERROR) = LAPIC_ERROR | LAPIC_DM(LAPIC_DM_FIXED) | LAPIC_MASK(0);//Error
        reg(LAPIC_SPIVR) |= (0x100 | LAPIC_SPU);//软启用LAPIC, 并设置伪中断号为255
    }
}
