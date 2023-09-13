.section .bss
    .align 16
    stackBottom:
        .skip 1048576, 0
    stackTop:

.section .text
    .global kernelEntry
    .type kernelEntry, @function
    kernelEntry:
        movl $stackTop - 8, %esp
        movl $stackTop - 4, %ebp
        pushl %ebx
        pushl %eax
        call _Z10kernelInitP9MemoryMapt

        subl $0x06, %esp

        movl $gdt, 2(%esp)
        movw gdtLimit, %ax
        movw %ax, (%esp)
        lgdt (%esp)

        movl $idt, 2(%esp)
        movw idtLimit, %ax
        movw %ax, (%esp)
        lidt (%esp)

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
        call _Z10kernelMainv
        cli
    pause:
        hlt
        jmp pause
