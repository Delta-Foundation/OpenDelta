; === SECTOR 1: Boot sector (512 bytes) === ;
[bits 16]
[org 0x7C00]

SECOND_STAGE_ADDR    equ 0x7E00
SECOND_STAGE_SECTOR  equ 2
SECOND_STAGE_SECTORS equ 16

global _start
global second_stage_entry

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

    mov si, str_stage1
    call print_rmode

    call load_stage2

[bits 16]
load_stage2:
    pusha

    mov	si, str_16_loading 
    call print_rmode 

    mov ax, 0x7E00
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 17
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    mov dl, [boot_drive]
    int 0x13
    jc read_error

    mov ah, 0x02
    mov al, 15
    mov ch, 0x00
    mov dh, 0x01
    mov cl, 0x01
    mov dl, [boot_drive]
    int 0x13
    jc read_error

    mov si, str_16_loaded
    call print_rmode 

    popa
    ret

read_error:
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
str_stage1:       db "[DBL]: STAGE 1", 13, 10, 0
str_16_loading:   db "[DBL 16]: Loading second stage of boot...", 13, 10, 0
str_16_loaded:    db "[DBL 16]: Stage 2 loaded at 0x7E00", 13, 10, 0
str_disk_error:   db "[DBL ERROR]: Disk read error!!!", 13, 10, 0

; --- boot signature --- ;
times 510 - ($ - $$) db 0
dw 0xAA55
