#include "../lib/ports.h"
#include "../lib/stdbase.h"
#include "../mem/header/memory.h"
#include "../lib/screen.h"

int print_char(
    char c,
    int col, 
    int row, 
    Colors color
) {
    unsigned char * vidmem = (unsigned char *)0xb8000;

    if (col >= COLUMNS_IN_LINE || row >= LINES) {
        vidmem[2 * (COLUMNS_IN_LINE) * (LINES) - 2] = 'E';
        vidmem[2 * (COLUMNS_IN_LINE) * (LINES) - 1] = color;
        return get_offset(col, row);
    }
   
    int offset;
    if (col >= 0 && row >= 0) {
        offset = get_offset(col, row);
    }
    else {
        offset = get_cursor_offset();
    }

    if (c == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row + 1);
    }
    else if (c == 0x08) {
        vidmem[offset] = ' ';
        vidmem[offset + 1] = color;
    }
    else {
        vidmem[offset] = c;
        vidmem[offset + 1] = color;
        offset += 2;
    }

    if (offset >= LINES * COLUMNS_IN_LINE * 2) {
        int i;
        for (i = 1; i < LINES; i++) {
           memory_copy(
                (unsigned char*)(get_offset(0, i) + vidptr),
                (unsigned char*)(get_offset(0, i - 1) + vidptr),
                COLUMNS_IN_LINE * 2
            );
            
        }

        char *lastLine = (char *) (get_offset(0, LINES - 1) + (unsigned char*) vidptr);
        for (i = 0; i < COLUMNS_IN_LINE * 2; i++) {
            lastLine[i] = 0;
        }

        offset -= 2 * COLUMNS_IN_LINE;
    }

    set_cursor_offset(offset);
    return offset;
}

static void kprint_at(
    const char * fmt, 
    int col, 
    int row, 
    uint32_t color
) {
    int offset;
    if (col >= 0 && row >= 0) {
        offset = get_offset(col, row);
    }
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    int i = 0;
    while (fmt[i] != 0) {
        offset = print_char(fmt[i++], col, row, color);
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

void kprint(const char * fmt, uint32_t color, ...) {
    kprint_at(fmt, -1, -1, color);
}

void kprintln(void) {
    kprint_at("\n", -1, -1, 0xFF);
}

void kprintb(void) {
    int offset = get_cursor_offset() - 2;
    int row = get_offset_row(offset);
    int col = get_offset_col(offset);
    print_char(0x08, col, row, 0x0F);
}

void clear_screen(void)
{
    unsigned int i = 0;
    while (i < SCREENSIZE) {
        vidptr[i++] = ' ';
        vidptr[i++] = 0x07;
    }

    set_cursor_offset(get_offset(0, 0));
}

int get_offset(int col, int row) {
    return 2 * (row * COLUMNS_IN_LINE + col);
}

int get_offset_row(int offset) {
    return offset / (2 * COLUMNS_IN_LINE);
}

int get_offset_col(int offset) {
    return (offset - (get_offset_row(offset) * 2 * COLUMNS_IN_LINE)) / 2;
}

int get_cursor_offset(void) {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2;
}

void set_cursor_offset(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

void delay(void) {
    for (unsigned int n = 0; n < 500000000; n++) { ; }
}
