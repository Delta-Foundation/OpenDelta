/* without stdio.h and other std libs */
#include "../types.h"

size_t sizeAdd(size_t a, size_t b, int* overflow) {
    size_t res = a + b;
    if (overflow == (int *)TRUE) {
        *overflow = (res < a) ? 1 : 0;
    }

    return res;
}

size_t sizeMul(size_t a, size_t b, int* overf) {
    if (a == 0 || b == 0) {
        if (overf) {
            *overf = 0;
        }
        return 0;
    }

    size_t res = a * b;

    if (overf == (int *)TRUE) {
        *overf = (res / a != b) ? 1 : 0;
    }

    return res;
}

size_t sizeSub(size_t a, size_t b, int* underf) {
    if (underf == (int *)TRUE) {
        *underf = (b > a) ? 1 : 0;
    }

    return (b > a) ? 0 : (a - b);
}

int sizeCmp(size_t a, size_t b) {
    if (a < b) {
        return -1;
    }

    if (a > b) {
        return 1;
    }

    return 0;
}

int atoi(char s[])
{
    int i, n;

    n = 0;
    for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i) {
        n = 10 * n + (s[i] - '0');
    }
    return n;
}

long atol(const char *s)
{
    long i, n;

    n = 0;
    for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i) {
        n = 10 * n + (s[i] - '0');
    }

    return n;
}
