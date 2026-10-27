#include "../kernel-lib/ports.h"
#include "../kernel-lib/screen.h"
#include "../libc/include/stdbase.h"

/*
 * Simple VGA text console.
 *
 * The old implementation always started printing at (0, 0) and used 30x85
 * dimensions, so every new message overwrote the previous one.  This one
 * keeps track of the real hardware cursor, wraps at column 80 and scrolls
 * the screen when the last line is reached.
 */

static volatile uint16_t * const vga = (volatile uint16_t *) VGA_MEMORY_ADDR;

static int console_row = 0;
static int console_col = 0;
static unsigned int console_color = VGA_DEFAULT_COLOR;

static inline uint16_t vga_cell(char c, unsigned int color) {
    return (uint16_t)(uint8_t)c | (uint16_t)((color & 0xFF) << 8);
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
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xFF));
}

static void console_update_cursor(void) {
    if (console_row < 0) {
        console_row = 0;
    }

    if (console_row >= LINES) {
        console_row = LINES - 1;
    }

    if (console_col < 0) {
        console_col = 0;
    }

    if (console_col >= COLUMNS_IN_LINE) {
        console_col = COLUMNS_IN_LINE - 1;
    }

    set_cursor_offset(get_offset(console_col, console_row));
}

static void console_scroll(void) {
    for (int row = 1; row < LINES; row++) {
        for (int col = 0; col < COLUMNS_IN_LINE; col++) {
            vga[(row - 1) * COLUMNS_IN_LINE + col] =
                vga[row * COLUMNS_IN_LINE + col];
        }
    }

    for (int col = 0; col < COLUMNS_IN_LINE; col++) {
        vga[(LINES - 1) * COLUMNS_IN_LINE + col] =
            vga_cell(' ', console_color);
    }

    console_row = LINES - 1;
}

void console_set_color(unsigned int color) {
    console_color = color & 0xFF;
}

unsigned int console_get_color(void) {
    return console_color;
}

void console_clear(void) {
    for (int i = 0; i < LINES * COLUMNS_IN_LINE; i++) {
        vga[i] = vga_cell(' ', console_color);
    }

    console_row = 0;
    console_col = 0;
    console_update_cursor();
}

void console_init(void) {
    console_color = VGA_DEFAULT_COLOR;
    console_clear();
}

void console_put_char(char c) {
    switch (c) {
        case '\n':
            console_col = 0;
            console_row++;
            break;

        case '\r':
            console_col = 0;
            break;

        case '\b':
            if (console_col > 0) {
                console_col--;
            }
            else if (console_row > 0) {
                console_row--;
                console_col = COLUMNS_IN_LINE - 1;
            }
            vga[console_row * COLUMNS_IN_LINE + console_col] =
                vga_cell(' ', console_color);
            break;

        case '\t': {
            int spaces = 4 - (console_col % 4);
            for (int i = 0; i < spaces; i++) {
                console_put_char(' ');
            }
            return;
        }

        default:
            if ((unsigned char)c < 32) {
                return;
            }

            vga[console_row * COLUMNS_IN_LINE + console_col] =
                vga_cell(c, console_color);

            console_col++;
            if (console_col >= COLUMNS_IN_LINE) {
                console_col = 0;
                console_row++;
            }
            break;
    }

    if (console_row >= LINES) {
        console_scroll();
    }

    console_update_cursor();
}

void console_backspace(void) {
    console_put_char('\b');
}

void console_write(const char * str) {
    if (str == NULL) {
        return;
    }

    while (*str != '\0') {
        console_put_char(*str++);
    }
}

void console_write_colored(const char * str, unsigned int color) {
    unsigned int old = console_get_color();
    console_set_color(color);
    console_write(str);
    console_set_color(old);
}

/* Compatibility with the old screen API. */
int print_char(char c, int col, int row, unsigned int color) {
    if (col >= 0 && col < COLUMNS_IN_LINE && row >= 0 && row < LINES) {
        console_col = col;
        console_row = row;
    }

    unsigned int old = console_get_color();
    console_set_color(color);
    console_put_char(c);
    console_set_color(old);

    return get_offset(console_col, console_row);
}

void kprint(const char * fmt, uint32_t color, ...) {
    console_write_colored(fmt, color);
}

void kprintln(void) {
    console_put_char('\n');
}

void kprintb(void) {
    console_backspace();
}

void clear_screen(void) {
    console_clear();
}

void delay(void) {
    for (unsigned int n = 0; n < 50000000; n++) {
        __asm__ volatile ("nop");
    }
}
