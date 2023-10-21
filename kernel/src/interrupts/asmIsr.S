#include "interrupts/fixedIntNum.h"

.macro isrTemplate vector, noErrorCode = 1
    .global asmIsr\vector
    .type asmIsr\vector, @function
    asmIsr\vector:
        .if \noErrorCode
            pushl $0
        .endif
        pushl $\vector
        jmp isrWrapper
.endm

.section .text
    isrTemplate 0
    isrTemplate 14, noErrorCode = 0

    isrWrapper:
        movl %esp, %eax
        andl $0xFFFFFFF0, %esp
        subl $0x10, %esp
        movl %eax, (%esp)
        call _Z16interruptHandlerP8IsrParam
        popl %eax
        movl %eax, %esp
        addl $8, %esp
        iret
