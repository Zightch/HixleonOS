#include "arch/x86/kernel/ttyWH.h"

.section .text
    .global _Z12ttySetCursorhh
    .type _Z12ttySetCursorhh, @function
    _Z12ttySetCursorhh:
        pushl %ebp
        movl %esp, %ebp

        xorl %eax, %eax
        xorl %ebx, %ebx
        xorl %ecx, %ecx
        xorl %edx, %edx
        /* ebp+8: x, ebp+12: y */
        movb 12(%ebp), %al
        subb $1, %al
        movb $TTY_WIDTH, %cl
        mulw %cx

        movb 8(%ebp), %bl
        subb $1, %bl
        addw %bx, %ax

        movw %ax, ttyOffset

        movw %ax, %cx
        xorl %eax, %eax

        movw $0x03D4, %dx
        movb $0x0F, %al
        outb %al, %dx /* 设置为光标低8位 */

        movw $0x03D5, %dx
        movb %cl, %al
        outb %al, %dx /* 设置为光标低8位 */

        shrl $8, %ecx

        movw $0x03D4, %dx
        movb $0x0E, %al
        outb %al, %dx /* 设置为光标高8位 */

        movw $0x03D5, %dx
        movb %cl, %al
        outb %al, %dx /* 设置为光标高8位 */

        leave
        ret
