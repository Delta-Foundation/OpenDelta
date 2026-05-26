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
    mov dl, [bp + 8]
    mov ah, 0x08
    xor di, di 
    mov es, di 
    stc 
    int 0x13
    mov eax, 1
    sbb eax, 0

    LINEAR_TO_SEG_OFFSET [bp + 12], es, esi, si 
    mov [es:si], bl 

    mov bl, ch
    mov bh, cl 
    shr bh, 6 
    inc bx 
    LINEAR_TO_SEG_OFFSET [bp + 16], es, esi, si 
    mov [es:si], bx 

    xor ch, ch 
    and cl, 0x3F 
    LINEAR_TO_SEG_OFFSET [bp + 20], es, esi, si 
    mov [es:si], cx

    mov cl, dh 
    inc cx 
    LINEAR_TO_SEG_OFFSET [bp + 24], es, esi, si 
    mov [es:si], cx 

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
    mov dl, [bp + 8]
    mov ch, [bp + 12]
    mov cl, [bp + 13]
    shl cl, 6 
    mov al, [bp + 16]
    and al, 0x3F 
    or cl, al 
    mov dh, [bp + 20]
    mov al, [bp + 24]

    LINEAR_TO_SEG_OFFSET [bp + 28], es, ebx, bx 
    mov ah, 0x02
    stc 
    int 0x13

    mov eax, 1 
    sbb eax, 0
    push eax 
    jmp near pmode_disk_read

rmode_get_next_block:
    LINEAR_TO_SEG_OFFSET [bp + 8], es, edi, di
    LINEAR_TO_SEG_OFFSET [bp + 12], ds, esi, si
    mov ebx, [ds:si]

    mov eax, 0xE820
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
