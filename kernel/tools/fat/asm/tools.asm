%include "tools/fat/asm/macros.asm"

e820_sign equ 0x534D4150

section .bss
    alignb 16
    pmode_stack_backup resd 1
    real_mode_entry_point resd 1

section .text
    [bits 32]

    global disk_get_drive_params
    global disk_reset
    global disk_read
    global get_next_block

call_real_mode:
    mov [pmode_stack_backup], esp

    mov eax, [esp + 4]  ; Return address (real-mode function)
    mov [real_mode_entry_point], eax

    jmp enter_real_mode

disk_get_drive_params:
    push ebp
    mov ebp, esp
    pushad
    push ds
    push es

    push rmode_disk_get_drive_params
    call call_real_mode
    add esp, 4

    call pmode_disk_get_drive_params

    pop es
    pop ds
    popad
    mov esp, ebp
    pop ebp
    ret

disk_reset:
    push ebp
    mov ebp, esp
    pushad
    push ds
    push es

    push rmode_disk_reset
    call call_real_mode
    add esp, 4

    call pmode_disk_reset

    pop es
    pop ds
    popad
    mov esp, ebp
    pop ebp
    ret

disk_read:
    push ebp
    mov ebp, esp
    pushad
    push ds
    push es

    push rmode_disk_read
    call call_real_mode
    add esp, 4

    call pmode_disk_read

    pop es
    pop ds
    popad
    mov esp, ebp
    pop ebp
    ret

get_next_block:
    push ebp
    mov ebp, esp
    pushad
    push ds
    push es

    push rmode_get_next_block
    call call_real_mode
    add esp, 4

    call pmode_get_next_block

    pop es
    pop ds
    popad
    mov esp, ebp
    pop ebp
    ret

pmode_disk_get_drive_params:
    ENTER_PROTECTED_MODE
    mov esp, [pmode_stack_backup]
    ret

pmode_disk_reset:
    ENTER_PROTECTED_MODE
    mov esp, [pmode_stack_backup]
    ret

pmode_disk_read:
    ENTER_PROTECTED_MODE
    mov esp, [pmode_stack_backup]
    ret

pmode_get_next_block:
    ENTER_PROTECTED_MODE
    mov esp, [pmode_stack_backup]
    ret

section .text.real
    [bits 16]

enter_real_mode:
    cli
    mov eax, cr0
    and al, 0xFE
    mov cr0, eax

    jmp 0x00:.rmode_start
.rmode_start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    jmp word [real_mode_entry_point]

rmode_disk_get_drive_params:
    push bp
    mov bp, sp

    mov dl, [bp + 4]      ; drive number
    mov ah, 0x08         ; get drive params
    xor di, di
    mov es, di
    stc
    int 0x13

    mov eax, 1
    sbb eax, 0

    mov ax, [bp + 6]
    mov es, ax
    mov di, [bp + 8]
    mov [es:di], bl

    mov bl, ch
    mov bh, cl
    shr bh, 6
    inc bx
    mov ax, [bp + 10]
    mov es, ax
    mov di, [bp + 12]
    mov [es:di], bx

    xor ch, ch
    and cl, 0x3F
    mov ax, [bp + 14]
    mov es, ax
    mov di, [bp + 16]
    mov [es:di], cx

    mov cl, dh
    inc cx
    mov ax, [bp + 18]
    mov es, ax
    mov di, [bp + 20]
    mov [es:di], cx

    mov sp, bp
    pop bp
    retf

rmode_disk_reset:
    push bp
    mov bp, sp

    mov dl, [bp + 4]
    mov ah, 0x00
    stc
    int 0x13

    mov eax, 1
    sbb eax, 0

    mov sp, bp
    pop bp
    retf

rmode_disk_read:
    push bp
    mov bp, sp

    mov dl, [bp + 4]      ; drive
    mov ch, [bp + 6]      ; cylinder low
    mov cl, [bp + 8]      ; cylinder high
    shl cl, 6
    mov al, [bp + 10]     ; sector count
    and al, 0x3F
    or cl, al
    mov dh, [bp + 12]     ; head
    mov al, [bp + 14]     ; sector count

    mov ax, [bp + 16]     ; segment
    mov es, ax
    mov bx, [bp + 18]     ; offset

    mov ah, 0x02
    int 0x13

    mov eax, 1
    sbb eax, 0

    mov sp, bp
    pop bp
    retf

rmode_get_next_block:
    push bp
    mov bp, sp

    mov ax, [bp + 4]      ; es segment
    mov es, ax
    mov di, [bp + 6]      ; es offset

    mov ax, [bp + 8]      ; ds segment
    mov ds, ax
    mov si, [bp + 10]     ; ds offset

    mov eax, 0xE820
    mov edx, e820_sign
    mov ecx, 24
    int 0x15

    cmp eax, e820_sign
    jne .fail

.ok:
    mov eax, ecx
    mov [ds:si], ebx
    jmp .end

.fail:
    mov eax, -1

.end:
    mov sp, bp
    pop bp
    retf
