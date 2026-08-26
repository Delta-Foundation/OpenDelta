%include "./print.asm"
%include "./data.asm"

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

        call test_A20
        jc  .done

    .failed:
        mov si, str_a20_failed
        call print_rmode

        jmp endless_loop

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

    mov     byte [es:di],   0x00 
    mov     byte [ds:si],   0xff

    cmp     byte [es:di],   0xff 

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

; ====================================== ;
; Read kernel and user program from disk ;
; ====================================== ;
read_kernel_from_disk:
    pusha 

    xor     edx,    edx 

    mov     eax,    
    mov     edx,    
