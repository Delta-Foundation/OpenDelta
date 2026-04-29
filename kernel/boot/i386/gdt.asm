gdt_start:
    gdt_null:
        dq 0x0

    gdt_code:
        dw 0xFFFF       ; Лимит 0-15
        dw 0x0          ; База 0-15
        db 0x0          ; База 16-23
        db 0x9A         ; Флаги: Present, Ring 0, Code, Exec/Read
        db 0xCF         ; Флаги: Granularity, 32-bit, Лимит 16-19
        db 0x0          ; База 24-31

    gdt_data:
        dw 0xFFFF
        dw 0x0
        db 0x0
        db 0x92         ; Present, Ring 0, Data, Read/Write
        db 0xCF
        db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Лимит GDT
    dd gdt_start                ; Базовый адрес GDT

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
