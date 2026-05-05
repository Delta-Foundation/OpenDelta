%macro enter_real_mode 0
    [bits 32]
    jmp word 18h:.pmode16

.pmode16:
    [bits 16]
    mov eax, cr0
    and al, ~1
    mov cr0, eax

    jmp word 00h:.rmode

.rmode:
    mov ax, 0
    mov ds, ax
    mov ss, ax

    sti

%endmacro


%macro enter_protected_mode 0
    cli

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp dword 08h:.pmode

.pmode:
    [bits 32]
    
    mov ax, 0x10
    mov ds, ax
    mov ss, ax

%endmacro

%macro linear_to_seg_offset 4

    mov %3, %1      
    shr %3, 4
    mov %2, %4
    mov %3, %1 
    and %3, 0xf

%endmacro

global disk_reset
global disk_read

disk_reset:
    [bits 32]

    push ebp
    mov ebp, esp

    enter_real_mode

    mov ah, 0
    mov dl, [bp + 8]
    stc 
    int 0x13

    mov eax, 1
    sbb eax, 0

    push eax 

    enter_protected_mode

    pop eax

    mov esp, ebp
    pop ebp
    ret

disk_read:
    
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    enter_protected_mode

    push ebx
    push es

    ; setup args
    mov dl, [bp + 8]    ; dl - drive

    mov ch, [bp + 12]    ; ch - cylinder (lower 8 bits)
    mov cl, [bp + 13]    ; cl - cylinder to bits 6-7
    shl cl, 6
    
    mov al, [bp + 16]    ; cl - sector to bits 0-5
    and al, 3Fh
    or cl, al

    mov dh, [bp + 20]   ; dh - head

    mov al, [bp + 24]   ; al - count

    linear_to_seg_offset [bp + 28], es, ebx, bx

    mov ah, 02h
    stc
    int 13h

    mov eax, 1
    sbb eax, 0           ; 1 on success, 0 on fail   

    pop es
    pop ebx

    push eax

    enter_protected_mode

    pop eax

    mov esp, ebp
    pop ebp
    ret
