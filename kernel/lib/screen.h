#ifndef SCREEN_H
#define SCREEN_H

#include "./types.h"

int printChar(char c, int col, int row, unsigned int color);
void kprint_at(const char *fmt, int col, int row, uint32_t color); 
void kprint(const char *fmt, uint32_t color, ...);
void kprintln(void);
void kprintb(void);
void clearScreen(void);
void delay(void);

#endif
