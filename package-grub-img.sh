#!/bin/bash

# 检查是否提供了平台参数
if [ -z "$1" ]; then
    echo "Usage: $0 <platform>"
    exit 1
fi

PLATFORM=$1

rm -rf HixleonOS-grub.img
make clean

make mkdir
make -j$(nproc)

dd if=/dev/zero of=HixleonOS-grub.img bs=512M count=1

fdisk HixleonOS-grub.img << EOF
g
n
1


t
1
p
w
EOF

sudo losetup /dev/loop7 HixleonOS-grub.img
sudo kpartx -av /dev/loop7

sudo mkfs.fat -F 16 -I /dev/mapper/loop7p1
mkdir tmp
sudo mount /dev/mapper/loop7p1 tmp

sudo grub-install --boot-directory=tmp/boot --efi-directory=tmp --no-floppy --target=$PLATFORM --removable /dev/loop7

sudo cp boot/grub/grub.cfg tmp/boot/grub
sudo cp kernel/KERNEL tmp

sudo umount -R tmp
rm -rf tmp
sudo kpartx -dv /dev/loop7
sudo losetup -d /dev/loop7
