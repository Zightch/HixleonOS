# HixleonOS
一个使用汇编和C/C++实现的简易操作系统  

## 构建项目
### 前情提要
1. 首先确保你的电脑是Linux或有一台Linux的虚拟机
2. 系统中具有nasm, i686-elf-g++, qemu, make软件

### 准备好后
构建
1. 在克隆仓库后打开终端
2. cd到项目所在目录
3. 执行`package.sh`
4. 执行`qemu-system-i386 HixleonOS.img -m 3G`

物理机运行
1. 插入IDE/SATA硬盘, 可以使用`IDE/SATA To USB`转接器进行外接
2. 使用`fdisk -l`寻找你的硬盘(这里假设你的磁盘是/dev/sda)
3. 使用`dd if=HixleonOS.img of=/dev/sda`将操作系统写入硬盘
4. 将硬盘移到测试用物理机上, 开机

若物理机显示内容与虚拟机相似, 即表示运行无误

## 说明
* 本操作系统使用Legacy MBR引导方式, FAT16文件系统  
  具体内容在[./boot/x86/](/boot/x86/)
* 本操作系统使用Legacy IDE磁盘控制器, 将无法在NVMe等更高级的磁盘上运行  
  会导致`The loader was not found. File in "/LOADER".`  
  这也就意味着如果你使用的是SATA, 需要开机时进BIOS设置SATA控制器为Legacy IDE
* 本操作系统为32位操作系统, 最大寻址到4GB
* 本系统仅限于学习和交流开发使用, 不可进行商业用途  
* 如有疑问, [请联系作者](#联系方式)

## 联系方式
QQ: 2166825850  
邮箱: Zightch@163.com

## 开发参考
* [Lunaixsky - 从零开始自制操作系统](https://www.bilibili.com/video/BV1zv4y1g7J3/)
* [成宇佳GrapeOS - 自己动手从零写桌面操作系统GrapeOS系列教程](https://www.bilibili.com/video/BV1nv4y1b7Nj/)
* [OSDev - i686-elf交叉编译套件](https://wiki.osdev.org/GCC_Cross-Compiler)
* [小甲鱼 - 零基础入门学习汇编语言](https://www.bilibili.com/video/BV1Rs411c7HG/)
