[bits 16]
[org 0x7c00]

%define BOOT_STAGE1

global start

jmp start 

%include "boot/x86_64/rmode/functions.asm"

start:
    cli
    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    mov ss, ax
    mov sp, loader_stage1_stack_top

    cld
    sti

    mov [boot_drive], dl

    mov si, str_stage1
    call print_rmode 

    call bios_check_extensions_present

    mov si, str_16_loading
    call print_rmode

    mov eax, 1
    mov bx, loader_file_num_of_blocks
    xor cx, cx
    mov di, loader_stage2_entry
    call bios_extended_read_sectors_from_drive

    mov si, str_16_loaded
    call print_rmode

    jmp 0x0000:loader_stage2_entry

times 510 - ($ - $$) db 0 
signature dw 0xAA55
