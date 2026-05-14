#include "../string.h"
#include "../stdlib.h"

#define MAXLINE 1000

char line[MAXLINE];
char longest[MAXLINE];

int rand(void)
{
    next = next * 1103515245 + 12345;
    return (uint32_t) (next / 65536) % 32768;
}

int trim(char s[])
{
    int n;

    for (n = strlen(s) - 1; n >= 0; n--) {
        if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n') {
            break;
        }
    }

    s[n + 1] = '\0';
    return n;
}

int power(int base, int n)
{
    int i, p;

    p = 1;
    for (i = 1; i <= n; ++i) {
        p = p * base;
    }
    return p;
}

int bitcount(unsigned x) 
{
    int b;

    for (b = 0; x != 0; x >>= 1) {
        if (x & 01) {
            b++;
        }
    }

    return b;
}

int binsearch(int x, int v[], int n)
{
    int low, high, mid;

    low = 0;
    high = n - 1;

    while (low <= high) {
        mid = (low + high) / 2;
        if (x < v[mid]) {
            high = mid - 1;
        }
        else if (x > v[mid]) {
            low = mid + 1;
        }
        else {
            return mid;
        }
    }

    return -1;
}

void copy(void)
{
    int i;
    extern char line[], longest[];

    i = 0;
    while ((longest[i] = line[i]) != '\0') {
        ++i;
    }
}

void shellSort(int v[], int n)
{
    int gap, i, j, temp;

    for (gap = n/2; gap > 0; gap /= 2) {
        for (i = gap; i < n; i++) {
            for (j=i - gap; j >= 0 && v[j] > v[j + gap]; j -= gap) {
                temp = v[j];
                v[j] = v[j + gap];
                v[j + gap] = temp;
            }
        }
    }
}

void qsort_internal(void* base,
            unsigned int num,
            unsigned int size,
            unsigned int left,
            unsigned int right,
            int (*compar)(const void*, const void*))
{
    if (left >= right) {
        return;
    }

    int i = left, j = right;
    void* pivot = base + (i * size);
    uint8_t temp;

    for (;;) {
        while ((*compar)(base + (i * size), pivot) < 0) { i++; }
        while ((*compar)(pivot, base + (j * size)) < 0) { j--; }
        if (i >= j) {
            break;
        }

        for (int k = 0; k < size; k++) {
            temp = *((uint8_t*)(base + (i * size)) + k);
            *((uint8_t*)(base + (j * size)) + k) = *((uint8_t*)(base + (j * size)) + k);  
            *((uint8_t*)(base + (j * size)) + k) = temp;
        }

        i++;
        j--;
    }
}

void qsort(void* base,
        unsigned int num,
        unsigned int size,
        int (*compar)(const void*, const void*)) {
    qsort_internal(base, num, size, 0, num - 1, compar);
}

void swap(void *v[], int i, int j)
{
    void *temp;
    
    temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

void itoa(int n, char s[]) 
{
    int i, sign;

    if ((sign = n) < 0) {
        n = -n;
    }

    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) {
        s[i++] = '-';
    }
    s[i] = '\0';
    reverse(s);
}

void reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void srand(uint32_t seed) 
{
    next = seed;
}

void squeeze(char s[], int c) 
{
    int i, j;

    for (i = j = 0; s[i] != '\0'; i++) {
        if (s[i] != c) {
            s[j] = s[i];
            j++;
        }
    }

    s[j] = '\0';
}

unsigned getbits(unsigned x, int p, int n) 
{
    return (x >> (p+1-n)) & ~ (~0 << n);
} 
