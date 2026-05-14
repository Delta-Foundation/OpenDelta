/* вспомогательые функции */
#ifndef STDLIB_H
#define STDLIB_H

#pragma once

#include "./types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAXLINE 1000

/* переменные для использования вне функций */
uint64_t next = 1;
char line[MAXLINE];
char longest[MAXLINE];

int rand(void);
void srand(uint32_t seed);

int power(int base, int n);
int trim(char s[]);

void itoa(int n, char s[]);
void reverse(char s[]);
void swap(void *v[], int i, int j);
void copy(void);
void shellSort(int v[], int n);
void squeeze(char s[], int c);

int bitcount(unsigned x);
int binsearch(int x, int v[], int n);
unsigned getbits(unsigned x, int p, int n);

void qsort_internal(void* base,
            unsigned int num,
            unsigned int size,
            unsigned int left,
            unsigned int right,
            int (*compar)(const void*, const void*));

void qsort(void* base,
        unsigned int num,
        unsigned int size,
        int (*compar)(const void*, const void*));

#ifdef __cplusplus
}
#endif

#endif
