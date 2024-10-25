#include "int/intTable.h"
#include "crash.h"

auto strTemp = "Interrupt : --\n"
               "    EIP   : --------\n"
               "    CS    : ----\n"
               "    ELFAGS: --------\n"
               "Error code: --------\n";

void intUndef(IsrParam *ip) {
    char ts[110] = {0};
    for (int i = 0; i < 110; i++)
        ts[i] = strTemp[i];

    ts[12] = (char)((ip->vector >> 4) & 0x0F);
    if (ts[12] < 10) ts[12] += 48;
    else ts[12] += 55;
    ts[13] = (char)(ip->vector & 0x0F);
    if (ts[13] < 10) ts[13] += 48;
    else ts[13] += 55;

    for (int i = 7; i >= 0; i--) {
        int index = 34 - i;
        ts[index] = (char)((ip->eip >> (i * 4)) & 0x0F);
        if (ts[index] < 10) ts[index] += 48;
        else ts[index] += 55;
    }

    for (int i = 3; i >= 0; i--) {
        int index = 51 - i;
        ts[index] = (char)((ip->cs >> (i * 4)) & 0x0F);
        if (ts[index] < 10) ts[index] += 48;
        else ts[index] += 55;
    }

    for (int i = 7; i >= 0; i--) {
        int index = 72 - i;
        ts[index] = (char)((ip->eflags >> (i * 4)) & 0x0F);
        if (ts[index] < 10) ts[index] += 48;
        else ts[index] += 55;
    }

    for (int i = 7; i >= 0; i--) {
        int index = 93 - i;
        ts[index] = (char)((ip->errCode >> (i * 4)) & 0x0F);
        if (ts[index] < 10) ts[index] += 48;
        else ts[index] += 55;
    }

    crash(ts);
}

IntFn intTable[256] = {nullptr};

void interruptHandler(IsrParam *ip) {
    if (ip->vector < 256) {
        auto tmp = intTable[ip->vector];
        if (tmp != nullptr)
            tmp(ip);
        else
            intUndef(ip);
    }
}

void setIntFn(unsigned char vector, IntFn fn) {
    intTable[vector] = fn;
}
