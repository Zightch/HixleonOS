#!/bin/bash

rm -rf HixleonOS.img
rm -rf boot/legacy/LOADER
rm -rf boot/legacy/mbr

cd kernel || exit
make
cd ..

nasm boot/legacy/LOADER.ASM
nasm boot/legacy/mbr.asm

dd if=/dev/zero of=HixleonOS.img bs=16M count=1
dd conv=notrunc if=boot/legacy/mbr of=HixleonOS.img

mkdir tmp
sudo mount HixleonOS.img tmp
sudo cp boot/legacy/LOADER tmp/LOADER
sudo cp kernel/KERNEL tmp/KERNEL
sudo umount -R tmp
rm -rf tmp
