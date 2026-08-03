%include "tools/fat/asm/macros.asm"

RM_STACK_TOP equ 0x7000

OP_DISK_GET_DRIVE_PARAMS equ 0
OP_DISK_RESET            equ 1
OP_DISK_READ             equ 2
OP_GET_NEXT_BLOCK        equ 3

section .bss
    [bits 64]
    align 4096
    lmode_stack_backup resq 1

; ---------------------------------------------------------------------------
; section .rmode — 16-bit real mode код, GDT, page tables, буферы аргументов
; ---------------------------------------------------------------------------
section .rmode
    [bits 16]
    align 4096

    pml4:    times 512 dq 0
    pdpt:    times 512 dq 0
    pd:      times 512 dq 0

    align 16
    MAKE_GDT

    align 8
    rm_return_addr: dq 0

rm_arg_buf:
    .op:   resb 1
    .args: resb 31

align 4
enter_real_mode:
    ENTER_REAL_MODE

align 4
rmode_dispatcher:
    mov al, [rm_arg_buf + 0]
    cmp al, OP_DISK_GET_DRIVE_PARAMS
    je rmode_disk_get_drive_params
    cmp al, OP_DISK_RESET
    je rmode_disk_reset
    cmp al, OP_DISK_READ
    je rmode_disk_read
    cmp al, OP_GET_NEXT_BLOCK
    je rmode_get_next_block
    jmp error_loop

align 4
rmode_disk_get_drive_params:
    mov dl, [rm_arg_buf + 1]
    mov ah, 0x08
    stc
    int 0x13

    mov ax, 1
    sbb ax, 0
    mov [rm_arg_buf + 18], al
    mov es, [rm_arg_buf + 2]
    mov bx, [rm_arg_buf + 4]
    mov [es:bx], bl

    mov es, [rm_arg_buf + 6]
    mov bx, [rm_arg_buf + 8]
    mov bl, ch
    mov bh, cl
    shr bh, 6
    inc bx
    mov [es:bx], bx

    mov es, [rm_arg_buf + 10]
    mov bx, [rm_arg_buf + 12]
    xor ch, ch
    and cl, 0x3F
    mov [es:bx], cx

    mov es, [rm_arg_buf + 14]
    mov bx, [rm_arg_buf + 16]
    mov cl, dh
    inc cx
    mov [es:bx], cx

    jmp enter_long_mode

align 4
rmode_disk_reset:
    mov dl, [rm_arg_buf + 1]
    mov ah, 0x00
    stc
    int 0x13
    mov ax, 1
    sbb ax, 0
    mov [rm_arg_buf + 2], al
    jmp enter_long_mode

align 4
rmode_disk_read:
    mov dl, [rm_arg_buf + 1]
    mov ch, [rm_arg_buf + 2]      ; cylinder bits 0-7
    mov cl, [rm_arg_buf + 3]      ; cylinder bits 8-9
    shl cl, 6
    mov al, [rm_arg_buf + 4]      ; sector (6 bit)
    and al, 0x3F
    or cl, al
    mov dh, [rm_arg_buf + 5]      ; head
    mov al, [rm_arg_buf + 6]      ; count

    mov es, [rm_arg_buf + 7]
    mov bx, [rm_arg_buf + 9]

    mov ah, 0x02
    stc
    int 0x13

    mov ax, 1
    sbb ax, 0
    mov [rm_arg_buf + 11], al
    jmp enter_long_mode

align 4
rmode_get_next_block:
    mov es, [rm_arg_buf + 1]
    mov di, [rm_arg_buf + 3]

    mov ds, [rm_arg_buf + 5]
    mov si, [rm_arg_buf + 7]

    mov ebx, [ds:si]

    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 24
    int 0x15

    cmp eax, edx
    jne .e820_fail

.e820_ok:
    mov [ds:si], ebx
    mov ax, cx
    jmp short .e820_end

.e820_fail:
    mov ax, 0xFFFF

.e820_end:
    mov [rm_arg_buf + 9], ax
    jmp enter_long_mode

align 4
enter_long_mode:
    ENTER_LONG_MODE

align 4
error_loop:
    hlt
    jmp error_loop

; ---------------------------------------------------------------------------
; section .text — 64-bit long mode код
; ---------------------------------------------------------------------------
section .text
    [bits 64]
    DEFAULT REL
    align 4096

    global disk_get_drive_params
    global disk_reset
    global disk_read
    global get_next_block

linear_to_seg_offset:
    push rbx
    mov rbx, rax
    and rbx, 0xF
    mov rdx, rbx
    shr rax, 4
    and rax, 0xFFFF
    pop rbx
    ret

disk_get_drive_params:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov r12, rdi
    mov r13, rsi
    mov r14, rdx
    mov r15, rcx
    mov rbx, r8

    mov [lmode_stack_backup], rsp

    mov byte [rm_arg_buf + 0], OP_DISK_GET_DRIVE_PARAMS
    mov byte [rm_arg_buf + 1], r12b

    mov rax, r13
    call linear_to_seg_offset
    mov [rm_arg_buf + 2], ax
    mov [rm_arg_buf + 4], dx

    mov rax, r14
    call linear_to_seg_offset
    mov [rm_arg_buf + 6], ax
    mov [rm_arg_buf + 8], dx

    mov rax, r15
    call linear_to_seg_offset
    mov [rm_arg_buf + 10], ax
    mov [rm_arg_buf + 12], dx

    mov rax, rbx
    call linear_to_seg_offset
    mov [rm_arg_buf + 14], ax
    mov [rm_arg_buf + 16], dx

    mov rax, .return
    mov [rm_return_addr], rax

    jmp enter_real_mode

.return:
    mov rsp, [lmode_stack_backup]
    xor rax, rax
    mov al, [rm_arg_buf + 18]
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

disk_reset:
    push rbp
    mov rbp, rsp
    push rbx
    push r12

    mov r12, rdi
    mov [lmode_stack_backup], rsp

    mov byte [rm_arg_buf + 0], OP_DISK_RESET
    mov byte [rm_arg_buf + 1], r12b

    mov rax, .return
    mov [rm_return_addr], rax

    jmp enter_real_mode

.return:
    mov rsp, [lmode_stack_backup]
    xor rax, rax
    mov al, [rm_arg_buf + 2]
    pop r12
    pop rbx
    pop rbp
    ret

disk_read:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov r12, rdi
    mov r13, rsi
    mov r14, rdx
    mov r15, rcx
    mov rbx, r8

    mov [lmode_stack_backup], rsp

    mov byte [rm_arg_buf + 0], OP_DISK_READ
    mov byte [rm_arg_buf + 1], r12b

    mov ax, r13w
    mov [rm_arg_buf + 2], al
    shr ax, 8
    and al, 0x03
    mov [rm_arg_buf + 3], al

    mov byte [rm_arg_buf + 4], r14b
    mov byte [rm_arg_buf + 5], r15b
    mov byte [rm_arg_buf + 6], bl

    mov rax, r9
    call linear_to_seg_offset
    mov [rm_arg_buf + 7], ax
    mov [rm_arg_buf + 9], dx

    mov rax, .return
    mov [rm_return_addr], rax

    jmp enter_real_mode

.return:
    mov rsp, [lmode_stack_backup]
    xor rax, rax
    mov al, [rm_arg_buf + 11]
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

get_next_block:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13

    mov r12, rdi
    mov r13, rsi
    mov [lmode_stack_backup], rsp

    mov byte [rm_arg_buf + 0], OP_GET_NEXT_BLOCK

    mov rax, r12
    call linear_to_seg_offset
    mov [rm_arg_buf + 1], ax
    mov [rm_arg_buf + 3], dx

    mov rax, r13
    call linear_to_seg_offset
    mov [rm_arg_buf + 5], ax
    mov [rm_arg_buf + 7], dx

    mov rax, .return
    mov [rm_return_addr], rax

    jmp enter_real_mode

.return:
    mov rsp, [lmode_stack_backup]
    xor rax, rax
    mov ax, [rm_arg_buf + 9]
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
