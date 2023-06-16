org 0x8000

;内存区段            大小                含义
;0x0500 ~ 0x7bff    30463B              栈内存区
;0x7c00 ~ 0x7dff    512B   (0.5KB)      MBR扇区
;0x7e00 ~ 0x7FFF    512B   (0.5KB)      扇区缓存区
;0x8000 ~ 0x9FBFF   621567B             LOADER文件内容和可用堆区
;0x100000 ~ --      4GB - 1MB           系统内存区

cli;关中断

;开启A20地址线
mov word dx, 0x92
in byte al, dx
or byte al, 0x02
out dx, byte al

;设置GDT
xor dword edx, edx
mov dword eax, am + 63
mov dword ecx, 0x1000
div dword ecx
cmp dword edx, 0
mul dword ecx
je setGDT.else
add dword eax, 0x1000
setGDT.else:
mov dword ebx, am
mov dword ecx, 56;表大小
mov word [ebx], cx
mov dword [ebx + 2], eax

mov dword edi, eax
mov dword eax, am + 6
mov dword esi, eax

mov word cx, 14
setGDT.for:
mov dword eax, [esi]
mov dword [edi], eax
add dword edi, 4
add dword esi, 4
loop setGDT.for
;以上是4KB对齐GDT表

mov dword ebx, am
lgdt [ebx]

;切换保护模式
mov dword eax, cr0
or dword eax, 0x00000001
mov dword cr0, eax

;刷新跳转
jmp 0x0018 : beforeEntryKernel ; CS使用3号GDT描述符

[bits 32];以下代码使用32bits编译
beforeEntryKernel:

;切换完成, 准备进入Kernel
;解析Kernel的ELF32结构
;am(4)      : 扇区号(33-64, 遍历根目录, 寻找"KERNEL"文件)
;am + 4(2)  : 读取扇区函数地址
;am + 6(2)  : 临时簇号保存
;am + 8(4) : 上一次读取的扇区号
;am + 12(4) : 文件断点续写地址
;0x7E00 : 扇区缓存区
;0x100000 : Kernel文件区

xor dword eax, eax
mov dword ebx, am + 8
mov dword [ebx], eax;初始化am + 8(上一次读取扇区号)
mov dword eax, 0x100000
mov dword ebx, am + 12
mov dword [ebx], eax;初始化am + 12(文件断电续写地址)

mov dword ebx, esp
mov word ax, [ebx];取出readSector函数地址
mov dword ebx, am + 4
mov word [ebx], ax;把地址转写到am + 4的地址上

mov dword eax, 33
mov dword ebx, am
mov dword [ebx], eax;写入扇区号

;遍历根目录
rootDirTrvs:
mov dword ebx, am
mov dword ebx, [ebx]
cmp dword ebx, 64
ja fileNotFind
mov dword edi, 0x7E00
call readSector32

;查找文件
cld ;repe cmpsb字节数组比较时si, di递增, std命令相反(递减)
xor dword ebx, ebx ;用bx记录遍历第几个目录项
nextDir:
mov dword esi, ebx
shl dword esi, 5 ;乘以32(目录项大小)
add dword esi, 0x7E00 ;si指向扇区缓存区
mov dword edi, kernelFileName ;di指向我的文件名
mov dword ecx, 11 ;文件名长度11
repe cmpsb ;逐字节比较
jcxz fileFind ;如果字节匹配相同跳转到文件已找到
add dword ebx, 1 ;bx += 1
cmp dword ebx, 16
jl nextDir ;如果bx < 16, 跳转到nextDir

;扇区号++
mov dword ebx, am
mov dword eax, [ebx]
add dword eax, 1
mov dword [ebx], eax
jmp rootDirTrvs; 循环下一个扇区

;文件找到
fileFind:
;判断文件长度
xor dword eax, eax
shl dword ebx, 5 ;乘以32
add dword ebx, 0x7e00
mov dword eax, [ebx + 28] ;获取文件长度
cmp dword eax, 0
je fileError

;获取起始簇号
xor dword eax, eax
mov word ax, [ebx + 26] ;获取起始簇号
mov dword ebx, am + 6
mov word [ebx], ax ;保存在临时内存中

readFileNext:
;确定当前簇号所在FAT1表的扇区号
cmp word ax, 0xFFF7
ja readFileEnd ;如果>0xfff7(说明>=0xfff8)跳转到读取文件结束
cmp word ax, 8192
jnb fileError
xor dword edx, edx
mov dword ecx, 0xFF
div dword ecx
add dword eax, 1
mov dword ebx, am + 8
mov dword ebx, [ebx]
cmp dword eax, ebx
je skipReadFAT;如果上一次读的扇区号和这一次相同, 跳过读取扇区

;读取FAT1表到0x7e00中
mov dword ebx, am + 8
mov dword [ebx], eax
mov dword edi, 0x7e00
mov dword ebx, eax
call readSector32

skipReadFAT:
;按簇号读取文件内容到0x100000中
mov dword ebx, am + 12
mov dword edi, [ebx];读取断点续写地址
mov word bx, am + 6
mov word bx, [ebx] ;从可用内存段里读取临时簇号
add word bx, 63 ;bx += 63获取扇区号
call readSector32
mov dword ebx, am + 12
mov dword [ebx], edi;更新断点续写地址

;读取下一个簇号
mov dword ebx, am + 6
mov word bx, [ebx] ;从可用内存段里读取临时簇号
shl word bx, 1 ;乘以2
add word bx, 0x7e00 ;指向下一个簇号的地址
mov word ax, [ebx]
mov dword ebx, am + 6
mov word [ebx], ax ;临时保存簇号
jmp readFileNext

readFileEnd:
;开始解析ELF32文件结构


jmp pause

fileNotFind:
mov dword esi, kernelNotFind
jmp print
fileError:
mov dword esi, kernelFileError
print:
mov dword ecx, esi
mov dword edi, 0xB8000
mov byte ah, 0x07 ;黑底白字
.for:
mov byte al, [esi] ;ds:si取出一个字节
cmp byte al, 0 ;比较al与0
jz .forEnd ;如果相等跳转到.end
mov word [edi], ax
add dword esi, 1
add dword edi, 2
jmp .for ;跳转到for
.forEnd:

mov dword edx, 0x03D4
mov byte al, 0x0F
out dx, byte al ; 设置为光标低8位

mov dword edx, 0x03D5
sub dword esi, ecx
mov dword eax, esi
out dx, byte al ; 设置光标低8位

pause:
hlt;暂停CPU
jmp pause

readSector32:
    mov dword esi, am + 4
    mov word ax, [esi]
    jmp 0x0008 : .readSector16
    [bits 16]
    .readSector16:
    call ax
    jmp 0x0018 : .readSector16End
    [bits 32]
    .readSector16End:
    ret

kernelFileName:
db 'KERNEL     ', 0x00

kernelNotFind:
db 'The kernel is not found. File in "/KERNEL".', 0x00

kernelFileError:
db 'The kernel file is error. File in "/KRENEL".', 0x00

am:
;GDTR数据, 对齐之后和英语存放程序数据
times 6 db 0

;GDT表数据, 对齐之后可用于存放程序数据
    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ; 0: (Base:0, Limit:0, Flags:0)
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0x8f, 0x00 ; 1: (Base:0, Limit:0xFFFFF, Flags:SEG_R0_CODE16)
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0x8f, 0x00 ; 2: (Base:0, Limit:0xFFFFF, Flags:SEG_R0_DATA16)
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00 ; 3: (Base:0, Limit:0xFFFFF, Flags:SEG_R0_CODE32)
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xcf, 0x00 ; 4: (Base:0, Limit:0xFFFFF, Flags:SEG_R0_DATA32)
    db 0xff, 0xff, 0x00, 0x00, 0x10, 0x9a, 0xcf, 0x00 ; 5: (Base:0x100000, Limit:0xFFFFF, Flags:SEG_R0_CODE32)
    db 0xff, 0xff, 0x00, 0x00, 0x10, 0x92, 0xcf, 0x00 ; 6: (Base:0x100000, Limit:0xFFFFF, Flags:SEG_R0_DATA32)
    db 0x00
