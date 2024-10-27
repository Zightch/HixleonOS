#!/bin/bash

# 检查是否提供了设备参数
if [ -z "$1" ]; then
    echo "Usage: $0 <device>"
    exit 1
fi

export DEVICE=$1

make clean

make mkdir
make -j$(nproc)

# 删除分区表
sudo fdisk $DEVICE << EOF
o
p
w
EOF

# 构建分区表
sudo fdisk $DEVICE << EOF
n
p
1

+2G
a
t
6
p
w
EOF

export PART=${DEVICE}1
sudo mkfs.fat -F 16 -I $PART
mkdir tmp
sudo mount $PART tmp

sudo grub-install --boot-directory=tmp/boot --no-floppy --target=i386-pc $DEVICE

sudo cp boot/grub/grub.cfg tmp/boot/grub
sudo cp kernel/KERNEL tmp

sudo umount -R tmp
rm -rf tmp
