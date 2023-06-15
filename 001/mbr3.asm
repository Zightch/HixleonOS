org 0x7c00

mov ax, 0xb800
mov es, ax
mov di, 0
mov cx, 2000

call cls

pause:
hlt
jmp pause

cls:
mov ax, 0x0700
.for:
mov [es : di], ax
add di, 2
loop .for
ret

times 510-($-$$) db 0
db 0x55, 0xaa
