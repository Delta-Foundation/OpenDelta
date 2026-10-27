/* Headers from dltlibc */
#include "../../include/stdbase.h"
#include "../../include/types.h"
#include "../../include/string.h"
#include "../../include/ctype.h"
#include "../../include/stdarg.h"

/* Headers from kernel */
#include "../../../fs/headers/fs.h"
#include "../../../kernel-lib/system.h"
#include "../../../kernel-lib/screen.h"
#include "../../../kernel-lib/keyboard.h"

FILE _iob[OPEN_MAX];

FILE * fopen(char * name, char * mode) 
{
    signed int fd = 0;
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
        fd = sys_create(name, PERMS);
    }
    else if (*mode == 'a') {
        fd = sys_open(name, O_WRONLY, 0);
        if (fd == -1) {
            fd = sys_create(name, PERMS);
        }
        lseek(fd, 0L, 2);
    }
    else {
        fd = sys_open(name, O_RDONLY, 0);
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

int fclose(FILE * file) {
    if (!file) {
        return -1;
    }

    int ret = sys_close(file->fd);
    return ret;
}


char * fgets(
    char * s, 
    int n, 
    FILE * iop
) {
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

int fgetc(FILE * file) 
{
    if (file == NULL || (uintptr_t)file == (uintptr_t)stdin) {
        return console_getchar();
    }

    if (file->error) {
        return EOF;
    }   
    
    if (file->buf_pos >= file->buf_size) {
        if (!(file->flag & _READ)) {
            return EOF;
        }

        unsigned int bytes_read = sys_read(file->fd, (char *)file->buf, BUFSIZE);
        if ((int)bytes_read <= 0) {
            file->error = 1;
            return EOF;
        }

        file->buf_size = (unsigned int)bytes_read;
        file->buf_pos = 0;
    }

    unsigned char ch = (unsigned char)file->buf[file->buf_pos++];
    return (int)ch;
}

int fputs(
    const char * str,
    FILE * file
) {
    int c;

    while ((c = (unsigned char)*str++) != '\0') {
        fputc((char)c, file);
    }

    return ferror(file) ? EOF : 0;
}

void fcopy(FILE * ifp, FILE * ofp) {
    int c;

    while ((c = _getc(ifp)) != EOF) {
        _putc(c, ofp);
    }
}

void fputc(char c, FILE * file) {
    uintptr_t handle = (uintptr_t)file;

    if (handle == (uintptr_t)stdin) {
        return;
    }

    if (handle == (uintptr_t)stdout || handle == (uintptr_t)stderr) {
        console_put_char(c);
        return;
    }

    if (handle == (uintptr_t)debug) {
        outb(0xE9, (uint8_t)c);
        return;
    }

    if (file != NULL) {
        sys_write(file->fd, &c, 1);
    }
}
