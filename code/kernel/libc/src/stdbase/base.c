/* Headers from dltlibc */
#include "../../include/stdbase.h"
#include "../../include/types.h"
#include "../../include/string.h"
#include "../../include/ctype.h"
#include "../../include/stdarg.h"

/* Headers from kernel */
#include "../../../fs/headers/fs.h"
#include "../../../kernel-lib/system.h"

ungetc_buf_t ungetc_buf = { EOF, 0, NULL };

int _getc(FILE * stream) {
    if (--(stream)->cnt >= 0) {
        return (unsigned char) *(stream)->ptr;
    }
    else {
        return _fillbuf(stream);
    }
}

int ngetc(FILE * stream) {
    if (ungetc_buf.has_buf && ungetc_buf.stream == stream) {
        ungetc_buf.has_buf = 0;
        return ungetc_buf.buf;
    }

    return fgetc(stream);
}

int _putc(char c, FILE * stream) {
    if (--(stream)->cnt >= 0) {
        return *(stream)->ptr++ = (c);
    }
    else {
        return  _flushbuf((c), stream);
    }
}

int getline(char * line, int max)
{
    if (fgets(line, max, stdin) == NULL) {
        return 0;
    }
    else {
        return strlen(line);
    }
}

int mode_getline(char s[], int lim) {
    int c, i;

    i = 0;
    while (--lim > 0 && (c=getchar()) != EOF && c != '\n') {
        s[i++] = c;
    }

    if (c == '\n') {
        s[i++] = c;
    }

    s[i] = '\0';
    return i;
}

int ungetc(int c, FILE * stream) 
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

    return 0;
}
