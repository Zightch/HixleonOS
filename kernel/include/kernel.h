#pragma once

#pragma pack(push, 1)
struct MemMap {
    unsigned long long base;
    unsigned long long size;
    unsigned int type;
};
#pragma pack(pop)
