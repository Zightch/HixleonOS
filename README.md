# HixleonOS
一个使用汇编和C/C++实现的简易的32位操作系统  

## 构建项目
### 环境
1. 首先确保你的电脑是Linux或有一台Linux的虚拟机
2. 安装软件包
   * g++
   * nasm
   * make
   * kpartx
   * xorriso
   * grub2
   * grub-efi-ia32
   * grub-efi-amd64 (可选)
   * parted (可选)
3. 安装其他软件包
    * [i686-elf](https://sxjc1.staticplant.top:6001/i686-elf.tar.gz)  

   将如上软件包放置/opt下, 并加入环境变量
4. (可选) 下载qemu uefi固件
   * [OVMF-amd64.fd](https://sxjc1.staticplant.top:6001/OVMF-amd64.fd) (适用于`qemu-system-x86_64`)
   * [OVMF-ia32.fd](https://sxjc1.staticplant.top:6001/OVMF-ia32.fd) (适用于`qemu-system-i386`)

### 构建
1. cd到项目所在目录
2. 执行`./package-XXX.sh`, 其中`XXX`对应不同的方式  

### 运行
针对不同的package方式, 有不同的运行特性
* `package-grub-iso.sh`直接打包iso  
  通过刻录U盘后或`qemu`可启动
* `package-legacy.sh`直接打包img  
  现代计算机无法运行  
  可以通过dd写盘到老式ide磁盘在老式电脑上启动
* `package-grub-img.sh`接受一个参数为`platform`  
  `platform`对应为`grub-install`的`--target`  
  通过`qemu`启动
* `package-grub-disk.sh`接受两个参数为`platform`和`device`  
  `platform`对应为`grub-install`的`--target`  
  `device`对应为你要安装到哪个磁盘设备  
  完成安装后可在现代计算机上运行

若物理机显示内容与虚拟机相似, 即表示运行无误

## 联系方式
QQ: 2166825850  
邮箱: Zightch@163.com

## 开发参考
* [Lunaixsky - 从零开始自制操作系统](https://www.bilibili.com/video/BV1zv4y1g7J3/)
* [成宇佳GrapeOS - 自己动手从零写桌面操作系统GrapeOS系列教程](https://www.bilibili.com/video/BV1nv4y1b7Nj/)
* [OSDev - i686-elf交叉编译套件](https://wiki.osdev.org/GCC_Cross-Compiler)
* [小甲鱼 - 零基础入门学习汇编语言](https://www.bilibili.com/video/BV1Rs411c7HG/)
