.section .bss /* 栈 */
    /* System V ABI */
    .align 16
    stackBottom:
        .skip 16318, 0
    stackTop:

.section .text /* 可执行代码 */
    .global start
    .type start, @function
    start:
        movl $stackTop - 4, %esp
        movl $stackTop, %ebp
        call kernelInit_

        subl $0x06, %esp
        movl $gdt_, 2(%esp)
        movw gdtLimit_, %ax
        movw %ax, (%esp)
        lgdt (%esp)
        addl $0x06, %esp

        movw $0x10, %cx
        movw %cx, %es
        movw %cx, %ds
        movw %cx, %fs
        movw %cx, %gs
        movw %cx, %ss

        pushw $0x08
        pushl $GDTDone
        retf

    GDTDone:
        pushl %ebx
        call kernelMain_
        cli
    pause:
        hlt
        jmp pause
