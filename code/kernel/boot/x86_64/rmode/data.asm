%ifndef DBL64_RMODE_DATA_ASM
%define DBL64_RMODE_DATA_ASM

; === data of 16-bit real mode. Boot sector === ;
boot_drive:                 db 0
bios_disk_status:           db 0
bios_dap_read_packet:       times 16 db 0
loader_file_num_of_blocks:  equ 5
loader_stage1_stack_top:    equ 0x7c00

loader_stage2_entry:        equ 0x1000
str_stage1:       db "[DBL]: STAGE 1", 13, 10, 0
str_16_loading:   db "[DBL 16]: Loading second stage of boot...", 13, 10, 0
str_16_loaded:    db "[DBL 16]: Stage 2 loaded at 0x1000", 13, 10, 0
str_bios_disk_extension_loading_error: db "[DBL 16]: BIOS Disk extension failed to read Stage 2 of DBL", 13, 10, 0
str_bios_disk_extension_present:       db "[DBL 16]: BIOS Disk extension is present", 13, 10, 0
str_bios_disk_extension_not_present:   db "[DBL 16]: BIOS Disk extension is not present", 13, 10, 0

%endif
