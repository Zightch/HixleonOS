org 0x7c00

mov ah, 0x07
mov cx, 0
mov dx, 0

for:
cmp cx, 16
jae pause
mov bx, 0
mov ds, bx
mov bx, [0x7deb]
mov al, [bx]
add bx, 1
mov [0x7deb], bx
mov bx, 0xb800
mov ds, bx
mov bx, dx
add dx, 2
mov [bx], ax
add cx, 1
jmp for

pause:
hlt
jmp pause

times 491-($-$$) db 0
db 0xed, 0x7d
db "Hello,HixleonOS!"
db 0
db 0x55, 0xaa
