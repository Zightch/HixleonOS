org 0x7c00

mov ax, cs
mov ds, ax ;ds指向cs相同段

mov ax, 0xb800
mov es, ax

mov di, 0
mov cx, 2000

mov si, byteArray ;ds:si存放字符串数据

call cls

mov di, 0 ;es:di指向显存地址

call print

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

print:
mov ah, 0x0c ;黑底红字
.for:
mov al, [si] ;ds:si取出一个字节
cmp al, 0 ;比较al与0
jz .end ;如果相等跳转到.end
mov [es : di], ax
add si, 1
add di, 2
jmp .for ;跳转到for
.end:
ret

byteArray:
db "Hello,HixleonOS!", 0

times 510-($-$$) db 0
db 0x55, 0xaa
