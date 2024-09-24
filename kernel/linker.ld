ENTRY(main)
SECTIONS {
    __gxx_personality_v0 = 0;
    _Unwind_Resume = 0;
    __cxa_throw_bad_array_new_length = 0;

    . = 0x00100000;

    .hhkInitText BLOCK(4K) : {
        * (.multiboot)
        obj/src/arch/x86/hhkInit/*.o (.hhkInitText)
    }
    .hhkInitBss BLOCK(4K) : {
        obj/src/arch/x86/hhkInit/*.o (hhkInitBss)
    }
    .hhkInitData BLOCK(4K) : {
        obj/src/arch/x86/hhkInit/*.o (.hhkInitData)
    }
    .hhkInitRodata BLOCK(4K) : {
        obj/src/arch/x86/hhkInit/*.o (.hhkInitRodata)
    }
    hhkInitEnd = ALIGN(4k);

    . += 0xC0000000;

    .text BLOCK(4K) : AT(ADDR(.text) - 0xC0000000) {
        kernelStart = .;
        obj/src/arch/x86/kernel/*.o (.text)
        obj/src/kernel/*.o (.text)
    }
    .bss BLOCK(4K) : AT(ADDR(.bss) - 0xC0000000) {
        obj/src/arch/x86/kernel/*.o (.bss)
        obj/src/kernel/*.o (.bss)
    }
    .data BLOCK(4K) : AT(ADDR(.data) - 0xC0000000) {
        obj/src/arch/x86/kernel/*.o (.data)
        obj/src/kernel/*.o (.data)
    }
    .rodata BLOCK(4K) : AT(ADDR(.rodata) - 0xC0000000) {
        obj/src/arch/x86/kernel/*.o (.rodata)
        obj/src/kernel/*.o (.rodata)
    }
    .kpg BLOCK(4K) : AT(ADDR(.kpg) - 0xC0000000) {
        obj/src/arch/x86/hhkInit/*.o (.kpg)
        obj/src/arch/x86/kernel/*.o (.kpg)
        obj/src/kernel/*.o (.kpg)
    }
    kernelEnd = ALIGN(4k);
}
