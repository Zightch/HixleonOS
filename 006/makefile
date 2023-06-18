oArgs+=-g -O0 -I ./includes -nostdlib
eArgs+=-g -nostdlib -O0 -T linker.ld

Kernel: kernel.o tty.o boot.o GDT.o ttySetCursor.o
	i686-elf-g++ build/boot.o build/kernel.o build/tty.o build/GDT.o build/ttySetCursor.o -o KERNEL.BIN ${eArgs}

kernel.o: kernel/kernel.cpp
	i686-elf-g++ -c kernel/kernel.cpp -o build/kernel.o ${oArgs}

ttySetCursor.o:
	i686-elf-g++ -c kernel/tty/ttySetCursor.S -o build/ttySetCursor.o ${oArgs}

tty.o: kernel/tty/tty.cpp
	i686-elf-g++ -c kernel/tty/tty.cpp -o build/tty.o ${oArgs}

boot.o: arch/x86/boot.S
	i686-elf-g++ -c arch/x86/boot.S -o build/boot.o ${oArgs}

GDT.o: arch/x86/GDT.cpp
	i686-elf-g++ -c arch/x86/GDT.cpp -o build/GDT.o ${oArgs}

clear:
	rm -rf build/*
	rm -rf KERNEL.BIN
