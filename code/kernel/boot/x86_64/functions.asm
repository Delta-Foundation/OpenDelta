%include "./boot/x86_64/print.asm"
%include "./boot/x86_64/data.asm"

; +------------------+
; |                  |
; | 16-bit functions |
; |                  |
; +------------------+

; ======================== ;
;   Enable and test A20    ;
; ======================== ;

enable_A20:
    push    ax

    call    test_A20
    jc      .done

    .attempt1:
        mov     ax,     0x2401
        int     0x15

        call    test_A20
        jc      .done

    .attempt2:
        call    .attempt2.wait1

        mov     al,     0xad
        out     0x64,   al
        call    .attempt2.wait1

        mov     al,     0xd0
        out     0x64,   al
        call    .attempt2.wait2

        in      al,     0x60
        push    eax
        call    .attempt2.wait1

        mov     al,     0xd1
        out     0x64,   al
        call    .attempt2.wait1

        pop     eax
        or      al,     2
        out     0x60,   al
        call    .attempt2.wait1

        mov     al,     0xae
        out     0x64,   al
        call    .attempt2.wait1

        call    test_A20
        jc      .done

        jmp     .attempt3

        .attempt2.wait1:
            in      al,     0x64
            test    al,     2
            jnz     .attempt2.wait1
            ret

        .attempt2.wait2:
            in      al,     0x64
            test    al,     1
            jz      .attempt2.wait2
            ret

    .attempt3:
        in      al,     0x92
        or      al,     2
        out     0x92,   al
        xor     ax,     ax

        call    test_A20
        jc      .done

    .failed:
        mov     si,     str_a20_failed
        call    print_rmode

        jmp     endless_loop

    .done:
        pop     ax
        ret

test_A20:
    push    ds
    push    es 
    pusha 

    clc 

    xor     ax,     ax
    mov     es,     ax 

    not     ax 
    mov     ds,     ax 

    mov     di,     0x0500
    mov     si,     0x0510

    mov     ax,     [es:di]
    push    ax
    mov     ax,     [ds:si]
    push    ax 

    mov byte [es:di], 0x00 
    mov byte [ds:si], 0xff

    cmp byte [es:di], 0xff 

    pop     ax 
    mov     [ds:si],    ax 
    pop     ax 
    mov     [es:di],    ax 

    je      .done 

    .enabled:
        stc 

    .done:
        popa 
        pop     es 
        pop     ds 
        ret 

; |==============================================================|
; | switch_protected_mode                                        |
; |                                                              |
; | Prepare the system to enter the protected mode (32-bits).    |
; |                                                              |
; | Return flags:                                                |
; |     None                                                     |
; |                                                              |
; | Killed registers:                                            |
; |     All, nothing will be saved.                              |
; |==============================================================|
switch_protected_mode:
    cli

    mov     si,     str_set_gdt_32
    call    print_rmode

    lgdt    [gdt32_table_pointer]

    mov     si,     str_jump_to_32
    call    print_rmode

    mov     eax,    cr0
    or      eax,    (1 << 0)    ; CR.PE
    mov     cr0,    eax

    jmp     0x08:protected_mode

; +------------------+
; |                  |
; | 32-bit functions |
; |                  |
; +------------------+

; ====================================== ;
; Read kernel and user program from disk ;
; ====================================== ;
read_kernel_from_disk:
    pusha 

    xor     edx,    edx 

    mov     eax,    loader_kernel_start
    mov     edx,    152
    mov     ecx,    6 

    .read_run:
  	    cmp     edx,    127
  	    jle     .read_remainder

    .read_block:
  	    mov     ebx,    127 
  	    call    bios_extended_read_sectors_from_drive

  	    add     ecx,    127 
  	    sub     edx,    127 
  	    add     eax,    (127 * 512)
  	    jmp     .read_run

    .read_remainder:
  	    mov     ebx,    edx
  	    call    bios_extended_read_sectors_from_drive

    popa
    ret

read_user_prog_from_disk:
    pusha

    xor     edx, edx

    mov     eax,    loader_user_prog_start 
    mov     edx,    user_prog_file_num_of_blocks
    mov     ecx,    158

    .read_run:
  	    cmp     edx,    127 
  	    jle     .read_remainder

    .read_block:
  	    mov     ebx,    127 
  	    call    bios_extended_read_sectors_from_drive

  	    add     ecx,    127 
  	    sub     edx,    127 
  	    add     eax,    (127 * 512)
  	    jmp     .read_run

    .read_remainder:
  	    mov     ebx,    edx
  	    call    bios_extended_read_sectors_from_drive

    mov     si,     str_loading_kernel
    call    print_rmode 

    popa
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

    mov   edi,    e820_mem_start
    mov   ecx,    (e820_mem_end - e820_mem_start)
    call  memzero

    .first_memory_entry:
        mov   eax,  0xe820
        xor   ebx,  ebx
        mov   edx,  0x534d4150
        mov   edi,  e820_mem_start 
        add   edi,  8
        mov   ecx,  20
        int   0x15

        jc    .not_supported

    .loop:
        inc dword [e820_mem_start]

        test  ebx,  ebx
        jz  .done

        mov   eax,  0xe820
        mov   edx,  0x534d4150
        mov   ecx,  20
        add   di,   cx
        int   0x15

        jc  .error

        cmp   eax,  0x534d4150
        jne   .error

        jmp   .loop

    .not_supported:
        mov   si,   str_e820_not_supported
        call  print_rmode

        jmp   endless_loop

    .error:
        mov   si,   str_e820_loading_error
        call  print_rmode

        jmp   endless_loop

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
      
        jmp     endless_loop

    .error_no_64_bit_mode:
        mov     si,     str_64_bit_not_supported
        call    print_rmode
 
        jmp     endless_loop

; |===========================================================|
; | move_kernel                                               |
; |                                                           |
; | Move the kernel blocks to a different location in memory  |
; |                                                           |
; | Killed registers:                                         |
; |     None                                                  |
; |===========================================================|
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
    push    eax 
    
    xor     eax,    eax 
    mov     al,     [0x45]
    mov     [current_row],  eax 
    mov     al,     [0x450 + 1]
    mov     [current_row],  eax 
    mov     ax,     word [0x44a]
    mov     [screen_width], eax 

    pop     eax 
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
        mov     ecx, (paging_end - paging_end) >> 2

        rep     stosd

    .setup_tables:

        .std_bits   equ 0x03
        .pde_ps	    equ (1 << 7)

        mov dword [mem_pml4], (mem_pdpe) | .std_bits

        mov dword [mem_pml4 + 256 * 0x08], (mem_pdpe) | .std_bits

        mov     edi,    mem_pdpe
        mov     ecx,    64
        mov     eax,    mem_pde | .std_bits

        .make_pdpe_page:
            mov     [edi],  eax
            add     edi,    0x8
            add     eax,    0x1000
            loop    .make_pdpe_page

        mov     edi,    mem_pde
        mov     ecx,    512 * 64
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
    or      eax,    (1 << 5)
    or      eax,    (1 << 7)
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

endless_loop:
    cli 
    .end:
        hlt
        jmp .end 
