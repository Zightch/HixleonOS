#pragma once

#pragma pack(push, 1)
typedef struct {
    unsigned int vector;
    unsigned int errCode;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} IsrParam;
#pragma pack(pop)

typedef void (*IntFn)(IsrParam*);

void setIntFn(unsigned char, IntFn);
