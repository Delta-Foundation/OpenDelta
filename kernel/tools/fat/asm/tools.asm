%macro ENTER_REAL_MODE 0
    [bits 32]
    jmp dw 0x18:.pmode16

.pmode16:
    [bits 16]
    mov eax, cr0
    and al, ~1
    mov cr0, eax

    jmp dw 0x00:.rmode

.rmode:
    mov ax, 0
    mov ds, ax
    mov ss, ax

    sti

%endmacro


%macro ENTER_PROTECTED_MODE 0
    cli

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp dd 0x8:.pmode32

.pmode32:
    [bits 32]
    
    mov ax, 0x10
    mov ds, ax
    mov ss, ax

%endmacro

%macro LINEAR_TO_SEG_OFFSET 4

    mov %3, %1      
    shr %3, 4
    mov %2, %4
    mov %3, %1 
    and %3, 0xf

%endmacro

global disk_get_drive_params
global disk_reset
global disk_read
global get_next_block

e820_sign equ 0x534D4150

disk_get_drive_params:
    [bits 32]

    push ebp             ; save old call frame
    mov ebp, esp         ; initialize new call frame

    ENTER_REAL_MODE

    [bits 16]

    push es
    push bx
    push esi
    push di

    mov dl, [bp + 8]    ; dl - disk drive
    mov ah, 08h
    mov di, 0           ; es:di - 0000:0000
    mov es, di
    stc
    int 0x13

    mov eax, 1
    sbb eax, 0

    LINEAR_TO_SEG_OFFSET [bp + 12], es, esi, si
    mov [es:si], bl

    mov bl, ch          ; cylinders - lower bits in ch
    mov bh, cl          ; cylinders - upper bits in cl (6-7)
    shr bh, 6
    inc bx

    LINEAR_TO_SEG_OFFSET [bp + 16], es, esi, si
    mov [es:si], bx

    xor ch, ch          ; sectors - lower 5 bits in cl
    and cl, 0x3F
    
    LINEAR_TO_SEG_OFFSET [bp + 20], es, esi, si
    mov [es:si], cx

    mov cl, dh          ; heads - dh
    inc cx

    LINEAR_TO_SEG_OFFSET [bp + 24], es, esi, si
    mov [es:si], cx

    pop di
    pop esi
    pop bx
    pop es

    push eax

    ENTER_PROTECTED_MODE

    [bits 32]

    pop eax

    mov esp, ebp
    pop ebp
    ret

disk_reset:
    [bits 32]

    push ebp
    mov ebp, esp

    ENTER_REAL_MODE

    mov ah, 0
    mov dl, [bp + 8]
    stc 
    int 0x13

    mov eax, 1
    sbb eax, 0

    push eax 

    ENTER_PROTECTED_MODE

    pop eax

    mov esp, ebp
    pop ebp
    ret

disk_read:
    
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    ENTER_PROTECTED_MODE

    push ebx
    push es

    mov dl, [bp + 8]    ; dl - drive

    mov ch, [bp + 12]    ; ch - cylinder (lower 8 bits)
    mov cl, [bp + 13]    ; cl - cylinder to bits 6-7
    shl cl, 6
    
    mov al, [bp + 16]    ; cl - sector to bits 0-5
    and al, 0x3F
    or cl, al

    mov dh, [bp + 20]   ; dh - head

    mov al, [bp + 24]   ; al - count

    LINEAR_TO_SEG_OFFSET [bp + 28], es, ebx, bx

    mov ah, 0x02
    stc
    int 0x13

    mov eax, 1
    sbb eax, 0           ; 1 on success, 0 on fail   

    pop es
    pop ebx

    push eax

    ENTER_PROTECTED_MODE

    pop eax

    mov esp, ebp
    pop ebp
    ret

get_next_block:

    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    ENTER_REAL_MODE

    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ds
    push es

    LINEAR_TO_SEG_OFFSET [bp + 8], es, edi, di     ; es:di pointer to structure
    
    LINEAR_TO_SEG_OFFSET [bp + 12], ds, esi, si    ; ebx - pointer to continuationId
    mov ebx, [ds:si]

    mov eax, 0xE820                             ; eax - function
    mov edx, e820_sign                          ; edx - signature
    mov ecx, 24                                 ; ecx - size of structure

    int 0x15

    cmp eax, e820_sign
    jne .Error

    .IfSuccedeed:
        mov eax, ecx            ; return size
        mov [ds:si], ebx        ; fill continuation parameter
        jmp .EndIf

    .Error:
        mov eax, -1

    .EndIf:

    pop es
    pop ds
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx

    push eax

    ENTER_PROTECTED_MODE

    pop eax

    mov esp, ebp
    pop ebp
    ret
