
export arch=x86

rm -rf HixleonOS.img
rm -rf boot/${arch}/LOADER
rm -rf boot/${arch}/mbr

cd kernel
make
cd ..

nasm boot/${arch}/LOADER.ASM
nasm boot/${arch}/mbr.asm

dd if=/dev/zero of=HixleonOS.img bs=1M count=4
dd conv=notrunc if=boot/${arch}/mbr of=HixleonOS.img

mkdir tmp
mount HixleonOS.img tmp
cp boot/${arch}/LOADER tmp/LOADER
cp kernel/kernel.bin tmp/KERNEL
umount -R tmp
rm -rf tmp
