%include "tools/fat/asm/macros.asm"

e820_sign equ 0x534D4150

section .bss
    alignb 16
    pmode_stack_backup resd 1

section .text 
    [bits 32]

    global disk_get_drive_params
    global disk_reset   
    global disk_read
    global get_next_block

disk_get_drive_params:
    push ebp             ; save old call frame
    mov ebp, esp         ; initialize new call frame
    push eax 
    push ebx 
    push ecx 
    push edx
    push esi 
    push edi
    push ds 
    push es 

    mov [pmode_stack_backup], esp 
    jmp enter_real_mode

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

    mov [pmode_stack_backup], esp 
    jmp enter_real_mode

pmode_disk_reset:
    ENTER_PROTECTED_MODE
    mov esp, [pmode_stack_backup]

    pop es 
    pop ds 
    pop edx 
    pop eax 
    mov esp, ebp
    pop ebx 
    ret 

disk_read:
    push ebp 
    mov ebp, esp 
    push eax 
    push ebx 
    push ecx 
    push edx 
    push esi 
    push edi 
    push ds 
    push es 

    mov [pmode_stack_backup], esp 
    jmp enter_real_mode

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
    push eax 
    push ebx 
    push ecx 
    push edx 
    push esi 
    push edi 
    push ds  
    push es 

    mov [pmode_stack_backup], esp
    jmp enter_real_mode

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

section .text.real 
    [bits 16]

enter_real_mode: 
    cli
    mov eax, cr0
    and al, 0xFE
    mov cr0, eax
    jmp .rmode_start
.rmode_start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax 
    mov sp, 0x7c00
    sti

    jmp near rmode_disk_get_drive_params
    jmp near rmode_disk_read
    jmp near rmode_disk_reset
    jmp near rmode_get_next_block

rmode_disk_get_drive_params:
    mov dl, [bp + 8]      ; drive number (1 байт)
    mov ah, 0x08         ; функция получения параметров
    xor di, di
    mov es, di
    stc
    int 0x13

    mov eax, 1
    sbb eax, 0

    ; Адрес для сохранения результата: [bp + 12] = сегмент, [bp + 14] = смещение
    mov ax, [bp + 12]     ; сегмент (16 бит)
    mov es, ax
    mov di, [bp + 14]     ; смещение (16 бит)
    mov [es:di], bl       ; сохранить количество секторов на дорожке

    mov bl, ch
    mov bh, cl
    shr bh, 6
    inc bx
    mov ax, [bp + 16]     ; сегмент
    mov es, ax
    mov di, [bp + 18]     ; смещение
    mov [es:di], bx       ; сохранить количество дорожек

    xor ch, ch
    and cl, 0x3F
    mov ax, [bp + 20]     ; сегмент
    mov es, ax
    mov di, [bp + 22]     ; смещение
    mov [es:di], cx       ; сохранить количество секторов

    mov cl, dh
    inc cx
    mov ax, [bp + 24]     ; сегмент
    mov es, ax
    mov di, [bp + 26]     ; смещение
    mov [es:di], cx       ; сохранить количество головок

    push eax
    jmp near pmode_disk_get_drive_params

rmode_disk_reset:
    mov dl, [bp + 8]
    mov ah, 0x00
    stc 
    int 0x13
    mov eax, 1 
    sbb eax, 0
    push eax 
    jmp near pmode_disk_reset

rmode_disk_read:
    mov dl, [bp + 8]      ; drive number
    mov ch, [bp + 12]     ; cylinder (16 бит)
    mov cl, [bp + 13]     ; cylinder high bits
    shl cl, 6
    mov al, [bp + 16]     ; sector count (16 бит)
    and al, 0x3F
    or cl, al
    mov dh, [bp + 20]     ; head
    mov al, [bp + 24]     ; sector count

    mov ax, [bp + 28]     ; сегмент
    mov es, ax
    mov bx, [bp + 30]     ; смещение

    mov ah, 0x02          ; функция чтения
    int 0x13

    mov eax, 1
    sbb eax, 0
    push eax
    jmp near pmode_disk_read

rmode_get_next_block:
    mov ax, [bp + 8]      ; сегмент для es
    mov es, ax
    mov di, [bp + 10]     ; смещение

    mov ax, [bp + 12]     ; сегмент для ds
    mov ds, ax
    mov si, [bp + 14]     ; смещение

    mov eax, 0xE820       ; функция e820
    mov edx, e820_sign
    mov ecx, 24
    int 0x15

    cmp eax, e820_sign
    jne .e820_fail

.e820_ok:
    mov eax, ecx
    mov [ds:si], ebx
    jmp .e820_end

.e820_fail:
    mov eax, -1

.e820_end:
    push eax
    jmp near pmode_get_next_block
