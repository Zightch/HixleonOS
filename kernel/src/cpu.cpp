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

void hlt() {
    while (true)
        __asm__("hlt");
}

unsigned int cr0() {
    unsigned int tmp;
    __asm__(
            "movl %%cr0, %0"
            :"=r"(tmp)
            );//获取cr0值
    return tmp;
}

unsigned int cr2() {
    unsigned int tmp;
    __asm__(
            "movl %%cr2, %0"
            :"=r"(tmp)
            );//获取cr2值
    return tmp;
}

unsigned int cr3() {
    unsigned int tmp;
    __asm__(
            "movl %%cr3, %0"
            :"=r"(tmp)
            );//获取cr3值
    return tmp;
}

void cr0(unsigned int val) {
    __asm__("mov %0, %%cr0"::"r"(val));
}

void cr2(unsigned int val) {
    __asm__("mov %0, %%cr2"::"r"(val));
}

void cr3(unsigned int val) {
    __asm__("mov %0, %%cr3"::"r"(val));
}

void invplg(unsigned int page) {
    if (page < 1048576)
        __asm__("invlpg (%0)"::"r"(page << 12) : "memory");//刷新TLB
}

void enableInterrupt() {
    __asm__("sti");
}

void disableInterrupt() {
    __asm__("cli");
}

void invtlb() {
    __asm__(
            "movl %cr3, %eax\n"
            "movl %eax, %cr3"
            );
}

unsigned long long rdmsr(unsigned int msr) {
    unsigned int edx, eax;
    __asm__("rdmsr" : "=d"(edx), "=a"(eax) : "c"(msr));
    unsigned long long tmp = 0;
    tmp = edx;
    tmp <<= 32;
    tmp |= eax;
    return tmp;
}

void wrmsr(unsigned int msr, unsigned long long val) {
    unsigned int edx, eax;
    edx = val >> 32;
    eax = val;
    __asm__("wrmsr" : : "d"(edx), "a"(eax), "c"(msr));
}

void outb(unsigned short port, unsigned char value) {
    __asm__("outb %0, %1" : : "a" (value), "Nd" (port));
}

void outw(unsigned short port, unsigned short value) {
    __asm__("outw %0, %1" : : "a" (value), "Nd" (port));
}

void outl(unsigned short port, unsigned int value) {
    __asm__("outl %0, %1" : : "a" (value), "Nd" (port));
}

unsigned char inb(unsigned short port) {
    unsigned char value;
    __asm__("inb %1, %0" : "=a" (value) : "Nd" (port));
    return value;
}

unsigned short inw(unsigned short port) {
    unsigned short value;
    __asm__("inw %1, %0" : "=a" (value) : "Nd" (port));
    return value;
}

unsigned int inl(unsigned short port) {
    unsigned int value;
    __asm__("inl %1, %0" : "=a" (value) : "Nd" (port));
    return value;
}
