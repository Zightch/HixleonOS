#pragma once

#include "interrupts/fixedIntNum.h"

#define IDT_ATTR(dpl)                     ((0x70 << 5) | (dpl & 3) << 13 | 1 << 15)

void initIDT_();
