OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(x86_64)
ENTRY(_start)

rmode_addr = 0x8000;
/* lmode_addr = 0x10000; */
lmode_addr = 0xffff800000200000;

PHDRS {
    rmode     PT_LOAD FLAGS(5);
    text      PT_LOAD FLAGS(5);
    data      PT_LOAD FLAGS(6);
}

SECTIONS {
    /* for 16-bit real mode */

    . = rmode_addr;

    .rmode ALIGN(16) : {
        __rmode_start = .;
        *(.rmode)
        *(.rmode*)
        __rmode_end = .;
    } :rmode

    __rmode_size = __rmode_end - __rmode_start;
    __rmode_seg = rmode_addr / 16; /* 0x800 */

    /* for 64-bit long mode */

    . = lmode_addr;

    .text ALIGN(4096) : {
        __text_start = .;
        *(.text._start)
        *(.text)
        *(.text*)
        __text_end = .;
    } :text

    .data ALIGN(4096) : {
        __data_start = .;
        *(.data)
        *(.data*)
        __data_end = .;
    } :data

    .rodata ALIGN(4096) : {
        __rodata_start = .;
        *(.rodata)
        *(.rodata*)
        __rodata_end = .;
    } :text

    .bss ALIGN(4096) : {
        __bss_start = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } :data

    .stack (NOLOAD) : ALIGN(16) {
        stack_bot = .;
        . += 0x10000;
        stack_top = .;
    }

    .heap (NOLOAD) : ALIGN(4096) {
        _heap_start = .;
        . = . + 256 * 1024;
        _heap_end = .;
    } :data

    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note.*)
    }

    __end = .;
}
