#pragma once

void cpuid(int func, unsigned int& eax, unsigned int& ebx, unsigned int& ecx, unsigned int& edx);

bool iscpuid();//判断cpuid指令是否可用

void hlt();

unsigned int cr0();

unsigned int cr2();

unsigned int cr3();

void cr0(unsigned int);

void cr2(unsigned int);

void cr3(unsigned int);

void invplg(unsigned int);

void enableInterrupt();

void disableInterrupt();

void invtlb();

unsigned long long rdmsr(unsigned int);

void wrmsr(unsigned int, unsigned long long);

void outb(unsigned short, unsigned char);
void outw(unsigned short, unsigned short);
void outl(unsigned short, unsigned int);

unsigned char inb(unsigned short);
unsigned short inw(unsigned short);
unsigned int inl(unsigned short);
