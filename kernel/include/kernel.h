#pragma once

#pragma pack(push, 1)
typedef struct {
    unsigned long long base;
    unsigned long long size;
    unsigned int type;
} MemMap;
#pragma pack(pop)

extern MemMap *memMap;
extern unsigned short memMapSize;
