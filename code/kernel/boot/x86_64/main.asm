[bits 16]
[org 0x7c00]

global start

jmp start 

%include "boot/x86_64/rmode/functions.asm"

start:
    cli
    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    mov bx, loader_stage1_stack_top

    mov ss, ax 
    mov sp, ax 
    sti 

    mov [boot_drive], dl 

    mov si, str_stage1
    call print_rmode 

    call bios_check_extensions_present

    xor eax, eax 
    xor ebx, ebx 
    xor ecx, ecx 

    mov eax, loader_stage2_stack_top
    mov bx, [loader_file_num_of_blocks]
    mov ecx, 1
    call bios_extended_read_sectors_from_drive

    mov dl, [boot_drive]

    jmp 0:loader_stage2_stack_top

    jmp endless_loop

times 510 - ($ - $$) db 0 
signature dw 0xAA55
