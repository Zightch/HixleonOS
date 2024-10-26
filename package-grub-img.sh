#!/bin/bash

rm -rf HixleonOS-grub.img

dd if=/dev/zero of=HixleonOS-grub.img bs=256M count=1

fdisk HixleonOS-grub.img << EOF
o
n
p
1


a
t
83
p
w
EOF

sudo losetup /dev/loop7 HixleonOS-grub.img
sudo kpartx -av /dev/loop7

sudo mkfs.ext2 /dev/mapper/loop7p1
mkdir tmp
sudo mount /dev/mapper/loop7p1 tmp

sudo grub-install --boot-directory=tmp/boot --no-floppy --target=i386-pc /dev/loop7

sudo cp boot/grub/grub.cfg tmp/boot/grub
sudo cp kernel/KERNEL tmp

sudo umount -R tmp
rm -rf tmp
sudo kpartx -dv /dev/loop7
sudo losetup -d /dev/loop7
