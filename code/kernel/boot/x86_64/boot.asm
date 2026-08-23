; === SECTOR 1: Boot sector (512 bytes) === ;
[bits 16]
[org 0x7C00]

global _start
global second_stage_entry

_start:
    cli
    cld 
    jmp 0x0000:flush_segments

flush_segments:
    mov sp, _start 
    xor ax, ax 
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov sp, 0x7C00
    
    mov byte [boot_drive], dl 

    mov ax, 0x0003
    int 0x10

    mov si, str_16_loading
    call print_rmode 
    
    mov ax, 0x0241
    mov bx, 0x7E00 
    mov cx, 0x0002
    xor dh, dh 
    
    int 0x13 
    jc disk_error 

    jmp second_stage_entry

disk_error:
    mov si, str_disk_error
    call print_rmode 
    jmp $

print_rmode:
    pusha
.loop:
    lodsb
    test al, al 
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp .loop
.done:
    popa 
    ret 

; === data of 16-bit real mode. Boot sector === ;
boot_drive:       db 0
str_16_loading:   db "[DBL 16]: Loading second stage of boot...", 13, 10, 0
str_disk_error:   db "[DBL ERROR]: Disk read error!!!", 13, 10, 0

; --- boot signature --- ;
times 510 - ($ - $$) db 0
dw 0xAA55
