%define RMODE_ADDR  0x8000
%define RMODE_SEG   0x800       /* RMODE_ADDR / 16, должен совпадать с linker script */

%define GDT_CODE16  RMODE_SEG
%define GDT_DATA16  (RMODE_SEG + 8)
%define GDT_CODE32  (RMODE_SEG + 0x10)
%define GDT_DATA32  (RMODE_SEG + 0x18)
%define GDT_CODE64  (RMODE_SEG + 0x20)
%define GDT_DATA64  (RMODE_SEG + 0x28)

; +--------------------------------------+
; | GDT_ENTRY base, limit, access, flags |
; +--------------------------------------+
%macro GDT_ENTRY 4
    dw (%2) & 0xFFFF
    dw (%1) & 0xFFFF
    db ((%1) >> 16) & 0xFF
    db (%3) & 0xFF
    db (((%2) >> 16) & 0x0F) | (((%4) & 0x0F) << 4)
    db ((%1) >> 24) & 0xFF
%endmacro

; +-----------------------------------------------------------------------------+
; | MAKE_GDT — создаёт GDT в .rmode.                                            |
; | 16-bit code descriptor помещается на селектор RMODE_SEG с базой RMODE_ADDR, |
; | чтобы после отключения protected mode CS продолжал указывать на реальный    |
; | сегмент кода.                                                               |
; +-----------------------------------------------------------------------------+
%macro MAKE_GDT 0
gdt_start:
    GDT_ENTRY 0, 0, 0x00, 0x0
    times (RMODE_SEG / 8) dq 0
    GDT_ENTRY RMODE_ADDR, 0xFFFFF, 0x9A, 0x8   ; 16-bit code
    GDT_ENTRY 0,          0xFFFFF, 0x92, 0x8   ; 16-bit data
    GDT_ENTRY 0,          0xFFFFF, 0x9A, 0xC   ; 32-bit code
    GDT_ENTRY 0,          0xFFFFF, 0x92, 0xC   ; 32-bit data
    GDT_ENTRY 0,          0xFFFFF, 0x9A, 0x2   ; 64-bit code
    GDT_ENTRY 0,          0xFFFFF, 0x92, 0xC   ; 64-bit data
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
%endmacro

; +-------------------------------------------------+
; | ENTER_LONG_MODE                                 | 
; | Переход из 16-bit real mode в 64-bit long mode. |
; +-------------------------------------------------+
%macro ENTER_LONG_MODE 0
[bits 16]
    cli

    mov dword [pml4], (pdpt) | 3
    mov dword [pml4 + 4], 0
    mov dword [pdpt], (pd) | 3
    mov dword [pdpt + 4], 0

    mov di, pd
    mov dword [di],      0x00000083
    mov dword [di + 4],  0
    mov dword [di + 8],  0x00200083
    mov dword [di + 12], 0
    mov dword [di + 16], 0x00400083
    mov dword [di + 20], 0
    mov dword [di + 24], 0x00600083
    mov dword [di + 28], 0

    o32 lgdt [gdt_descriptor]

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp dword GDT_CODE32:%%pmode

; +-----------------------------+
; | 32-х битный защищёный режим |
; +-----------------------------+
[bits 32]
%%pmode:
    mov ax, GDT_DATA32
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, RM_STACK_TOP

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov eax, pml4
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    jmp GDT_CODE64:%%lmode

; +---------------------------------------+
; | 64-х битный длинный режим (long mode) |
; +---------------------------------------+
[bits 64]
%%lmode:
    mov ax, GDT_DATA64
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rax, [rm_return_addr]
    jmp rax
%endmacro

; +-------------------------------------------------------------------+
; | ENTER_REAL_MODE macros                                            |
; | переход из 64-х битного режима обратно в 16-битный реальный режим |
; +-------------------------------------------------------------------+
%macro ENTER_REAL_MODE 0
    [bits 64]
    cli

    jmp GDT_CODE32:%%compat32

[bits 32]
%%compat32:
    mov ax, GDT_DATA32
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov eax, cr0
    and eax, ~(1 << 31)
    mov cr0, eax

    mov eax, cr4
    and eax, ~(1 << 5)
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    and eax, ~(1 << 8)
    wrmsr

    jmp word GDT_CODE16:%%pmode16

[bits 16]
%%pmode16:
    mov ax, GDT_DATA16
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov eax, cr0
    and al, ~1
    mov cr0, eax

    jmp GDT_CODE16:%%rmode

%%rmode:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, RM_STACK_TOP
    sti
    jmp rmode_dispatcher
%endmacro
