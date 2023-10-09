#pragma once

#pragma pack(push, 1)
typedef struct {
    unsigned int vector;
    unsigned int errCode;
    unsigned int eip;
    unsigned short cs;
    unsigned int eflags;
} IsrParam;
#pragma pack(pop)

void isr0(IsrParam*);
