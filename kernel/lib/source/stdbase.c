#include "../stdbase.h"
#include "../types.h"
#include "../string.h"
#include "../../fs/headers/fs.h"
#include "../stdarg.h"
#include "../system.h"
#include "../ctype.h"

FILE _iob[OPEN_MAX];
static Header base;
static Header *freep = NULL;
static char *dataStart = (char *)0x100000;
static char *dataEnd = (char *)&dataStart;
static ungetc_buf_t ungetc_buf = { EOF, 0, NULL };
uint32_t freeMemAddr = 0x10000;
const char g_hex_chars[] = "0123456789abcdef";

int _fillbuf(FILE *stream)
{
    if (stream->cnt > 0) {
        return (unsigned char)*stream->ptr++;
    }

    for (int i = 0; i < BUFSIZE; i++) {
        stream->buf[i] = inb(stream->port);
        if (stream->buf[i] == EOF) {
            stream->cnt = i;
            break;
        }
    }


    if (stream->cnt == 0) {
        return EOF;
    }

    stream->ptr = stream->base = stream->buf;
    stream->cnt = BUFSIZE - 1;

    return (uint8_t)*stream->ptr++;
}

int _flushbuf(int c, FILE *stream)
{
    if (stream->cnt == NULL) {
        outb(stream->port, *(stream->ptr)++);
        stream->cnt--;
        return c;
    }

    for (int i = 0; i < BUFSIZE; i++) {
        outb(stream->port, stream->buf[i]);
    }

    stream->ptr = stream->base = stream->buf;
    stream->cnt = BUFSIZE - 1;

    if (c != EOF) {
        outb(stream->port, c);
    }

    return c;
}

char sbrk(int incr)
{
    char *oldEnd = dataEnd;
    char *newEnd = oldEnd + incr;

    if (newEnd < oldEnd) {
        return (char) -1;
    }

    dataEnd = newEnd;

    return *oldEnd;
}

static Header *morecore(unsigned nu)
{
    char *cp;
    Header *up;

    if (nu < NALLOC) {
        nu = NALLOC;
    }

    *cp = sbrk(nu * sizeof(Header));
    if (cp == (char *) - 1) {
        return NULL;
    }
    up = (Header *) cp;
    up->s.size = nu;
    free((void *) (up + 1));
    return freep;
}

void *free(void *ap)
{
    Header *bp, *p;
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr) {
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr)) {
            break;
        }
    }

    if (bp + bp->s.size == p->s.ptr) {
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    }
    else {
        bp->s.ptr = p->s.ptr;
    }

    if (p + p->s.size == bp) {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    }
    else {
        p->s.ptr = bp;
    }

    freep = p;
}

void *malloc(unsigned nbytes)
{
    Header *p, *prevp;
    unsigned nunits;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
    if ((prevp = freep) == NULL) {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) {
            if (p->s.size == nunits) {
                prevp->s.ptr = p->s.ptr;
            }
            else {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)(p + 1);
        }

        if (p == freep) {
            if ((p = morecore(nunits)) == NULL) {
                return NULL;
            }
        }
    }
}

void *kmalloc(unsigned int size, int align, unsigned int *phys_addr)
{
	if (align == 1 && (freeMemAddr & 0xFFFFF000)) {
		freeMemAddr &= 0xFFFFF000;
		freeMemAddr += 0x1000;
	}
	if (phys_addr) {
		*phys_addr = freeMemAddr;
	}

    void *ret = (void *)freeMemAddr;
	freeMemAddr += size;
	return ret;
}

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a" (val), "Nd" (port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

void fprintf_unsigned(FILE *file, unsigned int num, int radix) 
{
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

void fprintf_signed(FILE *file, long long num, int radix) 
{
    if (num < 0) {
        fputc('-', file);
        fprintf_unsigned(file, -num, radix);
    }
    else {
        fprintf_unsigned(file, num, radix);
    }
}

void prints(const char *fmt, Colors color, ...)
{
    char *video_mem = (char *)0xb8000;
    int i = 0;
    int pos = 0;
    while (fmt[i] != '\0') {

        if (fmt[i] == '\n') {
            pos = ((pos / 80) + 1) * 80;
        }

        else {
            video_mem[2*pos] = fmt[i];
            video_mem[2*pos + 1] = (char)color;
            pos++;
        }

        i++;
    }
}

void vfprintf(FILE *file, const char *fmt, va_list args) 
{
    int state = PRINTF_STATE_NORMAL;
    int len = PRINTF_LENGTH_DEFAULT;
    int rad = 10;
    boolean *sign = FALSE;
    boolean *num = FALSE;
    
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
                        fputc(va_arg(args, char), file);
                    break;

                    case '%':   
                         fputc('%', file);
                    break;

                    case 'd':
                    case 'i':   
                        rad = 10;
                        sign = TRUE; 
                        num = TRUE;
                    break;

                    case 'u':   
                        rad = 10; 
                        sign = FALSE; 
                        num = TRUE;
                    break;

                    case 'X':
                    case 'x':
                    case 'p':   
                        rad = 16; 
                        sign = FALSE; 
                        num = TRUE;
                    break;

                    case 'o':   
                        rad = 8; 
                        sign = FALSE; 
                        num = TRUE;
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

void fprintf(FILE *file, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args);
    va_end(args);
}

void fprintf_buffer(FILE *file, const char* msg, const void* buf, uint32_t count) {
    const uint8_t* u8_buf = (const uint8_t*)buf;

    fputs(msg, file);
    for (uint16_t i = 0; i < count; i++) {
        fputc(g_hex_chars[u8_buf[i] >> 4], file);
        fputc(g_hex_chars[u8_buf[i] & 0xF], file);
    }
    fputs("\n", file);
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf((FILE *)stdout, fmt, args);
    va_end(args);
}

void printf_buffer(const char* fmt, const void* buf, uint32_t count) {
    fprintf_buffer((FILE *)stdout, fmt, buf, count);
}

void debugc(char c) {
    fputc(c, (FILE *)debug);
}

void debugs(const char* str) {
    fputs(str, (FILE *)debug);
}

void debugf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf((FILE *)debug, fmt, args);
    va_end(args);
}

void debug_buffer(const char* msg, const void* buf, uint32_t count) {
    fprintf_buffer((FILE* )debug, msg, buf, count);
}

void fputs(const char *str, FILE *file) {
    while (*str) {
        fputc(*str, file);
        str++;
    }
}

void fputc(char c, FILE *file) {
    vfs_write((fd_t *)file, (unsigned char *)&c, sizeof(c));
}

int _getc(FILE *stream)
{
    if (--(stream)->cnt >= 0) {
        return (unsigned char) *(stream)->ptr;
    }
    else {
        return _fillbuf(stream);
    }
}

int ngetc(FILE* stream) {
    if (ungetc_buf.has_buf && ungetc_buf.stream == stream) {
        ungetc_buf.has_buf = 0;
        return ungetc_buf.buf;
    }

    return fgetc(stream);
}

int _putc(char c, FILE *stream)
{
    if (--(stream)->cnt >= 0) {
        return *(stream)->ptr++ = (c);
    }
    else {
        return  _flushbuf((c), stream);
    }
}

char *fgets(char *s, int n, FILE * iop)
{
    register int c;
    register char *cs;

    cs = s;
    while (--n > 0 && (c = _getc(iop)) != EOF) {
        if ((*cs++ = c) == '\n') {
            break;
        }
    }

    *cs = '\0';
    return (c == EOF && cs == s) ? NULL : s;
}

int fgetc(FILE* file) 
{
    if (!file || file->error) {
        return EOF;
    }   
    
    if (file->buf_pos >= file->buf_size) {
        if (file->flag) {
            return EOF;
        }

        unsigned int bytes_read = sysRead(file->fd, file->buf, BUFSIZE);
        if (bytes_read < 0) {
            file->error = 1;
            return EOF;
        }

        file->buf_size = (unsigned int)bytes_read;
        file->buf_pos = 0;
    }

    unsigned char ch = (unsigned char)file->buf[file->buf_pos++];
    return (int)ch;
}

int _fputs(char *s, FILE *iop)
{
    int c;

    while ((c == *s++) != '\0') {
        _putc(c, iop);
    }

    return ferror(iop) ? EOF : 0;
}

void fcopy(FILE *ifp, FILE *ofp) {
    int c;

    while ((c = _getc(ifp)) != EOF) {
        _putc(c, ofp);
    }
}

int getline(char *line, int max)
{
    if (fgets(line, max, stdin) == NULL) {
        return 0;
    }
    else {
        return strlen(line);
    }
}

FILE *fopen(char *name, char *mode) 
{
    int fd;
    FILE *fp;

    if (*mode != 'r' && *mode != 'w' && *mode != 'a') {
        return NULL;
    }

    for (fp = _iob; fp < _iob + OPEN_MAX; fp++) {
        if ((fp->flag & (_READ | _WRITE)) == 0) {
            break;
        }
    }

    if (fp >= _iob + OPEN_MAX) {
        return NULL;
    }

    if (*mode == 'w') {
        fd = sysCreate(name, PERMS);
    }
    else if (*mode == 'a') {
        if ((fd == sysOpen(name, O_WRONLY, 0)) == -1) {
            fd = sysCreate(name, PERMS);
        }
        lseek(fd, 0L, 2);
    }
    else {
        fd = sysOpen(name, O_RDONLY, 0);
    }

    if (fd == -1) {
        return NULL;
    }

    fp->fd = fd;
    fp->cnt = 0;
    fp->base = NULL;
    fp->flag = (*mode == 'r') ? _READ : _WRITE;
    return fp; 
}

int fclose(FILE *file) {
    if (!file) {
        return -1;
    }

    int ret = sysClose(file->fd);
    return ret;
}

int ungetc(int c, FILE* stream) 
{
    if (c == EOF) {
        return EOF;
    }

    if (ungetc_buf.has_buf) {
        return EOF;
    }

    ungetc_buf.buf = c;
    ungetc_buf.has_buf = 1;
    ungetc_buf.stream = stream;
}

void skip_whitespace(FILE* stream) 
{
    int c;
    while ((c = _getc(stream)) != EOF && isspace(c)) { ; }

    if (c != EOF) {
        ungetc(c, stream);
    }
}

int scanf(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    int count = 0;
    FILE* stream = stdin;
    
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
