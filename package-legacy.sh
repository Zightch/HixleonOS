#!/bin/bash

rm -rf HixleonOS-legacy.img
rm -rf boot/legacy/LOADER
rm -rf boot/legacy/mbr
make clean

make mkdir
make -j$(nproc)

nasm boot/legacy/LOADER.ASM
nasm boot/legacy/mbr.asm

dd if=/dev/zero of=HixleonOS-legacy.img bs=16M count=1
dd conv=notrunc if=boot/legacy/mbr of=HixleonOS-legacy.img

mkdir tmp
sudo mount HixleonOS-legacy.img tmp
sudo cp boot/legacy/LOADER tmp/LOADER
sudo cp kernel/KERNEL tmp/KERNEL
sudo umount -R tmp
rm -rf tmp
