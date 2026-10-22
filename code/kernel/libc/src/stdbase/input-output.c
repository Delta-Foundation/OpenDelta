/* Headers from dltlibc */
#include "../../include/stdbase.h"
#include "../../include/types.h"
#include "../../include/string.h"
#include "../../include/ctype.h"
#include "../../include/stdarg.h"

/* Headers from kernel */
#include "../../../fs/headers/fs.h"
#include "../../../kernel-lib/system.h"

const char g_hex_chars[] = "0123456789abcdef";

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a" (val), "Nd" (port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

void fprintf_unsigned(
    FILE * file, 
    unsigned int num,
    int radix
) {
    char buf[32];
    int pos = 0;

    do {
        unsigned int rem = num % radix;
        num /= radix;
        buf[pos++] = g_hex_chars[rem];
    } while (num > 0);

    while (--pos >= 0) {
        fputc(buf[pos], file);
    }
}

void fprintf_signed(
    FILE * file,
    long long num,
    int radix
) {
    if (num < 0) {
        fputc('-', file);
        fprintf_unsigned(file, -num, radix);
    }
    else {
        fprintf_unsigned(file, num, radix);
    }
}

void prints(
    const char * fmt,
    Colors color, 
    ...
) {
    volatile uint16_t * video_mem = (volatile uint16_t *)0xB8000;

    int cursor_x = 0;
    int cursor_y = 0;

    while (*fmt != '\0') {
        if (*fmt == '\n') {
            cursor_x = 0;
            cursor_y++;
        }
        else {
            int pos = cursor_y * 80 + cursor_x;
            video_mem[pos] = (uint16_t)(*fmt) | ((uint16_t)color << 8);
            cursor_x++;
        }

        fmt++;
    }
}

void vfprintf(
    FILE * file, 
    const char *fmt,
    va_list args
) {
    int state = PRINTF_STATE_NORMAL;
    int len = PRINTF_LENGTH_DEFAULT;
    int rad = 10;
    boolean * sign = FALSE;
    boolean * num = FALSE;
    
    while (*fmt) {
        switch (state) {
            case PRINTF_LENGTH_DEFAULT:
                switch (*fmt) {
                    case '%': 
                        state = PRINTF_STATE_LENGTH;
                    break;
                    
                    default:
                        fputc(*fmt, file);
                    break;
                }
            break;            

            case PRINTF_STATE_LENGTH:
                switch (*fmt) {
                    
                    case 'h':
                        len = PRINTF_LENGTH_SHORT;
                        state = PRINTF_STATE_LENGTH_SHORT;
                    break;
                        
                    case 'l':
                        len = PRINTF_LENGTH_LONG;
                        state = PRINTF_STATE_LENGTH_LONG;
                     break;
                    
                    default: 
                        goto PRINTF_STATE_SPEC_;
        
                }
            break;
            
            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h') {
                    len = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else {
                    goto PRINTF_STATE_SPEC_;
                }
            break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt) {

                    case 'c':
                        fputc((char)va_arg(args, int), file);
                    break;
                    
                    case 's':
                        fputc((char)va_arg(args, int), file);
                    break;

                    case '%':   
                         fputc('%', file);
                    break;

                    case 'd':
                    case 'i':   
                        rad = 10;
                        sign = (char *)TRUE; 
                        num = (char *)TRUE;
                    break;

                    case 'u':   
                        rad = 10; 
                        sign = FALSE; 
                        num = (char *)TRUE;
                    break;

                    case 'X':
                    case 'x':
                    case 'p':   
                        rad = 16; 
                        sign = FALSE; 
                        num = (char *)TRUE;
                    break;

                    case 'o':   
                        rad = 8; 
                        sign = FALSE; 
                        num = (char *)TRUE;
                    break;

                    default:    break;
                }

                if (num) {
                    if (sign) {
                        switch (len) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:     
                                fprintf_signed(file, va_arg(args, int), rad);
                            break;

                            case PRINTF_LENGTH_LONG:        
                                fprintf_signed(file, va_arg(args, long), rad);
                            break;

                            case PRINTF_LENGTH_LONG_LONG:   
                                fprintf_signed(file, va_arg(args, long long), rad);
                            break;
                        }
                    }

                    else {
                        switch (len) {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:     
                                fprintf_unsigned(file, va_arg(args, unsigned int), rad);
                            break;
                                                        
                            case PRINTF_LENGTH_LONG:        
                                fprintf_unsigned(file, va_arg(args, unsigned  long), rad);
                            break;

                            case PRINTF_LENGTH_LONG_LONG:   
                                fprintf_unsigned(file, va_arg(args, unsigned  long long), rad);
                            break;
                        }
                    }
                }

                state = PRINTF_STATE_NORMAL;
                len = PRINTF_LENGTH_DEFAULT;
                rad = 10;
                sign = FALSE;
                num = FALSE;

                break;
        }

        fmt++;
    }
}

void fprintf(
    FILE * file,
    const char * fmt, 
    ...
) {
    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args);
    va_end(args);
}

void fprintf_buffer(
    FILE * file,
    const char * fmt,
    const void * buf, 
    uint32_t count
) {
    const uint8_t * u8_buf = (const uint8_t *)buf;

    fputs(fmt, file);
    for (uint16_t i = 0; i < count; i++) {
        fputc(g_hex_chars[u8_buf[i] >> 4], file);
        fputc(g_hex_chars[u8_buf[i] & 0xF], file);
    }
    fputs("\n", file);
}

void printf(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf((FILE *)stdout, fmt, args);
    va_end(args);
}

void printf_buffer(
    const char * fmt,
    const void * buf,
    uint32_t count
) {
    fprintf_buffer((FILE *)stdout, fmt, buf, count);
}


void skip_whitespace(FILE * stream) 
{
    int c;
    while ((c = _getc(stream)) != EOF && isspace(c)) { ; }

    if (c != EOF) {
        ungetc(c, stream);
    }
}

int scanf(const char * fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    int count = 0;
    FILE * stream = stdin;
    
    while (*fmt != '\0') {
        if (*fmt == ' ') {
            skip_whitespace(stream);
            fmt++;
            continue;
        }

        if (*fmt != '%') {
            int c = ngetc(stream);
            if (c != *fmt) {
                ungetc(c, stream);
                break;
            }
            fmt++;
            continue;
        }
        
        fmt++;
        
        int width = 0;
        while (isdigit((unsigned char)*fmt)) {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }

        char type = *fmt;
        fmt++;
        
        switch (type) {
            case 'd': {
                int *num = va_arg(args, int*);
                int val = 0;
                int sign = 1;
                int c;
                int digit_count = 0;
    
                skip_whitespace(stream);

                c = ngetc(stream);
                if (c == '-') {
                    sign = -1;
                    c = ngetc(stream);
                }
                else if (c == '+') {
                    c = ngetc(stream);
                } 

                while (isdigit(c) && (width == 0 || digit_count < width)) {
                    val = val * 10 + (c - '0');
                    c = ngetc(stream);
                    digit_count++;
                }
                
                ungetc(c, stream);

                *num = sign * val;
                count++;
                break;
            }
            
            case 's': {
                char *str = va_arg(args, char*);
                int i = 0;
                int c;

                skip_whitespace(stream);
                
                while ((c = ngetc(stream)) != EOF && !isspace((unsigned char)c) && (width == 0 || i < width)) {
                    str[i++];                    
                }
                str[i] = '\0';
                
                if (c != EOF) { 
                    ungetc(c, stream);
                }
                
                count++;
                break;
            }
            
            case 'c': {
                char *ch = va_arg(args, char*);
                *ch = (char)ngetc(stream);
                count++;
                break;
            }
            
            default:
            break;
        }
    }

    va_end(args);
    return count;
}
