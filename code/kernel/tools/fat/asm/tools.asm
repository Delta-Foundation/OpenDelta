;-------------------------------------------|
;                                           |
; lmode_func - функция 64-х битного режима. |
; rmode_func - функция 16 битного режима.   |
; func - обычная функция.                   |
;                                           |
;-------------------------------------------|
%include "tools/fat/asm/macros.asm"

e820_sign equ 0x534D4150
RM_STACK_TOP equ 0x9000

section .bss
    align 4096
    lmode_stack_backup resd 1
    rm_arg_buf         resb 32

section .text
    [bits 64]
    align 4096

    global disk_get_drive_params
    global disk_reset
    global disk_read
    global get_next_block

; ---------- Вспомогательные функции (64-bit) ----------
disk_get_drive_params:
    push rbp
    mov rbp, rsp
    pushad
    push ds
    push es

    mov rax, [rbp + 8]
    mov [rm_arg_buf + 0], rax
    mov rax, [rbp + 12]
    mov [rm_arg_buf + 4], rax
    mov rax, [rbp + 16]
    mov [rm_arg_buf + 8], rax
    mov rax, [rbp + 20]
    mov [rm_arg_buf + 12], rax
    mov rax, [rbp + 24]
    mov [rm_arg_buf + 16], rax

    mov [lmode_stack_backup], rsp
    jmp near enter_real_mode

lmode_disk_get_drive_params:
    ENTER_LONG_MODE
    mov rsp, [lmode_stack_backup]
    pop es
    pop ds
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret

disk_reset:
    push rbp
    mov rbp, rsp
    push rax
    push rdx
    push ds
    push es

    mov rax, [rbp + 8]
    mov [rm_arg_buf + 0], rax
    mov [lmode_stack_backup], rsp
    jmp near enter_real_mode

lmode_disk_reset:
    ENTER_LONG_MODE
    mov rsp, [lmode_stack_backup]
    pop es
    pop ds
    pop rdx
    pop rax
    mov rsp, rbp
    pop rbp
    ret

disk_read:
    push rbp
    mov rbp, rsp
    pushad
    push ds
    push es

    mov rax, [rbp + 8]
    mov [rm_arg_buf + 0], rax
    mov rax, [rbp + 12]
    mov [rm_arg_buf + 4], rax
    mov rax, [rbp + 16]
    mov [rm_arg_buf + 8], rax
    mov rax, [rbp + 20]
    mov [rm_arg_buf + 12], rax
    mov rax, [rbp + 24]
    mov [rm_arg_buf + 16], rax
    mov rax, [rbp + 28]
    mov [rm_arg_buf + 20], rax

    mov [lmode_stack_backup], rsp
    jmp near enter_real_mode

lmode_disk_read:
    ENTER_LONG_MODE
    mov rsp, [lmode_stack_backup]
    pop es
    pop ds
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret

get_next_block:
    push rbp
    mov rbp, rsp
    pushad
    push ds
    push es

    mov rax, [rbp + 8]
    mov [rm_arg_buf + 0], rax
    mov rax, [rbp + 12]
    mov [rm_arg_buf + 4], rax

    mov [lmode_stack_backup], rsp
    jmp near enter_real_mode

lmode_get_next_block:
    ENTER_LONG_MODE
    mov rsp, [lmode_stack_backup]
    pop es
    pop ds
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    mov rsp, rbp
    pop rbp
    ret

section .rmode
    [bits 16]
    align 32

enter_real_mode:
    cli
    mov rax, cr0
    and al, 0xFE
    mov cr0, rax
    jmp 0x1000:rmode_start

rmode_start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000
    sti

    mov rax, rm_arg_buf
    mov bp, ax

    jmp short rmode_dispatcher

rmode_dispatcher:
    jmp short rmode_disk_get_drive_params

rmode_disk_get_drive_params:
    mov dl, [bp + 0]
    mov ah, 0x08
    stc
    int 0x13
    mov rax, 1
    sbb rax, 0

    LINEAR_TO_SEG_OFFSET [bp + 4], es, rsi, si
    mov [es:si], bl

    mov eax, [bp + 8]
    LINEAR_TO_SEG_OFFSET eax, es, rsi, si
    mov bl, ch
    mov bh, cl
    shr bh, 6
    inc bx
    mov [es:si], bx

    xor ch, ch
    and cl, 0x3F
    mov eax, [bp + 12]
    LINEAR_TO_SEG_OFFSET eax, es, rsi, si
    mov [es:si], cx

    mov cl, dh
    inc cx
    mov eax, [bp + 16]
    LINEAR_TO_SEG_OFFSET eax, es, rsi, si
    mov [es:si], cx

    push eax
    jmp near lmode_disk_get_drive_params

rmode_disk_reset:
    mov dl, [bp + 0]
    mov ah, 0x00
    stc
    int 0x13
    mov eax, 1
    sbb eax, 0
    push eax
    jmp near lmode_disk_reset

rmode_disk_read:
    mov dl, [bp + 0]
    mov ch, [bp + 4]
    mov cl, [bp + 5]
    shl cl, 6
    mov al, [bp + 8]
    and al, 0x3F
    or cl, al
    mov dh, [bp + 12]
    mov al, [bp + 16]

    mov eax, [bp + 20]
    LINEAR_TO_SEG_OFFSET rax, es, rbx, bx

    mov ah, 0x02
    stc
    int 0x13

    mov rax, 1
    sbb rax, 0
    push rax
    jmp near lmode_disk_read

rmode_get_next_block:
    mov rax, [bp + 0]
    LINEAR_TO_SEG_OFFSET rax, es, rdi, di

    mov rax, [bp + 4]
    LINEAR_TO_SEG_OFFSET rax, ds, rsi, si

    mov rbx, [ds:si]

    mov rax, 0xE820
    mov rdx, 0x534D4150
    mov rcx, 24
    int 0x15

    cmp rax, rdx
    jne short .e820_fail

.e820_ok:
    mov rax, rcx
    mov [ds:si], rbx
    jmp short .e820_end

.e820_fail:
    mov rax, -1

.e820_end:
    push rax
    jmp near lmode_get_next_block
