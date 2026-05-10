%macro LINEAR_TO_SEG_OFFSET 4
    mov %3, %1          ; загрузить 32-битный адрес
    shr %3, 4           ; вычислить сегмент (linear >> 4)
    push %3
    pop %4              ; безопасно взять младшие 16 бит (сегмент)
    mov %2, %4          ; записать в сегментный регистр (разрешено!)

    mov %3, %1          ; перезагрузить исходный адрес
    and %3, 0xF         ; вычислить смещение (linear & 0xF)
    push %3
    pop %4    
%endmacro

%macro ENTER_REAL_MODE 0
    cli
    mov eax, cr0
    and al, 0xFE
    mov cr0, eax
    jmp 0x00:%%rmode_start
%%rmode_start:
    [bits 16]
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax 
    mov sp, 0x7C00
    sti
%endmacro

%macro ENTER_PROTECTED_MODE 0
    cli
    mov eax, cr0 
    or al, 0x01
    mov cr0, eax
    jmp 0x08:%%pmode_start
%%pmode_start:
    [bits 32]
    mov ax, 10
    mov ds, ax 
    mov es, ax
    mov fs, ax 
    mov gs, ax  
    mov ss, ax
%endmacro
