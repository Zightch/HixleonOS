#include "cpu.h"

void cpuid(int func, unsigned int& eax, unsigned int& ebx, unsigned int& ecx, unsigned int& edx) {
    __asm__ (
            "cpuid"
            : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
            : "a" (func)
            );
}

bool iscpuid() {
    unsigned int eflags0;
    __asm__(
            "pushf\n"
            "popl %0"
            :"=r"(eflags0)
            );//获取eflags值
    bool bit21 = (eflags0 >> 21) & 1;
    unsigned int eflags1 = bit21 ? (eflags0 & 0xFFDFFFFF) : (eflags0 | 0x200000);//取反第21位
    __asm__(
            "pushl %0\n"
            "popf"
            :
            :"g"(eflags1)
            );//存入eflags反转21位后的值
    unsigned int eflags2;
    __asm__(
            "pushf\n"
            "popl %0"
            :"=r"(eflags2)
            );//再次获取eflags值
    bool ret = eflags2 == eflags1;//判断是否相等
    __asm__(
            "pushl %0\n"
            "popf"
            :
            :"g"(eflags0)
            );//存入eflags最初的值
    return ret;
}
