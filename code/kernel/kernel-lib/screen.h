#ifndef SCREEN_H
#define SCREEN_H

#include "../libc/include/types.h"

#define LINES                   25
#define COLUMNS_IN_LINE         80
#define BYTES_FOR_EACH_ELEMENT  2
#define SCREENSIZE              (BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES)

#define REG_SCREEN_CTRL         0x3d4
#define REG_SCREEN_DATA         0x3d5

#define VGA_MEMORY_ADDR         0xB8000
#define VGA_DEFAULT_COLOR       0x0F

int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);
int get_cursor_offset(void);
void set_cursor_offset(int offset);

void console_init(void);
void console_clear(void);
void console_put_char(char c);
void console_write(const char * str);
void console_write_colored(const char * str, unsigned int color);
void console_set_color(unsigned int color);
unsigned int console_get_color(void);
void console_backspace(void);

int print_char(
    char c, 
    int col, 
    int row, 
    unsigned int color
);

void kprint(const char * fmt, uint32_t color, ...);
void kprintln(void);
void kprintb(void);
void clear_screen(void);
void delay(void);

#endif
