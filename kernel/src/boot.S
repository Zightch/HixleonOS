#define KPG_SIZE (10 * 4096)

.section .kpg, "wa", @nobits
    kpg:
        .skip KPG_SIZE, 0

.section .hhkInitBss, "wa", @nobits /* 临时栈 */
    .align 16
    stackBottom:
        .skip 4096, 0
    stackTop:

.section .hhkInitText, "ax" /* hhkInit可执行代码 */
    .global main
    .type main, @function
    main:
        cli # 关中断，直到我们准备好PIC为止
        movl $stackTop - 4, %esp
        movl $stackTop - 4, %ebp
        # 内存地图保存
        pushl %ecx
        pushl %ebx
        # 调用hhkInit
        pushl $KPG_SIZE
        pushl $(kpg - 0xC0000000)
        call _Z7hhkInitPvj

        movl (%esp), %eax
        andl $0xfffff000, %eax
        movl %eax, %cr3 # 安装PDT

        movl %cr0, %eax
        orl $0x80000000, %eax
        movl %eax, %cr0 # 打开内存分页

        popl %eax
        popl %eax
        popl %eax
        popl %ebx
        movl $kpg, %ecx
        movl $KPG_SIZE, %edx

        jmp kernelEntry
