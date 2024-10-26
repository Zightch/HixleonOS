#include "grub/multiboot.h"
#define KPG_SIZE (10 * 4096)
#define MULTIBOOT_MMAP_ENTRY_SIZE 24
#define MULTIBOOT_MMAP_MAX_LENGTH 31
#define MULTIBOOT_MMAP_DATA_MAX_SIZE (MULTIBOOT_MMAP_MAX_LENGTH * 20)

.section .multiboot
    .int MB_MAGIC
    .int MB_ALIGNED_4K_MEM_MAP
    .int CHECKSUM(MB_ALIGNED_4K_MEM_MAP)

.section .kpg, "wa", @nobits
    .align 4096
    kpg:
        .skip KPG_SIZE, 0

.section .hhkInitData, "a"
    multibootMMapLength:
        .skip 4, 0
    multibootMMapData:
        .skip MULTIBOOT_MMAP_DATA_MAX_SIZE, 0

.section .hhkInitRodata, "a"
    mmapLengthTooLongStr:
        .asciz "Memory map length too long!"

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
        # 根据启动方式判定是grub还是legacy
        cmpl $MB_FINISH_EAX, %eax
        jne legacySaveMemMap
        # grub保存内存地图
        grubSaveMemMap:
            # 偏移到mmap_length
            addl $44, %ebx
            # 保存mmap_length
            movl (%ebx), %ecx
            # 偏移到mmap_addr
            addl $4, %ebx
            # 提取mmap_addr
            movl (%ebx), %esi
            # 用ecx除以multiboot mmap entry size, 以确定条目个数
            movl %ecx, %eax # 被除数-字节数
            movl $MULTIBOOT_MMAP_ENTRY_SIZE, %ecx # 除数-条目长度
            divl %ecx # eax / reg = eax ... edx
            movl %eax, %ecx # 提取商
            # 先检查mmap_length是否超过范围
            cmpl $MULTIBOOT_MMAP_MAX_LENGTH, %ecx
            jg mmapLengthTooLong
            # 保存mmap_length
            movl $multibootMMapLength, %edi
            movl %ecx, (%edi)
            # 转储
            movl $multibootMMapData, %edi
            loop1:
                addl $4, %esi # 偏移走4个无用数据

                movl 0(%esi), %eax # 复制baseLow
                movl 4(%esi), %ebx # 复制baseHigh
                movl %eax, 0(%edi) # 转储baseLow
                movl %ebx, 4(%edi) # 转储baseHigh

                addl $8, %esi
                addl $8, %edi

                movl 0(%esi), %eax # 复制sizeLow
                movl 4(%esi), %ebx # 复制sizeHigh
                movl %eax, 0(%edi) # 转储sizeLow
                movl %ebx, 4(%edi) # 转储sizeHigh

                addl $8, %esi
                addl $8, %edi

                movl (%esi), %eax # 复制type
                movl %eax, (%edi) # 转储type

                addl $4, %esi
                addl $4, %edi
            loop loop1
            movl $multibootMMapLength, %edi
            movl (%edi), %ecx
            movl $multibootMMapData, %ebx
            # mmap_length
            pushl %ecx
            # mmap_addr
            pushl %ebx
        jmp callHHKInit
        # legacy内存地图保存
        legacySaveMemMap:
            # 先检查mmap_length是否超过范围
            cmpl $MULTIBOOT_MMAP_MAX_LENGTH, %ecx
            jg mmapLengthTooLong
            # mmap_length
            pushl %ecx
            # mmap_addr
            pushl %ebx
        callHHKInit:
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
        # mmap_addr
        popl %eax
        # mmap_length
        popl %ebx
        movl $kpg, %ecx
        movl $KPG_SIZE, %edx

        jmp kernelEntry

        mmapLengthTooLong:
            movl $mmapLengthTooLongStr, %esi
            movl %esi, %ecx # 存储字符串起始位置, 方便计算长度
            movl $0xB8000, %edi
            movb $0x0C, %ah # 黑底红字
            loop0:
                movb (%esi), %al # esi取出一个字节
                cmpb $0, %al # 比较al与0
                je loop0End # 如果相等跳转到loop0End
                movw %ax, (%edi) # 放入显存
                addl $1, %esi # 字符串后偏移1
                addl $2, %edi # 显存后偏移2
                jmp loop0
            loop0End:
                movl $0x03D4, %edx
                movb $0x0F, %al
                outb %al, %dx # 设置为光标低8位

                movl $0x03D5, %edx
                subl %ecx, %esi
                movl %esi, %eax
                outb %al, %dx # 设置光标低8位

        pause:
            cli
            hlt
        jmp pause
