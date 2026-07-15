#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include "./types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void terminalInit(void);
void terminalPutChar(char c);
void terminalWrite(const char* data, unsigned int size);
void terminalWriteString(const char* data);

#ifdef __cplusplus
}
#endif

#endif
