#pragma once

#pragma pack(push, 1)
class MemMap {
public:
    unsigned long long base;
    unsigned long long size;
    unsigned int type;
};
#pragma pack(pop)
