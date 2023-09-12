
export src=HixleonOS
export arch=x86

rm -rf HixleonOS.img
rm -rf ${src}/boot/${arch}/LOADER
rm -rf ${src}/boot/${arch}/mbr

cd ${src}
make
cd ..

nasm ${src}/boot/${arch}/LOADER.asm
nasm ${src}/boot/${arch}/mbr.asm

dd if=/dev/zero of=HixleonOS.img bs=1M count=4
dd conv=notrunc if=${src}/boot/${arch}/mbr of=HixleonOS.img

mkdir tmp
mount HixleonOS.img tmp/
cp ${src}/boot/${arch}/LOADER tmp/LOADER
cp ${src}/KERNEL tmp/KERNEL
umount -R tmp
rm -rf tmp
