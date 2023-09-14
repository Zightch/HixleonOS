#pragma once

#pragma pack(push, 1)
class IsrParam {
public:
    unsigned int vector;
    unsigned int errCode;
    unsigned int eip;
    unsigned short cs;
    unsigned int eflags;
};
#pragma pack(pop)

void isr0(IsrParam*);
