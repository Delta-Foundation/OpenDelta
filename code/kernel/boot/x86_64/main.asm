%include "./boot/x86_64/functions.asm"

[bits 16]
[org 0x7C00]

global _start

jmp _start 

_start:
    cli
    cld 

    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov sp, 0x7C00
    
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
    call enter_protected_mode

    jmp endless_loop

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
