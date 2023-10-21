#pragma once

#define LAPIC_DM_FIXED    0b000
#define LAPIC_DM_SMI      0b010
#define LAPIC_DM_NMI      0b100
#define LAPIC_DM_EXT_INT  0b111
#define LAPIC_DM_INIT     0b101

#define LAPIC_DM(DMVal) ((DMVal << 8) & 0x700)

#define LAPIC_MASK(isMask) ((isMask << 16) & 0x10000)

#define LAPIC_TIG_MODE(mode) ((mode << 15) & 0x8000)

#define LAPIC_REG(addr) (*(unsigned int*)0x00001##addr)

#define LAPIC_INT_IN(pin) ((pin << 13) & 0x2000)
