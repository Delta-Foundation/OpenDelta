OUTPUT_FORMAT(elf_x86_64)
OUTPUT_ARCH(x86_64)
ENTRY(_start)

start_addr = 0x10000;

PHDRS {
    text    PT_LOAD FLAGS(5);
    data    PT_LOAD FLAGS(6);
}

SECTIONS  {
    . = start_addr;

/*
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
*/

    .text ALIGN(4096) : {
        __text_start = .;
        *(.text)
        *(.text*)
        *(.text.start)
    } :text 

    .rmode ALIGN(4096) : {
        *(.rmode)
        *(.rmode*)
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

    __end = .;
}
