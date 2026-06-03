OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(_start)

rmode_addr = 0x7C00;
pmode_addr = 0x10000;

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
    __rmode_seg = rmode_addr / 16;

    /* for 32-bit protected mode */

    . = pmode_addr;
   
    .text ALIGN(4096) : {
        __text_start = .;
        *(.text)
        *(.text*)
        __text_end = .;
    } :text 

    .data ALIGN(4096) : {
        __data_start = .;
        *(.data)
        *(.data*)
    } :data 

    .bss ALIGN(4096) : {
        __bss_start = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } :data 

    .rodata ALIGN(4096) : {
        __rodata_start = .;
        *(.rodata)
        *(.rodata*)
    } :text 

    .heap ALIGN(4096) : {
        _heap_start = .;
        . = . + 4 * 1024 * 1024;
        _heap_end = .;
    } :data

    . = ALIGN(4096);
    stack_bot = .;
    . += 0x4000;
    stack_top = .;

    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
        *(.note.*)
    }

    __end = .;
}
