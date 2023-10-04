#pragma once

void cpuid(int func, unsigned int& eax, unsigned int& ebx, unsigned int& ecx, unsigned int& edx);

bool iscpuid();//判断cpuid指令是否可用
