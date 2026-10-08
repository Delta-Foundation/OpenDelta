%ifndef DBL64_RMODE_DATA_ASM
%define DBL64_RMODE_DATA_ASM

; === data of 16-bit real mode. Boot sector === ;
boot_drive:                 db 0
bios_dap_read_packet:       times 16 db 0
loader_file_num_of_blocks:  equ 5
str_stage1:       db "[DBL]: STAGE 1", 13, 10, 0
str_16_loading:   db "[DBL 16]: Loading second stage of boot...", 13, 10, 0
str_16_loaded:    db "[DBL 16]: Stage 2 loaded at 0x7E00", 13, 10, 0
str_disk_error:   db "[DBL ERROR]: Disk read error!!!", 13, 10, 0
str_bios_disk_extension_loading_error: db "[DBL 16]: BIOS Disk extension failed to read loader", 13, 10, 0
str_bios_disk_extension_present:       db "[DBL 16]: BIOS Disk extension is present", 13, 10, 0 
str_bios_disk_extension_not_present:   db "[DBL 16]: BIOS Disk extension is not present", 13, 10, 0

%endif
