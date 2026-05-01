OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x100000;

    .multiboot ALIGN(4) : {
        *(.multiboot)
    } :text
 
    .fsjump : { *(.fsjump) }
    .fsheaders : { *(.fsheaders) }

    .text ALIGN(16384) : {
        __text_start = .;
        *(.text)
        *(.text*)
    } :text

    .data ALIGN(4096) : {
        __data_start = .;
        *(.data)
        *(.data*)
    } :data

    .rodata ALIGN(4) : {
        __rodata_start = .;
        *(.rodata)
        *(.rodata*)
    } :data

    .bss ALIGN(32) : {
        __bss_start = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } :data

    .heap ALIGN(4) : {
        _heap_start = .;
        . = . + 2 * 1024 * 1024;
        _heap_end = .;
    } :data

    __end = .;
}
