#pragma once

#include "fixedIntNum.h"
#include "intTable.h"

extern "C" void asmIsr0();

void interruptHandler(IsrParam*);
