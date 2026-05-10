OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

PHDRS {
    text      PT_LOAD FLAGS(5);
    data      PT_LOAD FLAGS(6);
}

SECTIONS {
    . = 0x100000;

    .multiboot ALIGN(4) : {
        KEEP(*(.multiboot))
    } :text  
 
    .fsjump : { 
        *(.fsjump)
        *(.fsjump*) 
    }
    
    .fsheaders : { 
        *(.fsheaders) 
        *(.fsheaders*)
    }

    .text.real ALIGN(16) : {
        __text_real_start = .;
        *(.text.real)
        *(.text.real*)
        __text_real_end = .;
    } :text

    .text ALIGN(4096) : {
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
    } :text 

    .bss ALIGN(32) : {
        __bss_start = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } :data 

    .heap ALIGN(4096) : {
        _heap_start = .;
        . = . + 4 * 1024 * 1024;
        _heap_end = .;
    } :data

    __end = .;
}
