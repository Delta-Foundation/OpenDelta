#ifndef BASE_H
#define BASE_H

#pragma once

#include "./types.h"
#include "./stdarg.h"

#ifdef __cplusplus 
extern "C" {
#endif 

#define ALIGN 8
#define MAGIC 0xB16B00B5U

/*---for-screen-driver---*/
#define LINES 				30
#define COLUMNS_IN_LINE 		85
#define BYTES_FOR_EACH_ELEMENT 		2
#define SCREENSIZE 			BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 		0x60
#define KEYBOARD_STATUS_PORT 		0x64
#define INTERRUPT_GATE 			0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 	0x10000

#define ENTER_KEY_CODE 			0x1C
#define ENTER_BACKSPACE_CODE		0x0E

#define REG_SCREEN_CTRL			0x3d4
#define REG_SCREEN_DATA			0x3d5

static unsigned int currentLoc = 0;
static char *vidptr = (char*)0xb8000;

/* colors */
typedef enum {
    WHITE = 0x0F,
    BLACK = 0x0,
    CYAN = 0x3,
    DGRAY = 0x8,   // Dark Gray
    LCYAN = 0xB,   // Light Cyan
    LGRAY = 0x7    // Light Gray
} Colors;

/*---structures-for-system---*/
typedef struct {
    uint32_t magic;
    size_t size;
    int free;
    struct block_header *prev;
    struct block_header *next;
} block_header_t;

/*---For-Base---*/
#define EOF (-1)
#define BUFSIZE 1024
#define OPEN_MAX 20
#define NAME_MAX 14 

#define S_IFMT 0160000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IDBLK 0060000
#define S_IFREG 0100000

#define stdin 0
#define stdout 1
#define stderr 2
#define debug 3

#define NALLOC 1024

#define feof(p) (((p)->flag & _EOF) != 0)
#define ferror(p) (((p)->flag & _ERR) != 0)
#define fileno(p) ((p)->fd)

#define e9_putc(c) outb(0xE9, c);

#define getchar()   _getc(stdin)
#define putchar(x)  _putc((x), stdout)

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4

#define O_RDONLY    00000000  
#define O_WRONLY    00000001  
#define O_RDWR      00000002  

#define O_CREAT     00000100  
#define O_TRUNC     00001000  
#define O_APPEND    00002000  

#define S_IRWXU     00700     
#define S_IRUSR     00400     
#define S_IWUSR     00200     
#define S_IXUSR     00100     

#define S_IRWXG     00070     
#define S_IRGRP     00040     
#define S_IWGRP     00020     
#define S_IXGRP     00010     

#define S_IRWXO     00007     
#define S_IROTH     00004     
#define S_IWOTH     00002     
#define S_IXOTH     00001     

#define S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

#define PERMS 0666

/*---структуры---*/
typedef struct _iobuf {
    uint16_t port;
    uint8_t buf[BUFSIZE];
    int buf_pos;
    int buf_size[BUFSIZE];
    int cnt;
    char *ptr;
    char *base;
    int flag;
    int fd;
    int error;
} FILE;

typedef struct {
    long ino;
    char name[NAME_MAX + 1];
} Dirent;

typedef struct {
    int fd;
    Dirent d;
} DIR;

/*---перечисление---*/
enum _flags {
    _READ   =    01,
    _WRITE  =    02,
    _UNBUF  =    04,
    _EOF    =    010,
    _ERR    =    020,
    _RDONLY =    0x01,
    _CREAT  =    0x02
};

/*---типы---*/
typedef long Align;
typedef union header Header;

union header {
    struct {
        union header *ptr;
        unsigned size;
    } s;
    Align x;
};

/*---functions---*/
char sbrk(int incr);
void fputc(char c, FILE *file);
void fputs(const char *str, FILE *file);
int _fillbuf(FILE *stream);
int _flushbuf(int c, FILE *stream);
int _getc(FILE *stream);
int _putc(char c, FILE *stream);
void *malloc(unsigned nbytes);

void *kmalloc(unsigned int size, int align, unsigned int *phys_addr);

void fprintf_unsigned(FILE *file, unsigned int num, int radix);
void fprintf_signed(FILE *file, long long num, int radix);
void fprintf_bu(FILE *file, const char* msg, const void* buf, uint32_t count);
void vfprintf(FILE *file, const char *fmt, va_list args);
void fprintf(FILE *file, const char *fmt, ...);
void printf(const char* fmt, ...);
void *free(void *ap);
char *fgets(char *s, int n, FILE * iop);
void fcopy(FILE *ifp, FILE *ofp);
int getline(char *line, int max);
int _fputs(char *s, FILE *iop);
FILE *fopen(char *name, char *mode);
int fclose(FILE *file);

/*---input-output---*/
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);
void prints(const char *fmt, unsigned int color, ...);
void kprint(const char *str, unsigned int color);
void kprintnl(void);

#ifdef __cplusplus
}
#endif

#endif
