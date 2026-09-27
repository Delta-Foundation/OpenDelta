%include "./boot/x86_64/global-functions.asm"
%include "./boot/x86_64/pmode/functions.asm"

[bits 16]
[org 0x7e00]

global _start

jmp _start 

_start:
    cli
    cld 

    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov sp, 0x7e00
    
    mov [boot_drive], dl 

    mov ax, 0x0003
    int 0x10
    
    mov si, str_16_loading
    call print_rmode

    call bios_e820_memory_map
    call enable_A20 
    call read_kernel_from_disk
    call read_user_prog_from_disk
    call cpu_supports_64_bit_mode 
    call switch_protected_mode

    jmp stage2_endless_loop

[bits 32]
protected_mode:
    cli 

    mov ax, 10 
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov sp, 0x7E00

    call retrive_video_cursor_settings
    
    mov eax, str_32_bit_start
    call print_pmode

    call setup_page_tables
    call switch_long_mode
    jmp stage2_endless_loop

[bits 64]
long_mode:
    mov ax, 0x10 
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax 
    mov rsp, kernel_new_start_virt

    mov rax, str_64_bit_start 
    call print_lmode

    ; jmp kernel_new_start_virt + kernel_new_elf_text_header_offset
    jmp 0x10000

times (loader_file_num_of_blocks * 512) - ($ - $$) db 0
