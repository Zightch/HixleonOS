#!/bin/bash

# 检查是否提供了足够的参数
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <platform> <device>"
    exit 1
fi

PLATFORM=$1
DEVICE=$2

make clean

make mkdir
make -j$(nproc)

# 删除旧的分区表
sudo fdisk $DEVICE << EOF
g
p
w
EOF

# 创建新的分区表
sudo fdisk $DEVICE << EOF
n
p
1

+512M
t
1
p
w
EOF

export PART=${DEVICE}1
sudo mkfs.fat -F 16 -I $PART
mkdir tmp
sudo mount $PART tmp

sudo grub-install --boot-directory=tmp/boot --efi-directory=tmp --no-floppy --target=$PLATFORM --removable $DEVICE

sudo cp boot/grub/grub.cfg tmp/boot/grub
sudo cp kernel/KERNEL tmp

sudo umount -R tmp
rm -rf tmp
