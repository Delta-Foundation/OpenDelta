#ifndef SCREEN_H
#define SCREEN_H

int printChar(char c, int col, int row, unsigned int color);
void kprint_at(const char *message, int col, int row, unsigned int color);
void kprint(const char *message, unsigned int color);
void kprintnl(void);
void kprintb(void);
void clearScreen(void);
void delay(void);

#endif
