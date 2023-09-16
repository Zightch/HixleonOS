#pragma once

#define HHK_INIT_TEXT __attribute__((section(".hhkInitText")))
#define HHK_INIT_BSS __attribute__((section(".hhkInitBss")))
#define HHK_INIT_DATA __attribute__((section(".hhkInitData")))
#define HHK_INIT_RODATA __attribute__((section(".hhkInitRodata")))

#define GET_LD_DATA(var) ((unsigned int)&var)

extern unsigned int hhkInitEnd;
extern unsigned int kernelStart;
extern unsigned int kernelEnd;
