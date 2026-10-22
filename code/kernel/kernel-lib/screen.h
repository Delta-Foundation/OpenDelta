#ifndef SCREEN_H
#define SCREEN_H

#include "../libc/include/types.h"

#define LINES 				    30
#define COLUMNS_IN_LINE 		85
#define BYTES_FOR_EACH_ELEMENT 	2
#define SCREENSIZE 			    BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES
#define REG_SCREEN_CTRL			0x3d4
#define REG_SCREEN_DATA			0x3d5

static unsigned int currentLoc = 0;
static char * vidptr = (char *)0xb8000;

int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);
int get_cursor_offset(void);
void set_cursor_offset(int offset);

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
