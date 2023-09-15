org 0x8000

;内存区段            大小         含义
;0x0500 ~ 0x7bff     30463B       可用内存区
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
mov dword eax, am + 0x200 ;留512B大小内存
mov dword ecx, 0x1000
div dword ecx
cmp dword edx, 0
mul dword ecx
je setGDT.else
add dword eax, 0x1000
setGDT.else:
mov dword ebx, am
mov dword ecx, 56 ;表大小
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

;开始获取内存地图
xor dword esi, esi
xor dword ebx, ebx ;下是否有下一个表
mov dword edi, am + 22 ;信息存放地址
mov dword ecx, 20 ;表大小
mov dword edx, 0x534d4150 ;表标识符
mov dword eax, 0xe820 ;中断号
; 获取内存信息
getMemorySize.for:
int 0x15
jc memE
cmp dword edx, 0x534d4150 ;比较edx和内存映射表的签名是否相同
jne memE ;如果不相同, 跳转到错误处理
mov dword eax, 0xe820 ;中断号
add dword esi, 1
add dword edi, ecx
cmp dword ebx, 0
jne getMemorySize.for
mov dword ebx, am + 20
mov word [ebx], si ;写入内存地图长度
jmp toPM
;内存地图获取结束

;----内存获取错误----
memE:
mov dword esi, memoryErrorStr
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
out dx, byte al ;设置为光标低8位

mov dword edx, 0x03D5
sub dword esi, ecx
mov dword eax, esi
out dx, byte al ;设置光标低8位

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
jmp 0x0018 : beforeEntryKernel ;CS使用3号GDT描述符

[bits 32] ;以下代码使用32bits编译
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
;am + 16(4) : Kernel文件的起始地址
;am + 20(2) : 内存地图长度
;am + 22 : 内存地图

xor dword eax, eax
mov dword ebx, am + 8
mov dword [ebx], eax ;初始化am + 8(上一次读取扇区号)
xor dword eax, eax
mov dword ebx, am + 12
mov dword [ebx], eax ;初始化am + 12(文件断点续写地址)

mov dword ebx, esp
mov word ax, [ebx] ;取出readSector函数地址
mov dword ebx, am + 4
mov word [ebx], ax ;把地址转写到am + 4的地址上

mov dword eax, 33
mov dword ebx, am
mov dword [ebx], eax ;写入扇区号

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
mov dword edi, kernelFileNameStr ;di指向我的文件名
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

;文件起始位置
;计算文件写入地址:内存的最后一块区域
;先找到0x100000开始的内存长度, 并计算
get1MBLimit:
xor dword ecx, ecx
mov dword ebx, am + 20
mov word cx, [ebx]
mov dword ebx, am + 22
.find1MB:
mov dword edx, [ebx]
cmp dword edx, 0x100000
jne .continue
mov dword edx, [ebx + 4]
cmp dword edx, 0
jne .continue
jmp .break
.continue:
add dword ebx, 20
loop .find1MB
jmp 0x0008 : memE ;如果没有0x100000内存段
.break:

mov dword edi, eax ;暂存eax的值到edi

mov dword eax, [ebx + 8]
cmp dword eax, 0xFF_F0_00_00
ja .maxMemSize
mov dword ecx, [ebx + 12]
cmp dword ecx, 0
je .setMemSize
.maxMemSize:
mov dword eax, 0xFF_F0_00_00
.setMemSize:
sub dword eax, 1
add dword eax, 0x100000

sub dword eax, edi

xor dword edx, edx
mov dword ecx, 512
div dword ecx
mul dword ecx
mov dword ebx, am + 16
mov dword [ebx], eax
mov dword ebx, am + 12
mov dword [ebx], eax

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
je skipReadFAT ;如果上一次读的扇区号和这一次相同, 跳过读取扇区

;读取FAT1表到0x7e00中
mov dword ebx, am + 8
mov dword [ebx], eax
mov dword edi, 0x7E00
mov dword ebx, eax
call readSector32

skipReadFAT:
;按簇号读取文件内容到目标内存中
mov dword ebx, am + 12
mov dword edi, [ebx] ;读取断点续写地址
mov word bx, am + 6
mov word bx, [ebx] ;从可用内存段里读取临时簇号
add word bx, 63 ;bx += 63获取扇区号
call readSector32
mov dword ebx, am + 12
mov dword [ebx], edi ;更新断点续写地址

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
mov dword ebx, am + 16
mov dword esi, [ebx] ;获取文件开始地址
mov dword eax, [esi]
cmp dword eax, 0x464C457F
jne fileError
mov dword eax, [esi + 4]
cmp dword eax, 0x00010101
jne fileError
;----确定Magic----

;----确定文件类型和处理器平台----
mov dword eax, [esi + 16]
cmp dword eax, 0x00030002
jne fileError
;----确定文件类型和处理器平台----

;----确定文件版本号必须是1----
mov dword eax, [esi + 20]
cmp dword eax, 0x00000001
jne fileError
;----确定文件版本号必须是1----

;----节头表(Elf32_Shdr)----
; 内存偏移   长度   名称            含义           选项
; 0 ~ 3        4    sh_name         节名           节名在节名字符表中的偏移
; 4 ~ 7        4    sh_type         节类型
; 8 ~ 11       4    sh_flags        段标志
; 12 ~ 15      4    sh_addr         虚拟地址       虚拟基地址
; 16 ~ 19      4    sh_offset       偏移           节在文件中的偏移
; 20 ~ 23      4    sh_size         节长度
; 24 ~ 27      4    sh_link         链接下标
; 28 ~ 31      4    sh_info         其他信息
; 32 ~ 35      4    sh_addralign    对齐
; 36 ~ 39      4    sh_entsize      节内成员大小   如果节内成员参差不齐, 该项为0
;共40个字节
;----程序头表(Elf32_Phdr)----
; 内存偏移   长度   名称            含义             选项
; 0 ~ 3        4    p_type          类型             1: LOAD
; 4 ~ 7        4    p_offset        文件偏移
; 8 ~ 11       4    p_vaddr         虚拟地址
; 12 ~ 15      4    p_paddr         物理地址
; 16 ~ 19      4    p_filesz        在文件中的长度
; 20 ~ 23      4    p_memsz         所占内存大小
; 24 ~ 27      4    p_flags         flags            RWX(低3bit)
; 28 ~ 31      4    p_align         对齐
;共32个字节

;遍历所有程序头表, 将type为LOAD的段载入物理内存

mov dword ebx, [esi + 28] ;获取程序头表在文件中的偏移
xor dword ecx, ecx
mov word cx, [esi + 44] ;获取程序头表元素个数
add dword ebx, esi ;文件基地址+程序头在文件中的偏移

;开始遍历
;需要满足如下条件
;1. type == LOAD
;2. 虚拟地址和物理地址均对齐
;3. 如果文件偏移为0, 文件长度也必须为0, 否则文件长度必须与内存大小相同
procTrvs:
;条件一
;先判断type是否为LOAD
mov dword eax, [ebx]
cmp dword eax, 0x01 ;type是否为LOAD
jne .continue ;如果不是则跳过
;条件二
xor dword edx, edx ;清空edx
mov dword eax, [ebx + 12] ;获取物理地址
cmp dword eax, 0x100000
jb fileError ;如果物理地址小于1MB, kernel文件错误
div dword [ebx + 28] ;物理内存/对齐
cmp dword edx, 0 ;比较余数和
jne fileError ;如果不能被整除, kernel文件错误
xor dword edx, edx ;清空edx
mov dword eax, [ebx + 8] ;获取虚拟地址
cmp dword eax, 0
je fileError ;如果虚拟地址为0, kernel文件错误
div dword [ebx + 28] ;虚拟内存/对齐
cmp dword edx, 0 ;比较余数和
jne fileError ;如果不能被整除, kernel文件错误
;条件三
mov dword eax, [ebx + 4] ;获取文件偏移
cmp dword eax, 0 ;比较文件偏移与0
jne .term3Else ;如果不相等, 跳转到条件三else
;判断文件长度也为0
mov dword eax, [ebx + 16] ;获取在文件中的长度
cmp dword eax, 0 ;比较在文件中的长度与0
jne fileError ;如果不相等, kernel文件错误
mov dword edx, 1 ;load模式为fill 0模式
jmp .term3End
.term3Else: ;条件三else, 如果文件偏移不为0
mov dword eax, [ebx + 16] ;获取在文件中的长度
mov dword edx, [ebx + 20] ;获取所占内存大小
cmp dword eax, edx ;比较在文件中的长度与所占内存大小
ja fileError ;如果文件长度>内存大小, kernel文件错误
mov dword edx, 2 ;load模式为copy file模式
.term3End: ;条件三结束
jmp .skipRepeater

.repeater: ;loop中继跳转
jmp procTrvs

.skipRepeater:
;开始拷贝文件数据或者初始化内存
cmp dword edx, 1 ;比较load类型与fill 0模式
jne .copyFile ;否则跳转到copy file模式
mov dword edi, [ebx + 12] ;获取物理地址
mov dword eax, ecx ;暂存ecx的值到eax
mov dword ecx, [ebx + 20] ;获取所占内存大小
.fill0:
mov byte [edi], 0 ;填0
add dword edi, 1 ;累加1
loop .fill0
mov dword ecx, eax ;还原ecx的值
jmp .continue
.copyFile:
cmp dword edx, 2 ;验证load模式为copy file模式
jne loadError ;不相等跳转到加载时发生错误
mov dword ebp, ecx ;暂存ecx的值到ebp
mov dword eax, [ebx + 4] ;获取文件内的偏移
mov dword ecx, [ebx + 16] ;获取在文件中的长度
mov dword edi, [ebx + 12] ;获取物理内存地址
.memcpy:
mov byte dl, [esi + eax] ;获取文件内容
mov byte [edi], dl ;写入内存
add dword eax, 1 ;文件偏移++
add dword edi, 1 ;物理内存++
loop .memcpy
mov dword ecx, [ebx + 20] ;获取所占内存大小
mov dword eax, [ebx + 16] ;获取在文件中的长度
sub dword ecx, eax ;所占内存大小 - 在文件中的长度
.copyFile.fill0:
cmp dword ecx, 0 ;判断所占内存大小 - 在文件中的长度后还剩多少
je .copyFile.fill0End ;如果剩余0, 直接结束
mov byte [edi], 0 ;填0
add dword edi, 1 ;edi++
sub dword ecx, 1 ;ecx--
jmp .copyFile.fill0
.copyFile.fill0End:
mov dword ecx, ebp ;从ebp恢复ecx的值
.continue:
add dword ebx, 32 ;向后挪一个头表元素
loop .repeater ;循环

;分配完成后进入kernel
;eax:0x100000
;ebx:内存地图起始地址
;ecx:内存地图大小
mov dword eax, 0x7bfc
mov dword ebp, eax
mov dword esp, eax
mov dword eax, 0x20
mov word ds, ax
mov word es, ax
mov word gs, ax
mov word fs, ax
mov word ss, ax

mov dword esi, am + 16
mov dword esi, [esi]
mov dword eax, [esi + 24]
mov dword esi, am + 20
mov dword ecx, [esi]
mov dword ebx, am + 22

xor dword edx, edx
xor dword esi, esi
xor dword edi, edi
jmp eax;跳转到Kernel

jmp pause

loadError:
mov dword esi, loadErrorStr
jmp print32
fileNotFind:
mov dword esi, kernelNotFindStr
jmp print32
fileError:
mov dword esi, kernelFileErrorStr
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
out dx, byte al ;设置为光标低8位

mov dword edx, 0x03D5
sub dword esi, ecx
mov dword eax, esi
out dx, byte al ;设置光标低8位

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

kernelFileNameStr:
db 'KERNEL     ', 0x00

kernelNotFindStr:
db 'The kernel is not found. File in "/KERNEL".', 0x00

kernelFileErrorStr:
db 'The kernel file is error. File in "/KRENEL".', 0x00

memoryErrorStr:
db 'The memory is Error.', 0x00

loadErrorStr:
db 'An error occurred while loading the kernel, error code: 0x12', 0x00

am:
;GDTR数据, 对齐之后和用于存放程序数据
times 6 db 0

;GDT表数据, 对齐之后可用于存放程序数据
    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ;0: (Base:0, Limit:0, Flags:0)
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0x8f, 0x00 ;1: (Base:0, Limit:0xFFFFF, Flags:SEG_R0_CODE16)
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0x8f, 0x00 ;2: (Base:0, Limit:0xFFFFF, Flags:SEG_R0_DATA16)
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00 ;3: (Base:0, Limit:0xFFFFF, Flags:SEG_R0_CODE32)
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xcf, 0x00 ;4: (Base:0, Limit:0xFFFFF, Flags:SEG_R0_DATA32)
    db 0xff, 0xff, 0x00, 0x00, 0x10, 0x9a, 0xcf, 0x00 ;5: (Base:0x100000, Limit:0xFFFFF, Flags:SEG_R0_CODE32)
    db 0xff, 0xff, 0x00, 0x00, 0x10, 0x92, 0xcf, 0x00 ;6: (Base:0x100000, Limit:0xFFFFF, Flags:SEG_R0_DATA32)
    db 0x00
