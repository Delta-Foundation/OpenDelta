[bits 16]
[org 0x7C00]

; includes like .h in C
%include "boot/i386/gdt.asm"
%include "boot/i386/midt.asm"
%include "boot/i386/disk.asm"
%include "boot/i386/print.asm"
%include "boot/i386/print_pmode.asm"
%include "boot/i386/switch.asm"
%include "boot/i386/print_hex.asm"
%include "boot/i386/mboot_tables.asm"
%include "boot/i386/fat_header.asm"
%include "boot/i386/const.asm"

; new line (\n)
%define ENDL 0x0D, 0x0A

; data, strings, messages...
section .data
    str_real db "Started in 16-bit real mode", ENDL, 0
    str_pmode db "Landed in 32-bit prorected mode", ENDL, 0
    str_load db "Loading dltkernel from the disk", ENDL, 0
    
    boot_drive db 0
    boot_part_seg dw 0
    boot_part_off dw 0
    
    ; errors
    str_returned_kernel db "Returned from kernel. Error?", 0
    str_read_fail db "[err]: [read failed!]", 0

; main and important variably for starting program
section .text
    kernel_offset equ 0x100000 
    global _start

_start:
    cli 

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax 
    mov sp, 0x7000

    mov [boot_drive], dl
    mov  bx, str_real 
    call print
    call print_nl
    
    call load_kernel
    call setup_idt
    call disable_timer
    ; call load_kernel
    jmp switch

    jmp kernel_offset

[bits 16]
load_kernel:
    mov	bx, str_load
    call print
    call print_nl

    mov	bx, 0x1000
    mov es, bx 
    xor bx, bx

    mov	dl, [boot_drive]
    mov dh, 0
    mov ch, 0
    mov cl, 2
    mov al, 32
    mov ah, 0x02
    
    int 0x13
    jc read_error
    ret

read_error:
    mov bx, str_read_fail
    call print
    jmp $

[bits 32]
protected_mode:
    mov ax, DATA_SEG
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
    mov esp, 0x90000

    mov ebx, str_pmode
    call print_pmode

    mov eax, [boot_part_seg]
    shl eax, 16
    or eax, [boot_part_off]
    push eax
    movzx eax, byte [boot_drive] 

    jmp kernel_offset

times 510 - ($-$$) db 0
dw 0xAA55
