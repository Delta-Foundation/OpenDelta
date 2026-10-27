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

const char g_hex_chars[] = "0123456789abcdef";

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a" (val), "Nd" (port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

static void print_padding(FILE * file, int count, char pad) {
    while (count-- > 0) {
        fputc(pad, file);
    }
}

static void print_unsigned_number(
    FILE * file,
    unsigned long long value,
    int radix,
    int uppercase,
    int width,
    int zero_pad,
    int left_align,
    int alt_prefix,
    int negative,
    int show_plus,
    int show_space,
    int precision
) {
    char buf[65];
    int pos = 0;
    const char * digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if (value == 0 && precision != 0) {
        buf[pos++] = '0';
    }

    while (value > 0) {
        buf[pos++] = digits[value % (unsigned int)radix];
        value /= (unsigned int)radix;
    }

    while (pos < precision) {
        buf[pos++] = '0';
    }

    char sign = negative ? '-' : (show_plus ? '+' : (show_space ? ' ' : '\0'));
    int prefix_len = (alt_prefix && radix == 16) ? 2 : 0;
    int len = pos + (sign ? 1 : 0) + prefix_len;

    if (!left_align && !zero_pad) {
        print_padding(file, width - len, ' ');
    }

    if (sign) {
        fputc(sign, file);
    }

    if (prefix_len) {
        fputc('0', file);
        fputc(uppercase ? 'X' : 'x', file);
    }

    if (!left_align && zero_pad) {
        print_padding(file, width - len, '0');
    }

    while (pos > 0) {
        fputc(buf[--pos], file);
    }

    if (left_align) {
        print_padding(file, width - len, ' ');
    }
}

void fprintf_unsigned(
    FILE * file,
    unsigned int num,
    int radix
) {
    print_unsigned_number(file, num, radix, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void fprintf_signed(
    FILE * file,
    long long num,
    int radix
) {
    int negative = num < 0;
    unsigned long long value = negative ? (unsigned long long)(-num) : (unsigned long long)num;
    print_unsigned_number(file, value, radix, 0, 0, 0, 0, 0, negative, 0, 0, 0);
}

void prints(
    const char * fmt,
    Colors color,
    ...
) {
    unsigned int old_color = console_get_color();
    console_set_color((unsigned int)color);

    va_list args;
    va_start(args, color);
    vfprintf((FILE *)stdout, fmt, args);
    va_end(args);

    console_set_color(old_color);
}

void vfprintf(
    FILE * file,
    const char * fmt,
    va_list args
) {
    while (*fmt != '\0') {
        if (*fmt != '%') {
            fputc(*fmt++, file);
            continue;
        }

        fmt++;

        if (*fmt == '%') {
            fputc('%', file);
            fmt++;
            continue;
        }

        int left_align = 0;
        int zero_pad = 0;
        int show_plus = 0;
        int show_space = 0;
        int alt_prefix = 0;

        for (;;) {
            if (*fmt == '-') {
                left_align = 1;
            }
            else if (*fmt == '0') {
                zero_pad = 1;
            }
            else if (*fmt == '+') {
                show_plus = 1;
            }
            else if (*fmt == ' ') {
                show_space = 1;
            }
            else if (*fmt == '#') {
                alt_prefix = 1;
            }
            else {
                break;
            }

            fmt++;
        }

        int width = 0;
        if (*fmt == '*') {
            width = va_arg(args, int);
            if (width < 0) {
                left_align = 1;
                width = -width;
            }
            fmt++;
        }
        else {
            while (isdigit((unsigned char)*fmt)) {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }
        }

        int precision = -1;
        if (*fmt == '.') {
            fmt++;
            precision = 0;

            if (*fmt == '*') {
                precision = va_arg(args, int);
                fmt++;
            }
            else {
                while (isdigit((unsigned char)*fmt)) {
                    precision = precision * 10 + (*fmt - '0');
                    fmt++;
                }
            }
        }

        enum {
            LEN_DEFAULT,
            LEN_CHAR,
            LEN_SHORT,
            LEN_LONG,
            LEN_LONG_LONG,
            LEN_SIZE
        } length = LEN_DEFAULT;

        for (;;) {
            if (*fmt == 'h') {
                length = (length == LEN_SHORT) ? LEN_CHAR : LEN_SHORT;
                fmt++;
            }
            else if (*fmt == 'l') {
                length = (length == LEN_LONG) ? LEN_LONG_LONG : LEN_LONG;
                fmt++;
            }
            else if (*fmt == 'z' || *fmt == 't' || *fmt == 'j') {
                length = LEN_SIZE;
                fmt++;
            }
            else {
                break;
            }
        }

        char spec = *fmt;
        if (spec != '\0') {
            fmt++;
        }

        switch (spec) {
            case 'c': {
                char c = (char)va_arg(args, int);
                if (!left_align) {
                    print_padding(file, width - 1, ' ');
                }
                fputc(c, file);
                if (left_align) {
                    print_padding(file, width - 1, ' ');
                }
                break;
            }

            case 's': {
                const char * str = va_arg(args, const char *);
                if (str == NULL) {
                    str = "(null)";
                }

                int len = (int)strlen(str);
                if (precision >= 0 && len > precision) {
                    len = precision;
                }

                if (!left_align) {
                    print_padding(file, width - len, ' ');
                }

                for (int i = 0; i < len; i++) {
                    fputc(str[i], file);
                }

                if (left_align) {
                    print_padding(file, width - len, ' ');
                }
                break;
            }

            case 'd':
            case 'i': {
                long long value;

                switch (length) {
                    case LEN_CHAR:      value = (signed char)va_arg(args, int); break;
                    case LEN_SHORT:     value = (short)va_arg(args, int); break;
                    case LEN_LONG:      value = va_arg(args, long); break;
                    case LEN_LONG_LONG: value = va_arg(args, long long); break;
                    case LEN_SIZE:      value = (long long)va_arg(args, size_t); break;
                    default:            value = va_arg(args, int); break;
                }

                int negative = value < 0;
                unsigned long long abs_value = negative
                    ? (unsigned long long)(-(value + 1)) + 1ULL
                    : (unsigned long long)value;

                print_unsigned_number(
                    file, abs_value, 10, 0, width, zero_pad, left_align,
                    0, negative, show_plus, show_space, precision < 0 ? 1 : precision
                );
                break;
            }

            case 'u':
            case 'o':
            case 'x':
            case 'X':
            case 'p': {
                unsigned long long value;

                if (spec == 'p') {
                    value = (unsigned long long)(uintptr_t)va_arg(args, void *);
                    alt_prefix = 1;
                }
                else {
                    switch (length) {
                        case LEN_CHAR:      value = (unsigned char)va_arg(args, unsigned int); break;
                        case LEN_SHORT:     value = (unsigned short)va_arg(args, unsigned int); break;
                        case LEN_LONG:      value = va_arg(args, unsigned long); break;
                        case LEN_LONG_LONG: value = va_arg(args, unsigned long long); break;
                        case LEN_SIZE:      value = va_arg(args, size_t); break;
                        default:            value = va_arg(args, unsigned int); break;
                    }
                }

                int radix = (spec == 'o') ? 8 : ((spec == 'u') ? 10 : 16);
                print_unsigned_number(
                    file, value, radix, spec == 'X', width, zero_pad, left_align,
                    alt_prefix, 0, 0, 0, precision < 0 ? 1 : precision
                );
                break;
            }

            default:
                fputc('%', file);
                if (spec != '\0') {
                    fputc(spec, file);
                }
                break;
        }
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
    for (uint32_t i = 0; i < count; i++) {
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

void skip_whitespace(FILE * stream) {
    int c;

    while ((c = _getc(stream)) != EOF && isspace((unsigned char)c)) {
        ;
    }

    if (c != EOF) {
        ungetc(c, stream);
    }
}

static int read_number(
    FILE * stream,
    int width,
    int radix,
    unsigned long long * value_out,
    int * negative_out
) {
    int c;
    int digits = 0;
    int negative = 0;
    unsigned long long value = 0;

    skip_whitespace(stream);

    c = ngetc(stream);
    if (c == '-') {
        negative = 1;
        c = ngetc(stream);
    }
    else if (c == '+') {
        c = ngetc(stream);
    }

    if (radix == 16 && c == '0') {
        int next = ngetc(stream);
        if (next == 'x' || next == 'X') {
            c = ngetc(stream);
        }
        else {
            ungetc(next, stream);
            value = 0;
            digits = 1;
            c = ngetc(stream);
        }
    }

    while (c != EOF && (width == 0 || digits < width)) {
        int digit;

        if (c >= '0' && c <= '9') {
            digit = c - '0';
        }
        else if (c >= 'a' && c <= 'f') {
            digit = c - 'a' + 10;
        }
        else if (c >= 'A' && c <= 'F') {
            digit = c - 'A' + 10;
        }
        else {
            break;
        }

        if (digit >= radix) {
            break;
        }

        value = value * (unsigned int)radix + (unsigned int)digit;
        digits++;
        c = ngetc(stream);
    }

    if (c != EOF) {
        ungetc(c, stream);
    }

    if (digits == 0) {
        return 0;
    }

    *value_out = value;
    *negative_out = negative;
    return 1;
}

int scanf(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int count = 0;
    int matched = 0;
    FILE * stream = (FILE *)stdin;

    while (*fmt != '\0') {
        if (isspace((unsigned char)*fmt)) {
            skip_whitespace(stream);
            fmt++;
            continue;
        }

        if (*fmt != '%') {
            int c = ngetc(stream);
            if (c != *fmt) {
                if (c != EOF) {
                    ungetc(c, stream);
                }
                break;
            }
            fmt++;
            matched++;
            continue;
        }

        fmt++;

        if (*fmt == '%') {
            int c = ngetc(stream);
            if (c != '%') {
                if (c != EOF) {
                    ungetc(c, stream);
                }
                break;
            }
            fmt++;
            matched++;
            continue;
        }

        int suppress = 0;
        if (*fmt == '*') {
            suppress = 1;
            fmt++;
        }

        int width = 0;
        while (isdigit((unsigned char)*fmt)) {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }

        int length = 0;
        while (*fmt == 'h' || *fmt == 'l' || *fmt == 'z' || *fmt == 't') {
            if (*fmt == 'l') {
                length++;
            }
            fmt++;
        }

        char type = *fmt;
        if (type != '\0') {
            fmt++;
        }

        switch (type) {
            case 'c': {
                int c = ngetc(stream);
                if (c == EOF) {
                    goto done;
                }
                if (!suppress) {
                    char * ch = va_arg(args, char *);
                    *ch = (char)c;
                    count++;
                }
                matched++;
                break;
            }

            case 's': {
                char * str = suppress ? NULL : va_arg(args, char *);
                int i = 0;
                int c;

                skip_whitespace(stream);

                while ((c = ngetc(stream)) != EOF && !isspace((unsigned char)c)
                       && (width == 0 || i < width)) {
                    if (str) {
                        str[i] = (char)c;
                    }
                    i++;
                }

                if (c != EOF) {
                    ungetc(c, stream);
                }

                if (i == 0) {
                    goto done;
                }

                if (str) {
                    str[i] = '\0';
                    count++;
                }
                matched++;
                break;
            }

            case '[': {
                int scanset[256] = {0};
                int negate = 0;
                int first = 1;

                if (*fmt == '^') {
                    negate = 1;
                    fmt++;
                }

                while (*fmt != '\0' && (*fmt != ']' || first)) {
                    if (*fmt == '-' && !first && fmt[1] != ']' && fmt[1] != '\0') {
                        unsigned char start = (unsigned char)fmt[-1];
                        unsigned char end = (unsigned char)fmt[1];
                        for (unsigned int c = start; c <= end; c++) {
                            scanset[c] = 1;
                        }
                        fmt += 2;
                    }
                    else {
                        scanset[(unsigned char)*fmt] = 1;
                        fmt++;
                    }
                    first = 0;
                }

                if (*fmt == ']') {
                    fmt++;
                }

                char * str = suppress ? NULL : va_arg(args, char *);
                int i = 0;
                int c;

                while ((c = ngetc(stream)) != EOF && (width == 0 || i < width)) {
                    int in_set = (c >= 0 && c <= 255) ? scanset[(unsigned char)c] : 0;
                    if (negate) {
                        in_set = !in_set;
                    }
                    if (!in_set) {
                        break;
                    }
                    if (str) {
                        str[i] = (char)c;
                    }
                    i++;
                }

                if (c != EOF) {
                    ungetc(c, stream);
                }

                if (i == 0) {
                    goto done;
                }

                if (str) {
                    str[i] = '\0';
                    count++;
                }
                matched++;
                break;
            }

            case 'd':
            case 'i':
            case 'u':
            case 'o':
            case 'x':
            case 'X': {
                int radix = 10;
                if (type == 'o') {
                    radix = 8;
                }
                else if (type == 'x' || type == 'X') {
                    radix = 16;
                }

                unsigned long long value = 0;
                int negative = 0;

                if (!read_number(stream, width, radix, &value, &negative)) {
                    goto done;
                }

                if (!suppress) {
                    if (type == 'd' || type == 'i') {
                        long long signed_value = negative
                            ? -(long long)value
                            : (long long)value;

                        switch (length) {
                            case 0: {
                                int * out = va_arg(args, int *);
                                *out = (int)signed_value;
                                break;
                            }
                            case 1: {
                                long * out = va_arg(args, long *);
                                *out = (long)signed_value;
                                break;
                            }
                            default: {
                                long long * out = va_arg(args, long long *);
                                *out = signed_value;
                                break;
                            }
                        }
                    }
                    else {
                        switch (length) {
                            case 0: {
                                unsigned int * out = va_arg(args, unsigned int *);
                                *out = (unsigned int)value;
                                break;
                            }
                            case 1: {
                                unsigned long * out = va_arg(args, unsigned long *);
                                *out = (unsigned long)value;
                                break;
                            }
                            default: {
                                unsigned long long * out = va_arg(args, unsigned long long *);
                                *out = value;
                                break;
                            }
                        }
                    }
                    count++;
                }
                matched++;
                break;
            }

            default:
                goto done;
        }
    }

done:
    va_end(args);

    if (count > 0) {
        return count;
    }

    return matched > 0 ? 0 : EOF;
}
