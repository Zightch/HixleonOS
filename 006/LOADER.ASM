org 0x8000

;内存区段            大小         含义
;0x0500 ~ 0x7bff     30463B       栈内存区
;0x7c00 ~ 0x7dff     512B         MBR扇区
;0x7e00 ~ 0x7FFF     512B         扇区缓存区
;0x8000 ~ 0x9FBFF    621567B      LOADER文件内容和可用堆区
;0x100000 ~ --       4095MB       系统内存区

;开启A20地址线
mov word dx, 0x92
in byte al, dx
or byte al, 0x02
out dx, byte al

;设置GDT
xor dword edx, edx
mov dword eax, am + 0x200;留512B大小内存
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

;初始化am + 16
mov dword ebx, am + 16
xor dword eax, eax
mov dword [ebx], eax
;开始计算内存大小
mov dword edi, am + 20;信息存放地址
mov dword ecx, 20;表大小
mov dword ebx, 0;下是否有下一个表
mov dword edx, 0x534d4150;表标识符
; 获取内存信息
getMemorySize.for:
xor dword esi, esi
mov dword eax, 0xe820;中断号
int 0x15
jc memE
cmp dword edx, 0x534d4150; 比较edx和内存映射表的签名是否相同
jne memE ; 如果不相同，跳转到错误处理
mov dword esi, es : [edi]
cmp dword esi, 0x100000
jne getMemorySize.continue
mov dword esi, es : [edi + 4]
cmp dword esi, 0
jne getMemorySize.continue
mov dword esi, es : [edi + 16]
cmp dword esi, 1
jne memE
jmp getMemSize
getMemorySize.continue:
cmp dword ebx, 0
je memE
jmp getMemorySize.for
;内存大小结束计算

getMemSize:
mov dword eax, es : [edi + 8]
cmp dword eax, 0xFF_F0_00_00
ja .maxMemSize
mov dword ebx, es : [edi + 12]
cmp dword ebx, 0
je .setMemSize
.maxMemSize:
mov dword eax, 0xFF_F0_00_00
.setMemSize:
sub dword eax, 1
add dword eax, 0x100000
mov dword ebx, am + 16
mov dword [ebx], eax

mov dword ebx, am + 16
mov dword eax, [ebx]
cmp eax, 0
jne toPM

;----内存获取错误----
memE:
mov dword esi, memoryError
jmp print16
print16:
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

cli
jmp pause
;----内存获取错误----

toPM:
cli;关中断

;切换保护模式
mov dword eax, cr0
or dword eax, 0x00000001
mov dword cr0, eax

;刷新跳转
jmp 0x0018 : beforeEntryKernel ; CS使用3号GDT描述符

[bits 32];以下代码使用32bits编译
beforeEntryKernel:

;初始化所有段寄存器
mov dword eax, 0x20
mov word ds, ax
mov word es, ax
mov word gs, ax
mov word fs, ax
mov word ss, ax

;切换完成, 准备进入Kernel
;解析Kernel的ELF32结构
;am(4)      : 扇区号(33-64, 遍历根目录, 寻找"KERNEL"文件)
;am + 4(2)  : 读取扇区函数地址
;am + 6(2)  : 临时簇号保存
;am + 8(4) : 上一次读取的扇区号
;am + 12(4) : 文件断点续写地址
;am + 16(4) : 最大访问到的内存条地址
;am + 20(4) : Kernel文件的起始地址
;0x7E00 : 扇区缓存区
;0x100000 : Kernel文件区

xor dword eax, eax
mov dword ebx, am + 8
mov dword [ebx], eax;初始化am + 8(上一次读取扇区号)
xor dword eax, eax
mov dword ebx, am + 12
mov dword [ebx], eax;初始化am + 12(文件断点续写地址)

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
mov dword esi, ebx
mov dword eax, [esi + 28] ;获取文件长度
cmp dword eax, 0
je fileError
;计算文件写入地址:内存的最后一块区域
xor dword edx, edx
mov dword ecx, 512
div dword ecx
cmp dword edx, 0
mul dword ecx
je skipAdd512
add dword eax, 512
skipAdd512:
mov dword ebx, am + 16
mov dword ecx, [ebx]
sub ecx, eax
mov dword ebx, am + 12
mov dword [ebx], ecx
mov dword ebx, am + 20
mov dword [ebx], ecx

;获取起始簇号
xor dword eax, eax
mov word ax, [esi + 26] ;获取起始簇号
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
mov dword edi, 0x7E00
mov dword ebx, eax
call readSector32

skipReadFAT:
;按簇号读取文件内容到目标内存中
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

;ELF32文件头结构
; 内存偏移   长度    名称           含义                        选项
; 0 ~ 15      16     e_ident        Magic数字和其他信息
; 16 ~ 17      2     e_type         对象文件类型                0: 未知, 1: 重定向, 2: 可执行, 3: 共享对象, 4: 核心
; 18 ~ 19      2     e_machine      处理器架构                  0: 未知, 1: AT&T WE 32100, 2: SPARC, 3: Intel Architecture, 4: Motorola 68000, 5: Motorola 88000, 7: Intel 80860, 8: MIPS RS3000 Big-Endian, 10: MIPS RS4000 Big-Endian
; 20 ~ 23      4     e_version      文件版本号                  1
; 24 ~ 27      4     e_entry        程序入口
; 28 ~ 31      4     e_phoff        程序头表在文件中的偏移
; 32 ~ 35      4     e_shoff        节头表在文件中的偏移
; 36 ~ 39      4     e_flags        标志                        (暂不知道用法)
; 40 ~ 41      2     e_ehsize       ELF头大小
; 42 ~ 43      2     e_phentsize    程序头表中一个元素的大小
; 44 ~ 45      2     e_phnum        程序头表元素数量
; 46 ~ 47      2     e_shentsize    节头表中一个元素的大小
; 48 ~ 49      2     e_shnum        节头表中元素数量
; 50 ~ 51      2     e_shstrndx     节名称字符串表关联的条目的节头表索引
;共52个字节长度

;在ELF32文件头中, Magic包含4个字段信息
; 内存偏移   长度   名称         含义       选项
; 0 ~ 3        4    EL_MAG       文件标识   7f 45 4c 46
; 4            1    EL_CLASS     文件位数   0: 无效, 1: 32位, 2: 64位
; 5            1    EL_DATA      数据编码   0: 无效编码, 1: 小端序, 2: 大端序
; 6            1    EL_VERSION   文件版本   1
; 7 ~ 15       9    忽略

;----确定Magic---
mov dword ebx, am + 20
mov dword esi, [ebx];获取文件开始地址
mov dword eax, [esi]
cmp dword eax, 0x464C457F
jne kernelFileError
mov dword eax, [esi + 4]
cmp dword eax, 0x00010101
jne kernelFileError
;----确定Magic----

;----确定文件类型和处理器平台----
mov dword eax, [esi + 16]
cmp dword eax, 0x00030002
jne kernelFileError
;----确定文件类型和处理器平台----

;----确定文件版本号必须是1----
mov dword eax, [esi + 20]
cmp dword eax, 0x00000001
jne kernelFileError
;----确定文件版本号必须是1----

;----遍历节头表, 为Kernel程序分配内存----
;节头表
; 内存偏移   长度   名称            含义           选项
; 0 ~ 3        4    sh_name         节名           节名在节名字符表中的偏移
; 4 ~ 7        4    sh_type         节类型
; 8 ~ 11       4    sh_flags        段标志
; 12 ~ 15      4    sh_addr         虚拟地址       如果是需要分配内存的节, 表示该段需要被加载到的内存虚拟基地址
; 16 ~ 19      4    sh_offset       偏移           节在文件中的偏移
; 20 ~ 23      4    sh_size         节长度
; 24 ~ 27      4    sh_link         链接下标
; 28 ~ 31      4    sh_info         其他信息
; 32 ~ 35      4    sh_addralign    对齐
; 36 ~ 39      4    sh_entsize      节内成员大小   如果节内成员参差不齐, 该项为0
;共40个字节

mov dword ebx, [esi + 32];获取节头表在文件中的偏移
xor dword ecx, ecx
mov word cx, [esi + 48];获取一个节头表元素个数
sub dword ecx, 1;去掉第一个空节头表
add dword ebx, esi;文件基地址+节头在文件中的偏移
add dword ebx, 40;跳过第一个空节头表

;开始遍历, 如果需要分配内存, 并且不是NOBITS, 则把文件内容复制到对应虚拟内存中
;如果是NOBITS, 全0填充, 不读取文件内容
sectionTrvs:
;先判断标志是否需要分配内存
mov dword eax, [ebx + 8]
and dword eax, 0x02
cmp dword eax, 0x02;是否需要分配内存
jne .continue;如果不需要则跳过
mov dword eax, [ebx + 12];获取虚拟地址
cmp dword eax, 0
je kernelFileError;如果需要分配内存且虚拟地址为0, kernel文件错误
mov dword edi, ecx;暂存ecx的值到edi
mov dword ecx, [ebx + 32];获取对齐整数
xor dword edx, edx;清空edx
div dword ecx;eax / ecx = eax ... edx
cmp dword edx, 0;确保地址能被对齐整除
jne kernelFileError;如果不能被整除, 否则kernel文件错误
mul dword ecx;eax * ecx = edx eax
mov dword ecx, edi;从edi还原ecx的值
mov dword edi, eax;将虚拟内存地址放入目标指针寄存器
mov dword edx, ecx;暂存ecx的值到edx
mov dword ecx, [ebx + 20];获取节长度
mov dword eax, [ebx + 4];获取节类型
cmp dword eax, 8
je .skipCopyFile;如果是NOBITS, 跳过复制文件
mov dword esi, am + 20
mov dword esi, [esi];获取文件起始地址
mov dword eax, [ebx + 16];获取文件偏移
add dword esi, eax;偏移
.copyFile:
xor dword eax, eax
mov byte al, [esi]
mov byte [edi], al
add dword edi, 1
add dword esi, 1
loop .copyFile
mov dword ecx, edx;从edx还原ecx的值
jmp .continue
.skipCopyFile:;全部填0
xor dword eax, eax
.fill0:
mov byte [edi], al
add dword edi, 1
loop .fill0
mov dword ecx, edx;从edx还原ecx的值
.continue:
add dword ebx, 40
loop sectionTrvs
;----遍历节头表, 为Kernel程序分配内存----

;分配完成后进入kernel
mov dword esi, am + 20
mov dword esi, [esi]
mov dword eax, [esi + 24]
mov dword esi, am + 16
mov dword ebx, [esi]
;清空寄存器
xor dword ecx, ecx
xor dword edx, edx
xor dword esi, esi
xor dword edi, edi
jmp eax;跳转到Kernel

jmp pause

fileNotFind:
mov dword esi, kernelNotFind
jmp print32
fileError:
mov dword esi, kernelFileError
jmp print32
print32:
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

memoryError:
db 'The memory is Error.', 0x00

am:
;GDTR数据, 对齐之后和用于存放程序数据
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
