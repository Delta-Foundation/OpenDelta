#include "../libc/include/types.h"
#include "../libc/include/string.h"
#include "./header/tty.h"
#include "../kernel-lib/vga.h"

static uint32_t * const VGA_MEM = (uint32_t*) 0xB8000;

static unsigned int terminal_row;
static unsigned int terminal_col;
static unsigned int terminal_color;
static unsigned int * terminal_buf;

void terminal_init(void) 
{
    terminal_row = 0;
    terminal_col = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buf = VGA_MEM;

    for (unsigned int y = 0; y < VGA_HEIGHT; y++) {
        for (unsigned int x = 0; x < VGA_WIDTH; x++) {
            const unsigned int index = y * VGA_WIDTH + x;
            terminal_buf[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_set_color(uint8_t color) {
    terminal_color = color;
}

static void terminal_put_entry_at(
    unsigned char c, 
    uint8_t color,
    unsigned int x,
    unsigned int y
) {
    const unsigned int index = y * VGA_WIDTH + x;
    terminal_buf[index] = vga_entry(c, color);
}

void terminal_put_char(char c)
{
    unsigned char uc = c;
    
    terminal_put_entry_at(
        uc, 
        terminal_color,
        terminal_col,
        terminal_row
    );
    
    if (++terminal_col == VGA_WIDTH) {
        terminal_col = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void terminal_write(
    const char * data, 
    unsigned int size
) {
    for (unsigned int i = 0; i < size; i++) {
        terminal_put_char(data[i]);
    }
}

void terminal_write_string(const char * data) {
    terminal_write(data, strlen(data));
}

