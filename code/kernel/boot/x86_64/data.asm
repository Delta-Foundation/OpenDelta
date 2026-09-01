; === data of 16-bit real mode. Boot sector === ;
boot_drive:       db 0
str_stage1:       db "[DBL]: STAGE 1", 13, 10, 0
str_16_loading:   db "[DBL 16]: Loading second stage of boot...", 13, 10, 0
str_16_loaded:    db "[DBL 16]: Stage 2 loaded at 0x7E00", 13, 10, 0
str_disk_error:   db "[DBL ERROR]: Disk read error!!!", 13, 10, 0

; === data of 16-bit real mode, 32-bit protected mode and 64-bit long mode. Stage 2 === ;
str_16_bit_start:    db "[DBL 16]: Real mode   [OK]", 13, 10, 0
str_16_long_mode_ok: db "[DBL 16]: has Long mode [OK]", 13, 10, 0
str_a20:             db "[DBL 16]: A20 enabled", 13, 10, 0
str_jump_to_32:      db "[DBL 16]: -> Protecterd Mode... ", 13, 10, 0
str_set_gdt_32:  db "[DBL 16]: Loading 32-bit GDT table...", 13, 10, 0

str_32_bit_start: db "[DBL 32]: Protected mode   [OK]", 13, 10, 0 
str_cpuid:        db "[DBL 32]: CPUID: Long Mode [OK]", 13, 10, 0
str_pages_ready:  db "[DBL 32]: Page tables ready", 13, 10, 0
str_jump_to_64:   db "[DBL 32]: -> Long Mode... ", 13, 10, 0
str_32_second_stage_clean_pages:    db "[DBL 32]: Cleaning 4-level paging structure", 13, 10, 0 
str_32_second_stage_pages_build:    db "[DBL 32]: Identity-mapped pages setup for first 64GiB", 13, 10, 0
str_set_gdt_64: db "[DBL 32]: Loading 64-bit GDT table...", 13, 10, 0

str_64_bit_supported:   db "[DBL 64]: Support   [OK]", 13, 10, 0 
str_64_bit_start:       db "[DBL 64]: Long mode [OK]", 13, 10, 0
str_64_bit_done:        db "[DBL 64]: All transitions done!", 13, 10, 0
str_64_bit_kernel_load: db "[DBL 64]: Loading kernel... ", 13, 10, 0 

str_loading_kernel:     db "[DBL]: Loading kernel from the disk...", 13, 10, 0

; --- Errors --- ;
str_no_cpuid:          db "[DBL ERROR]: No CPUID!", 13, 10, 0
str_kernel_load_error: db "[DBL ERROR]: Failed to load kernel!", 13, 10, 0 
str_no_long_mode:      db "[DBL ERROR]: No long mode!", 13, 10, 0
str_a20_failed:        db "[DBL ERROR]: Failed to enable A20", 13, 10, 0
str_e820_not_supported: db "[DBL ERROR]: BIOS E820 is not supported.", 13, 10, 0 
str_e820_loading_error: db "[DBL ERROR]: BIOS E820 failed to read next memory entry", 13, 10, 0
str_64_bit_not_supported: db "[DBL ERROR]: 64-bit not supported", 13, 10, 0

; === Memory data and constatns === ;
loader_file_num_of_blocks    equ 5 
kernel_file_num_of_blocks    equ 152 
user_prog_file_num_of_blocks equ 127

e820_mem_start      equ (0x7E00 + loader_file_num_of_blocks * 512)
e820_mem_end        equ (e820_mem_start + 4 * 512) 

loader_kernel_start     equ e820_mem_end
loader_kernel_end       equ (loader_kernel_start + kernel_file_num_of_blocks * 512)

loader_user_prog_start  equ loader_kernel_end
loader_user_prog_end    equ (loader_kernel_end - user_prog_file_num_of_blocks * 512)

paging_start        equ 0x20000
paging_table_size   equ 0x1000
mem_pml4    equ paging_start
mem_pdpe    equ (mem_pml4 + paging_table_size)
mem_pde     equ (mem_pdpe + paging_table_size)
paging_end  equ (mem_pde + (64 * paging_table_size))

kernel_new_start_virt             equ 0xffff800000200000
kernel_new_start                  equ 0x00200000
kernel_new_elf_text_header_offset equ 0x00001000

current_row:    dd 0x00 
current_column: dd 0x00 
screen_width:   dd 0x00 

; === GDT for 64-bit and 32-bit === ;
gdt32_table:
    ; --- Null descriptor --- ;
    dw      0x0000
    dw      0x0000 
    db      0x00 
    db      0x00 
    db      0x00 

    ; --- Code segment descriptor --- ;
    dw      0xFFFF 
    dw      0x0000 
    db      0x00 
    db      10011010b 
    db      11001111b 
    db      0x00 

    ; --- Data segment descriptor --- ;
    dw      0xFFFF 
    dw      0x0000 
    db      0x00 
    db      10010010b 
    db      11001111b 
    db      0x00 

gdt32_table_size equ ($ - gdt32_table)

gdt32_table_pointer:
    dw gdt32_table_size - 1 
    dd gdt32_table

gdt64_table:
    ; --- Null descriptor --- ;
    dw      0x0000
    dw      0x0000
    db      0x00
    db      0x00
    db      0x00
    db      0x00

    ; --- Code segment descriptor --- ;
    dw      0x0000
    dw      0x0000
    db      0x00
    db      10011000b
    db      00100000b
    db      0x00

    ; --- Data segment descriptor --- ;
    dw      0x0000
    dw      0x0000
    db      0x00
    db      10010010b
    db      0x00
    db      0x00


gdt64_table_size equ ($ - gdt64_table)

gdt64_table_pointer:
    dw  gdt64_table_size - 1    ; Limit = offset of last byte in table
    dd  gdt64_table
