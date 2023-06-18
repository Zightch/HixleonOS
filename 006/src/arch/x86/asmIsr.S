.macro isrTemplate vector, noErrorCode=1
    .global asmIsr\vector
    .type asmIsr\vector, @function
    asmIsr\vector:
        .if \noErrorCode
            pushl $0
        .endif
        pushl $\vector
        jmp interruptWrapper
.endm

.section .text
    isrTemplate 0

    interruptWrapper:
        movl %esp, %eax
        andl $0xFFFFFFF0, %esp
        subl $0x10, %esp
        movl %eax, (%esp)
        call interruptHandler
        popl %eax
        movl %eax, %esp
        addl $8, %esp

        iret
