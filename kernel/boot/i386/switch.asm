[bits 16]
switch:
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword CODE_SEG:init

[bits 32]
init:
    jmp protected_mode
