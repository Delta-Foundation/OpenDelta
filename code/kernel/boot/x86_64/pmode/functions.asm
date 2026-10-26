%ifndef DBL64_PMODE_FUNCTIONS_ASM
%define DBL64_PMODE_FUNCTIONS_ASM

%include "boot/x86_64/pmode/data.asm"
%include "boot/x86_64/print.asm"

[bits 16]

; ====================================== ;
; Read kernel and user program from disk ;
; ====================================== ;
read_kernel_from_disk:
    pusha

    mov     eax,    6                       ; kernel starts right after stage 2
    mov     esi,    kernel_file_num_of_blocks
    mov     ecx,    loader_kernel_start >> 4
    xor     edi,    edi

    .read_run:
        cmp     esi,    127
        jbe     .read_remainder

        mov     ebx,    127
        call    bios_extended_read_sectors_from_drive

        add     eax,    127
        add     ecx,    0x0FE0              ; 127 * 512 / 16
        sub     esi,    127
        jmp     .read_run

    .read_remainder:
        mov     ebx,    esi
        call    bios_extended_read_sectors_from_drive

    popa
    ret

read_user_prog_from_disk:
    ret

; |==========|
; | memzero  |
; |==========|
memzero:
  pusha
  cld

  xor   eax,    eax
  rep   stosb

  popa
  ret

; |================================================|
; | bios_e820_memory_map                           |
; |                                                |
; | Read (free) memory available on this machine   |
; |                                                |
; | Killed registers:                              |
; |     None                                       |
; |================================================|
bios_e820_memory_map:
    pusha

    mov   dword [e820_entry_count], 0

    mov   edi,    e820_mem_start
    mov   ecx,    (e820_mem_end - e820_mem_start)
    call  memzero

    mov   esi,    e820_mem_start + 8
    xor   ebx,    ebx

    .loop:
        mov   eax,  0xe820
        mov   edx,  0x534d4150
        mov   ecx,  24
        mov   edi,  esi
        int   0x15

        jc    .failed
        cmp   eax,  0x534d4150
        jne   .failed

        inc   dword [e820_entry_count]
        add   esi,  24

        cmp   esi,  e820_mem_end - 24
        jae   .done

        test  ebx,  ebx
        jz    .done
        jmp   .loop

    .failed:
        cmp   dword [e820_entry_count], 0
        jne   .done

        mov   si,   str_e820_not_supported
        call  print_rmode

        jmp   pmode_endless_loop

    .done:
        clc
        popa
        ret

; |=============================================================================|
; | has_cpuid                                                                   |
; | Detect if the cpu supports the CPUID instruction.                           |
; |                                                                             |
; | Bit 21 of the EFLAGS register can be modified only if the CPUID instruction |
; | is supported.                                                               |
; |                                                                             |
; | Return flags:                                                               |
; |     CF      Set if CPUID is supported                                       |
; |                                                                             |
; | Killed registers:                                                           |
; |     None                                                                    |
; |=============================================================================|
has_cpuid:
    push    eax
    push    ecx

    pushfd
    pop     eax
    mov     ecx,    eax

    xor     eax,    (1 << 21)
    push    eax
    popfd

    pushfd
    pop     eax

    push    ecx
    popfd

    clc

    xor     eax,    ecx
    jz .done

    .supported:
        stc

    .done:
        pop     ecx
        pop     eax

        ret

cpu_supports_64_bit_mode:
    call    has_cpuid
    jnc     .error_no_cpuid 

    mov     eax,    0x80000000
    cpuid
    cmp     eax,    0x80000001
    jb      .error_no_64_bit_mode 

    mov     eax,    0x80000001
    cpuid
    test    edx,    (1 << 29)
    jz      .error_no_64_bit_mode

    xor     eax,    eax
    xor     edx,    edx

    mov     si,     str_64_bit_supported
    call    print_rmode
    
    ret

    .error_no_cpuid: 
        mov     si,     str_no_cpuid
        call    print_rmode
      
        jmp     pmode_endless_loop

    .error_no_64_bit_mode:
        mov     si,     str_64_bit_not_supported
        call    print_rmode
 
        jmp     pmode_endless_loop

; |===========================================================|
; | move_kernel                                               |
; |                                                           |
; | Move the kernel blocks to a different location in memory  |
; |                                                           |
; | Killed registers:                                         |
; |     None                                                  |
; |===========================================================|
[bits 32]
move_kernel:
    pusha 

    cld 
    xor     edi,    edi 
    xor     ecx,    ecx 
    xor     esi,    esi 

    mov     edi,    kernel_new_start
    mov     esi,    loader_kernel_start
    mov     ecx,    (512 * kernel_file_num_of_blocks) / 4 
    rep     movsd 

    popa 
    ret 

; |=======================================================================================|
; | set_cursor                                                                            |
; |                                                                                       |
; | Set the hardware cursor position based on the current columnt (current_column) and    |
; | current row (current_row) coordinates                                                 |
; | See:      https://wiki.osdev.org/Text_Mode_Cursor#Moving_the_Cursor_2                 |
; | See:      https://stackoverflow.com/a/53866689/832748                                 |
; |                                                                                       |
; | Killed registers:                                                                     |
; |     ecx, edx                                                                          |
; |=======================================================================================|
set_cursor:
    mov     ecx,    [current_row]
    imul    ecx,    [screen_width]
    add     ecx,    [current_column]

    mov     edx,    0x3d4 
    mov     al,     0x0f 

    out     dx,     al 
    inc     edx
    mov     al,     cl 
    out     dx,     al 

    dec     edx 
    mov     al,     0x0e 
    out     dx,     al 
    inc     edx 
    mov     al,     ch 
    out     dx,     al 

    ret 

; |===============================================================================|
; | retrive_video_cursor_settings                                                 |
; |                                                                               |
; | Obtains BIOS cursor position that was used in real mode so new messages can   |
; | be printed where the BIOS stopped at                                          |
; |                                                                               |
; | Killed registers:                                                             |
; |     None                                                                      |
; |===============================================================================|
retrive_video_cursor_settings:
    mov     dword [current_row],    0
    mov     dword [current_column], 0
    mov     dword [screen_width],   80
    ret 

; |=============================================================================|
; | setup_page_tables                                                           |
; |                                                                             |
; | Setup Paging mechanism used in long mode (64-bits). This routine cleans the |
; | memory used, create pages and enable paging                                 |
; |                                                                             |
; | Return flags:                                                               |
; |     None                                                                    |
; |                                                                             |
; | Killed registers:                                                           |
; |     None                                                                    |
; |=============================================================================|
setup_page_tables:
    pusha

    .clean_memory:
        mov     eax,    str_32_second_stage_clean_pages
        call    print_pmode

        cld
        xor     eax,    eax
        xor     ecx,     ecx

        mov     edi, mem_pml4
        mov     ecx, (paging_end - paging_start) >> 2

        rep     stosd

    .setup_tables:

        .std_bits   equ 0x03
        .pde_ps	    equ (1 << 7)

        mov dword [mem_pml4], (mem_pdpe) | .std_bits

        mov dword [mem_pml4 + 256 * 0x08], (mem_pdpe) | .std_bits

        mov dword [mem_pdpe], (mem_pde) | .std_bits

        mov     edi,    mem_pde
        mov     ecx,    512
        mov     eax,    .std_bits | .pde_ps

        .make_pde_page:
            mov     [edi],  eax
            add     edi,    0x8
            add     eax,    0x200000
            loop    .make_pde_page

    mov     eax,    str_32_second_stage_pages_build
    call    print_pmode 

    popa
    ret

; |=======================================================|
; | switch_long_mode                                      |
; |                                                       |
; | Prepare the system to enter the long mode (64-bits).  |
; |                                                       |
; | Return flags:                                         |
; |     None                                              |
; |                                                       |
; | Killed registers:                                     |
; |     All, nothing will be saved.                       |
; |=======================================================|
switch_long_mode:
    cli 

    mov     si,     str_set_gdt_64 
    call    print_pmode 

    lgdt    [gdt64_table_pointer]
 
    mov     si,     str_jump_to_64
    call    print_pmode 

    mov     eax,    cr4 
    or      eax,    (1 << 5)      ; PAE
    or      eax,    (1 << 7)      ; PGE
    or      eax,    (1 << 9)      ; OSFXSR - enable SSE
    or      eax,    (1 << 10)     ; OSXMMEXCPT
    mov     cr4,    eax 

    mov     eax,    paging_start
    mov     cr3,    eax 

    mov     ecx,    0xc0000080
    rdmsr
    or      eax,    (1 << 8)
    wrmsr

    mov     eax,    cr0
    or      eax,    (1 << 31)
    mov     cr0,    eax 

    jmp     0x08:long_mode

pmode_endless_loop:
    cli 
    .end:
        hlt
        jmp .end

%endif
