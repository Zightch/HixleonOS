#pragma once

#define LAPIC_DM_FIXED    0b000
#define LAPIC_DM_SMI      0b010
#define LAPIC_DM_NMI      0b100
#define LAPIC_DM_EXT_INT  0b111
#define LAPIC_DM_INIT     0b101

#define LAPIC_TM_ONE_SHOT     0b00
#define LAPIC_TM_PERIODIC     0b01
#define LAPIC_TM_TSC_DEADLINE 0b10

#define LAPIC_DM(DMVal) ((DMVal << 8) & 0x700)

#define LAPIC_MASK(isMask) ((isMask << 16) & 0x10000)

#define LAPIC_TIG_MODE(mode) ((mode << 15) & 0x8000)

#define LAPIC_INT_IN(pin) ((pin << 13) & 0x2000)

#define LAPIC_TM(TMVal) ((TMVal << 17) & 0x60000)

#define LAPIC_IDR 0x20   // ID Reg
#define LAPIC_VER 0x30   // Version Reg
#define LAPIC_TPR 0x80   // Task Priority
#define LAPIC_APR 0x90   // Arbitration Priority
#define LAPIC_PPR 0xA0   // Processor Priority
#define LAPIC_EOI 0xB0   // End-Of-Interrupt
#define LAPIC_RRD 0xC0   // Remote Read
#define LAPIC_LDR 0xD0   // Local Destination Reg
#define LAPIC_DFR 0xE0   // Destination Format Reg
#define LAPIC_SPIVR 0xF0 // Spurious Interrupt Vector Reg
#define LAPIC_ISR_BASE 0x100 // Base address for In-Service-Interrupt bitmap register (256bits)
#define LAPIC_TMR_BASE 0x180 // Base address for Trigger-Mode bitmap register (256bits)
#define LAPIC_IRR_BASE 0x200 // Base address for Interrupt-Request bitmap register (256bits)
#define LAPIC_ESR 0x280      // Error Status Reg
#define LAPIC_ICR_BASE 0x300 // Interrupt Command
#define LAPIC_LVT_LINT0 0x350
#define LAPIC_LVT_LINT1 0x360
#define LAPIC_LVT_ERROR 0x370
#define LAPIC_TIMER_LVT 0x320
#define LAPIC_TIMER_ICR 0x380 // Initial Count
#define LAPIC_TIMER_CCR 0x390 // Current Count
#define LAPIC_TIMER_DCR 0x3E0 // Divide Configuration

#define LAPIC_TIMER_DIV1 0b1011
#define LAPIC_TIMER_DIV2 0b0000
#define LAPIC_TIMER_DIV4 0b0001
#define LAPIC_TIMER_DIV8 0b0010
#define LAPIC_TIMER_DIV16 0b0011
#define LAPIC_TIMER_DIV32 0b1000
#define LAPIC_TIMER_DIV64 0b1001
#define LAPIC_TIMER_DIV128 0b1010

namespace LAPIC {
    void init();

    unsigned int &reg(unsigned short);
}
