#ifndef SCREEN_H
#define SCREEN_H

#include "./types.h"

#define LINES 				    30
#define COLUMNS_IN_LINE 		85
#define BYTES_FOR_EACH_ELEMENT 	2
#define SCREENSIZE 			    BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES
#define REG_SCREEN_CTRL			0x3d4
#define REG_SCREEN_DATA			0x3d5

static unsigned int currentLoc = 0;
static char *vidptr = (char*)0xb8000;

int printChar(char c, int col, int row, unsigned int color);
void kprint_at(const char *fmt, int col, int row, uint32_t color); 
void kprint(const char *fmt, uint32_t color, ...);
void kprintln(void);
void kprintb(void);
void clearScreen(void);
void delay(void);

#endif
