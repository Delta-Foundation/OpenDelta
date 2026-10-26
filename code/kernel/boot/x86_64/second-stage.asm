[bits 16]
[org 0x1000]

global _start

_start:
    jmp stage2_main

%include "./boot/x86_64/global-functions.asm"
%include "./boot/x86_64/global-data.asm"
%include "./boot/x86_64/pmode/functions.asm"
%include "./boot/x86_64/rmode/functions.asm"
%include "./boot/x86_64/rmode/data.asm"

[bits 16]
stage2_main:
    cli
    cld 

    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov sp, loader_stage2_stack_top
    
    mov [boot_drive], dl 

    mov ax, 0x0003
    int 0x10
    
    mov si, str_16_loading
    call print_rmode

    call bios_e820_memory_map
    call enable_A20 
    call read_kernel_from_disk
    call cpu_supports_64_bit_mode 
    call switch_protected_mode

    jmp global_endless_loop

[bits 32]
protected_mode:
    cli 

    mov ax, 0x10 
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov esp, loader_stage2_stack_top

    call retrive_video_cursor_settings
    
    mov eax, str_32_bit_start
    call print_pmode

    call setup_page_tables
    call switch_long_mode
    jmp global_endless_loop

[bits 64]
long_mode:
    mov ax, 0x10 
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov rsp, 0x00090000

    mov rax, str_64_bit_start 
    mov rdi, 0xB8000
    call print_lmode

    jmp 0x00010000

times (loader_file_num_of_blocks * 512) - ($ - $$) db 0
