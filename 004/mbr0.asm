org 0x7c00

;目录项(32个字节)
;名称            偏移  长度    含义
;DIR_Name          0   11      文件名(长度8个字节, 扩展名3个字节)
;DIR_Attr         11    1      目录类型(0x10文件夹, 0x20文件)
;DIR_Reservedl    12   10      保留位
;DIR_WrtTime      22    2      最后一次写入时间
;DIR_WrtDate      24    2      最后一次写入日期
;DIR_FstClus      26    2      起始簇号
;DIR_FileSize     28    4      文件大小

;内存区段            大小                含义
;0x0500 ~ 0x2fff    11008B (10.75KB)    栈内存区
;0x3000 ~ 0x7bff    19456B (19KB)       文件内容区
;0x7c00 ~ 0x7dff    512B   (0.5KB)      MBR扇区
;0x7e00 ~ 0x8000    512B   (0.5KB)      扇区缓存区
;4MB磁盘容量
;FAT1表占用扇区数32
;根目录占用扇区数32
;数据区起始扇区号65, 对应簇号为2
;簇号 + 63 = 扇区号
;一个扇区最多存16个目录项

;区域名     扇区数    扇区号(范围)         字节偏移(范围)
;引导扇区       1               0        0x0000 ~ 0x01ff
;FAT1表        32          1 ~ 32        0x0200 ~ 0x41ff
;FAT2表        --              --                     --
;根目录区      32         33 ~ 64        0x4200 ~ 0x81ff
;数据区      8127     65 ~ 0x1fff      0x8200 ~ 0x3fffff

;短跳转指令(偏移0, 长度3)
jmp start
db 0

;FAT16引导扇区表                    偏移     长度   名称             含义
db 'Zightch', 0                 ;    3       8     BS_OEMName        厂商名称
db 0x00, 0x02                   ;   11       2     BPB_BytsPerSec    每扇区字节数
db 0x01                         ;   13       1     BPB_SecPerClus    每簇扇区数
db 0x01, 0x00                   ;   14       2     BPB_RsvdSecCnt    引导扇区的扇区数
db 0x01                         ;   16       1     BPB_NumFATs       FAT表个数
db 0x00, 0x02                   ;   17       2     BPB_RootEntCnt    根目录可容纳的目录项数
db 0x00, 0x20                   ;   19       2     BPB_TotSec16      扇区总数
db 0xf8                         ;   21       1     BPB_Media         介质描述符
db 0x20, 0x00                   ;   22       2     BPB_FATSz16       每个FAT表扇区数
db 0x20, 0x00                   ;   24       2     BPB_SecPerTrk     每磁道扇区数
db 0x40, 0x00                   ;   26       2     BPB_NumHeads      磁头数
db 0x00, 0x00, 0x00, 0x00       ;   28       4     BPB_HiddSec       隐藏扇区数
db 0x00, 0x00, 0x00, 0x00       ;   32       4     BPB_TotSec32      如果BPB_TotSec16为0, 由这个值记录扇区数
db 0x80                         ;   36       1     BS_DrvNum         int 13h的驱动器号
db 0x00                         ;   37       1     BS_Reservedl      未使用
db 0x29                         ;   38       1     BS_BootSig        扩展引导标记
db 0x00, 0x00, 0x00, 0x00       ;   39       4     BS_VolID          卷序列号
db 'HixleonOS', 0x00, 0x00      ;   43      11     BS_VolLab         卷标
db 'FAT16', 0x00, 0x00, 0x00    ;   54       8     BS_FileSysType    文件系统类型

;引导程序,数据等(偏移62, 长度448)
start:

;清空屏幕
mov di, 0
mov ax, 0xb800
mov es, ax
mov ax, 0x0700
mov cx, 2000
cls.for:
mov [es : di], ax
add di, 2
loop cls.for

;初始化段寄存器
mov ax, cs
mov ds, ax
mov es, ax
mov ax, 0
mov ss, ax
mov eax, 0x00002ffe
mov ebp, eax
mov esp, eax

;读取根目录
mov di, 0x7e00
mov ebx, 33
call readSector

;查找文件
cld ;repe cmpsb字节数组比较时si, di递增, std命令相反(递减)
mov bx, 0 ;用bx记录遍历第几个目录项
nextDir:
mov si, bx
shl si, 5 ;乘以32(目录项大小)
add si, 0x7e00 ;si指向扇区缓存区
mov di, myFileName ;di指向我的文件名
mov cx, 11 ;文件名长度11
repe cmpsb ;逐字节比较
jcxz fileFind ;如果字节匹配相同跳转到文件已找到
add bx, 1 ;bx += 1
cmp bx, 16
jl nextDir ;如果bx < 16, 跳转到nextDir
jmp fileNotFind ;跳转到文件未找到

;文件已找到
fileFind:
shl bx, 5 ;乘以32
add bx, 0x7e00
mov ax, [bx + 26] ;获取起始簇号
mov bx, usable
mov [bx], ax ;保存在临时内存中

;读取FAT1表到0x7e00中
mov di, 0x7e00
mov ebx, 1
call readSector

mov di, 0x3000 ;初始化di为3000
readFileNext:
;按簇号读取文件内容到0x1000中
mov bx, usable
mov bx, [bx] ;从可用内存段里读取临时簇号
cmp bx, 0xfff7 ;比较0xfff7
ja readFileEnd ;如果>0xfff7(说明>=0xfff8)跳转到读取文件结束
add bx, 63 ;bx += 63获取扇区号
call readSector

;读取下一个簇号
mov bx, usable
mov bx, [bx] ;从可用内存段里读取临时簇号
shl bx, 1 ;乘以2
add bx, 0x7e00 ;指向下一个簇号的地址
mov ax, [bx]
mov bx, usable
mov [bx], ax ;临时保存簇号
jmp readFileNext

;文件读取结束
readFileEnd:
mov eax, 0
mov ebx, 0
mov ecx, 0
mov edx, 0
mov esi, 0
mov edi, 0
mov es, ax
mov ds, ax
mov ss, ax
mov eax, 0x00002ffe
mov ebp, eax
mov esp, eax
mov eax, 0
jmp 0x0000 : 0x3000

;文件未找到
fileNotFind:
mov si, loaderNotFind
mov ax, 0xb800
mov es, ax
mov di, 0
mov ah, 0x07 ;黑底白字
print.for:
mov al, [si] ;ds:si取出一个字节
cmp al, 0 ;比较al与0
jz print.end ;如果相等跳转到.end
mov [es : di], ax
add si, 1
add di, 2
jmp print.for ;跳转到for
print.end:

pause:
hlt
jmp pause

readSector:
;输入:di扇区缓存区, ebx扇区号
;第一步，检查硬盘控制器状态
.notReady1:
mov dx, 0x01f7
in al, dx ;读0x01f7端口
and al, 0xc0 ;第7位与第6位
cmp al, 0x40 ;第7位为0，第6位为1
jne .notReady1 ;如果不相等跳转到notReady1

;第二步，设置要读取的扇区数量
mov al, 1
mov dx, 0x01f2
out dx, al ;设置读取一个扇区

;第三步，设置扇区地址(LBA)
mov eax, ebx
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

;第五步，向0x01f7写入0x20表示读取
mov dx, 0x01f7
mov al, 0x20
out dx, al

;第六步，检查硬盘控制器状态
.notReady2:
mov dx, 0x01f7
in al, dx ;读0x01f7端口
and al, 0x88 ;第7位与第3位
cmp al, 0x08 ;第7位为0，第3位为1
jne .notReady2 ;如果不相等跳转到notReady2

;第7步，循环读取256次数据写到di指向的内存中
mov dx, 0x01f0
mov cx, 256
.readData:
in ax, dx
mov [di], ax
add di, 2
loop .readData
ret

myFileName:
db 'LOADER  BIN', 0x00

loaderNotFind:
db 'The loader was not found. File in "LOADER.BIN".', 0x00

usable:

times 510-($-$$) db 0

;结束标志(偏移510, 长度2)
db 0x55, 0xaa
