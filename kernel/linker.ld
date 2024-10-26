ENTRY(main)
SECTIONS {
    __gxx_personality_v0 = 0;
    _Unwind_Resume = 0;
    __cxa_throw_bad_array_new_length = 0;

    . = 0x00100000;

    .hhkInitText BLOCK(4K) : {
        * (.multiboot)
        * (.hhkInitText)
    }
    .hhkInitBss BLOCK(4K) : {
        * (.hhkInitBss)
    }
    .hhkInitData BLOCK(4K) : {
        * (.hhkInitData)
    }
    .hhkInitRodata BLOCK(4K) : {
        * (.hhkInitRodata)
    }
    hhkInitEnd = ALIGN(4k);

    . += 0xC0000000;

    .text BLOCK(4K) : AT(ADDR(.text) - 0xC0000000) {
        kernelStart = .;
        * (.text)
    }
    .bss BLOCK(4K) : AT(ADDR(.bss) - 0xC0000000) {
        * (COMMON)
        * (.bss)
    }
    .data BLOCK(4K) : AT(ADDR(.data) - 0xC0000000) {
        * (.data)
    }
    .rodata BLOCK(4K) : AT(ADDR(.rodata) - 0xC0000000) {
        * (.rodata)
    }
    .kpg BLOCK(4K) : AT(ADDR(.kpg) - 0xC0000000) {
        * (.kpg)
    }
    kernelEnd = ALIGN(4k);
}
