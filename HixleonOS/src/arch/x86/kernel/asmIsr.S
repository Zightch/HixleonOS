.macro isrTemplate vector, noErrorCode=1, suffix=v
    .global _Z7asmIsr\vector\suffix
    .type _Z7asmIsr\vector\suffix, @function
    _Z7asmIsr\vector\suffix:
        .if \noErrorCode
            pushl $0
        .endif
        pushl $\vector
.endm

.section .text
    isrTemplate 0

    movl %esp, %eax
    andl $0xFFFFFFF0, %esp
    subl $0x10, %esp
    movl %eax, (%esp)
    call _Z16interruptHandlerP8IsrParam
    popl %eax
    movl %eax, %esp
    addl $8, %esp

    iret
