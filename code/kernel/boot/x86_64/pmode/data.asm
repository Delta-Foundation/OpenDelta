%ifndef DBL64_PMODE_DATA_ASM
%define DBL64_PMODE_DATA_ASM

str_jump_to_32:  db "[DBL 16]: -> Protecterd Mode... ", 13, 10, 0
str_set_gdt_32:  db "[DBL 16]: Loading 32-bit GDT table...", 13, 10, 0

str_32_bit_start: db "[DBL 32]: Protected mode   [OK]", 13, 10, 0 
str_cpuid:        db "[DBL 32]: CPUID: Long Mode [OK]", 13, 10, 0
str_pages_ready:  db "[DBL 32]: Page tables ready", 13, 10, 0
str_jump_to_64:   db "[DBL 32]: -> Long Mode... ", 13, 10, 0
str_32_second_stage_clean_pages:    db "[DBL 32]: Cleaning 4-level paging structure", 13, 10, 0 
str_32_second_stage_pages_build:    db "[DBL 32]: Identity-mapped pages setup for first 64GiB", 13, 10, 0
str_set_gdt_64: db "[DBL 32]: Loading 64-bit GDT table...", 13, 10, 0

%endif
