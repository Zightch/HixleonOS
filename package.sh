
export src=006

cd ${src}
make clear
make
cd ..

rm -rf HixleonOS.img
rm -rf ${src}/LOADER
rm -rf ${src}/mbr

nasm ${src}/mbr.asm
nasm ${src}/LOADER.ASM

dd if=/dev/zero of=HixleonOS.img bs=1M count=4
dd conv=notrunc if=${src}/mbr of=HixleonOS.img

mount HixleonOS.img tmp/
cp ${src}/LOADER tmp/LOADER
cp ${src}/KERNEL.BIN tmp/KERNEL
umount -R tmp
