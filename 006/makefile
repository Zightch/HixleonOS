Kernel: kernel.o tty.o boot.o GDT.o
	i686-elf-g++ build/boot.o build/kernel.o build/tty.o build/GDT.o -o KERNEL.BIN -g -nostdlib -O0 -T linker.ld

kernel.o: kernel/kernel.cpp
	i686-elf-g++ -c kernel/kernel.cpp -o build/kernel.o -g -O0 -I ./ -nostdlib

tty.o: kernel/tty/tty.cpp
	i686-elf-g++ -c kernel/tty/tty.cpp -o build/tty.o -g -O0 -I ./ -nostdlib

boot.o: arch/x86/boot.S
	i686-elf-g++ -c arch/x86/boot.S -o build/boot.o -g -O0 -I ./ -nostdlib

GDT.o: arch/x86/GDT.cpp
	i686-elf-g++ -c arch/x86/GDT.cpp -o build/GDT.o -g -O0 -I ./ -nostdlib

clear:
	rm -rf build/*
	rm -rf KERNEL.BIN
