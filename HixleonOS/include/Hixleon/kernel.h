#pragma once

#pragma pack(push, 1)
class MemoryMap {
    unsigned long long base;
    unsigned long long limit;
    unsigned int type;
};
#pragma pack(pop)
