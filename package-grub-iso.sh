#!/bin/bash

rm -rf HixleonOS-grub.iso
mkdir -p iso/boot/grub
cp boot/grub/grub.cfg iso/boot/grub
cp kernel/KERNEL iso
grub-mkrescue -o HixleonOS-grub.iso iso
rm -rf iso
