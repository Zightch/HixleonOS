#define KPG_SIZE (10 * 4096)

.section .kpg, "wa", @nobits
    kPTD:
        .skip KPG_SIZE, 0

.section .hhkInitBss, "wa", @nobits /* 临时栈 */
    .align 16
    memoryMap:
        .skip 4096, 0 /* mSize: 2byte, mmap: 4094byte */
    stackBottom:
        .skip 4096, 0
    stackTop:

.section .hhkInitText, "ax" /* hhkInit可执行代码 */
    .global main
    .type main, @function
    main:
        cli # 关中断，直到我们准备好PIC为止
        movl $stackTop - 4, %esp
        movl $stackTop, %ebp
        /* 如下代码进行内存地图的保存 */
        cmpw $204, %cx /* 比较204与cx */
        jbe skip204ToCX_ /* if (cx > 204) */
        movl $204, %ecx /* cx = 204 */
        skip204ToCX_:
        movl $memoryMap, %edi /* edi = memoryMap */
        movw %cx, (%edi) /* mSize = cx */
        movl $20, %eax /* eax = 20 */
        mull %ecx /* edx eax = eax * ecx */
        xorl %ecx, %ecx /* ecx = 0 */
        addl $2, %edi /* edi += 2 */
        memcpy_:
            cmpl %eax, %ecx
            jge endMemcpy_
            movl (%ebx), %edx
            movl %edx, (%edi)
            addl $4, %ebx
            addl $4, %edi
            addl $4, %ecx
            jmp memcpy_
        endMemcpy_:
        /* 内存地图保存完成 */
        subl $8, %esp /* esp -= 8 */
        movl $(KPG_SIZE), 4(%esp) /*  */
        movl $(kPTD - 0xC0000000), (%esp) /* PTD物理地址 */
        call _Z7kpgInitPvj
        

.section .text /* 可执行代码 */
    .global entry
    .type start, @function
    start:
        movl $stackTop - 4, %esp
        movl $stackTop, %ebp
        pushl %ecx
        pushl %ebx
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
