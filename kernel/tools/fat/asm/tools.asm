;-------------------------------------------|
;                                           |
; pmode_func - функция 32-х битного режима. |
; rmode_func - функция 16 битного режима.   |
; func - обычная функция.                   |
;                                           |
;-------------------------------------------|
%include "tools/fat/asm/macros.asm"

e820_sign equ 0x534D4150
RM_STACK_TOP equ 0x9000

section .bss
    align 4096
    pmode_stack_backup resd 1
    rm_arg_buf         resb 32

section .text
    [bits 32]
    align 4096

    global disk_get_drive_params
    global disk_reset
    global disk_read
    global get_next_block

; ---------- Вспомогательные функции (32-bit) ----------
disk_get_drive_params:
    push ebp
    mov ebp, esp
    pushad
    push ds
    push es

    mov eax, [ebp + 8]
    mov [rm_arg_buf + 0], eax
    mov eax, [ebp + 12]
    mov [rm_arg_buf + 4], eax
    mov eax, [ebp + 16]
    mov [rm_arg_buf + 8], eax
    mov eax, [ebp + 20]
    mov [rm_arg_buf + 12], eax
    mov eax, [ebp + 24]
    mov [rm_arg_buf + 16], eax

    mov [pmode_stack_backup], esp
    jmp near enter_real_mode

pmode_disk_get_drive_params:
    ENTER_PROTECTED_MODE
    mov esp, [pmode_stack_backup]
    pop es
    pop ds
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    mov esp, ebp
    pop ebp
    ret

disk_reset:
    push ebp
    mov ebp, esp
    push eax
    push edx
    push ds
    push es

    mov eax, [ebp + 8]
    mov [rm_arg_buf + 0], eax
    mov [pmode_stack_backup], esp
    jmp near enter_real_mode

pmode_disk_reset:
    ENTER_PROTECTED_MODE
    mov esp, [pmode_stack_backup]
    pop es
    pop ds
    pop edx
    pop eax
    mov esp, ebp
    pop ebp
    ret

disk_read:
    push ebp
    mov ebp, esp
    pushad
    push ds
    push es

    mov eax, [ebp + 8]
    mov [rm_arg_buf + 0], eax
    mov eax, [ebp + 12]
    mov [rm_arg_buf + 4], eax
    mov eax, [ebp + 16]
    mov [rm_arg_buf + 8], eax
    mov eax, [ebp + 20]
    mov [rm_arg_buf + 12], eax
    mov eax, [ebp + 24]
    mov [rm_arg_buf + 16], eax
    mov eax, [ebp + 28]
    mov [rm_arg_buf + 20], eax

    mov [pmode_stack_backup], esp
    jmp near enter_real_mode

pmode_disk_read:
    ENTER_PROTECTED_MODE
    mov esp, [pmode_stack_backup]
    pop es
    pop ds
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    mov esp, ebp
    pop ebp
    ret

get_next_block:
    push ebp
    mov ebp, esp
    pushad
    push ds
    push es

    mov eax, [ebp + 8]
    mov [rm_arg_buf + 0], eax
    mov eax, [ebp + 12]
    mov [rm_arg_buf + 4], eax

    mov [pmode_stack_backup], esp
    jmp near enter_real_mode

pmode_get_next_block:
    ENTER_PROTECTED_MODE
    mov esp, [pmode_stack_backup]
    pop es
    pop ds
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    mov esp, ebp
    pop ebp
    ret

section .rmode
    [bits 16]
    align 32

enter_real_mode:
    cli
    mov eax, cr0
    and al, 0xFE
    mov cr0, eax
    jmp 0x1000:rmode_start

rmode_start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000
    sti

    mov eax, rm_arg_buf
    mov bp, ax

    jmp short rmode_dispatcher

rmode_dispatcher:
    jmp short rmode_disk_get_drive_params

rmode_disk_get_drive_params:
    mov dl, [bp + 0]
    mov ah, 0x08
    stc
    int 0x13
    mov eax, 1
    sbb eax, 0

    LINEAR_TO_SEG_OFFSET [bp + 4], es, esi, si
    mov [es:si], bl

    mov eax, [bp + 8]
    LINEAR_TO_SEG_OFFSET eax, es, esi, si
    mov bl, ch
    mov bh, cl
    shr bh, 6
    inc bx
    mov [es:si], bx

    xor ch, ch
    and cl, 0x3F
    mov eax, [bp + 12]
    LINEAR_TO_SEG_OFFSET eax, es, esi, si
    mov [es:si], cx

    mov cl, dh
    inc cx
    mov eax, [bp + 16]
    LINEAR_TO_SEG_OFFSET eax, es, esi, si
    mov [es:si], cx

    push eax
    jmp near pmode_disk_get_drive_params

rmode_disk_reset:
    mov dl, [bp + 0]
    mov ah, 0x00
    stc
    int 0x13
    mov eax, 1
    sbb eax, 0
    push eax
    jmp near pmode_disk_reset

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
    LINEAR_TO_SEG_OFFSET eax, es, ebx, bx

    mov ah, 0x02
    stc
    int 0x13

    mov eax, 1
    sbb eax, 0
    push eax
    jmp near pmode_disk_read

rmode_get_next_block:
    mov eax, [bp + 0]
    LINEAR_TO_SEG_OFFSET eax, es, edi, di

    mov eax, [bp + 4]
    LINEAR_TO_SEG_OFFSET eax, ds, esi, si

    mov ebx, [ds:si]

    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 24
    int 0x15

    cmp eax, edx
    jne short .e820_fail

.e820_ok:
    mov eax, ecx
    mov [ds:si], ebx
    jmp short .e820_end

.e820_fail:
    mov eax, -1

.e820_end:
    push eax
    jmp near pmode_get_next_block
