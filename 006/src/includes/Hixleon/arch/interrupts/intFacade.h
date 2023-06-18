#pragma once
#include <Hixleon/arch/interrupts/fixedIntNum.h>
#include <Hixleon/arch/interrupts/intTable.h>

extern "C" void asmIsr0();

extern "C" void interruptHandler(IsrParam*);
