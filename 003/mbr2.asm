org 0x7c00

mov ax, cs
mov ds, ax

mov di, 0x7e00

mov byte [di + 0x0000], 1
mov byte [di + 0x0001], 2
mov byte [di + 0x0002], 3
mov byte [di + 0x01FD], 3
mov byte [di + 0x01FE], 2
mov byte [di + 0x01FF], 1

;第一步，检查硬盘控制器状态
notReady1:
mov dx, 0x01f7
in al, dx ;读0x01f7端口
and al, 0xc0 ;第7位与第6位
cmp al, 0x40 ;第7位为0，第6位为1
jne notReady1 ;如果不相等跳转到notReady1

;第二步，设置要写入的扇区数量
mov al, 1
mov dx, 0x01f2
out dx, al ;设置写入一个扇区

;第三步，设置扇区地址(LBA)
mov eax, 1
mov dx, 0x01f3
out dx, al ;LBA地址7~0位写入0x01f3
shr eax, 8 ;右移8位
mov dx, 0x01f4
out dx, al ;LBA地址15~8位写入0x01f4
shr eax, 8 ;右移8位
mov dx, 0x01f5
out dx, al ;LBA地址16~23位写入0x01f5

;第四步，写入LBA最后4位+设置主盘与LBA模式
shr eax, 8 ;右移8位
and al, 0x0F
or al, 0xe0
mov dx, 0x01f6
out dx, al

;第五步，向0x01f7写入0x30表示写入
mov dx, 0x01f7
mov al, 0x30
out dx, al

;第六步，检查硬盘控制器状态
notReady2:
mov dx, 0x01f7
in al, dx ;读0x01f7端口
and al, 0x88 ;第7位与第3位
cmp al, 0x08 ;第7位为0，第3位为1
jne notReady2 ;如果不相等跳转到notReady2

;第7步，循环读取256次数据写到扇区中
mov dx, 0x01f0
mov cx, 256
writeData:
mov ax, [di]
out dx, ax
add di, 2
loop writeData

pause:
hlt
jmp pause

times 510-($-$$) db 0
db 0x55, 0xaa
