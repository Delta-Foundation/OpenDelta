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
str_loading_32_gdt:  db "[DBL 16]: Loading 32-bit GDT", 13, 10, 0

str_32_bit_start: db "[DBL 32]: Protected mode   [OK]", 13, 10, 0 
str_cpuid:        db "[DBL 32]: CPUID: Long Mode [OK]", 13, 10, 0
str_pages_ready:  db "[DBL 32]: Page tables ready", 13, 10, 0
str_jump_to_64:   db "[DBL 32]: -> Long Mode... ", 13, 10, 0

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
