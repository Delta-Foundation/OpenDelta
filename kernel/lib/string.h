#ifndef STRING_H
#define STRING_H

#include "types.h"

typedef uint32_t _wchar_t;

int strindex(char s[], char t[]);

const char* strchar(const char* str, char ch);

int strcmp(const char *stra, const char *strb);
int strncmp(const char *cs, const char *ct, unsigned int n); 

unsigned int strlen(const char *str);
unsigned int strnlen(const char *str, unsigned int maxlen);

char *strcat(char *dest, const char *src);

char *strncpy(char *s1, const char *s2, unsigned int n);

void int_to_ascii(int n, char str[]);

void hex_to_ascii(int n, char str[]);

int sprintf(char *buf, const char *fmt);

_wchar_t* utf16_to_codepoint(_wchar_t* string, int* codepoint);
char* codepoint_to_utf8(int codepoint, char* string_out);

#endif
