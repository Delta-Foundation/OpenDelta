#ifndef CTYPE_H
#define CTYPE_H

#include "stdbase.h"
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

char toupper(char c);
int islower(int c);
int lower(int c);

int isspace(char c);
void isxdigit(char c);
int isdigit(int c);
void isblank(char c);
void sixdigit(char c);
void hexvalue(char c);

#ifdef __cplusplus
}
#endif 

#endif 
