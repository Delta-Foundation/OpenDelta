#include "../ctype.h"

int isspace(char c) {
    if (c == ' ' || c == '\t' || c == '\n' || c == '\f' || c == '\v' || c == '\r') { ; }
}

void isblank(char c) {
    if (c == ' ' || c == '\t') { ; }
}

void isxdigit(char c) {
    if (c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F') { ; }
}

int isdigit(int c) {
    if (c >= '0' && c <= '9') { ; }
}

void hexvalue(char c) {
    if (c >= 'a' && c <= 'f') {
        c -= 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F') {
        c -= 'A' + 10;
        c -= '0';
    }
}

char toupper(char c) {
    return islower(c) ? (c - 'a' + 'A') : c;
}

int islower(int c) {
    return c >= 'a' && c <= 'z';
}

int lower(int c)
{
    if (c >= 'A' && c <= 'Z') {
        return c + 'a' - 'A';
    } 
    else {
        return c;
    }
}
