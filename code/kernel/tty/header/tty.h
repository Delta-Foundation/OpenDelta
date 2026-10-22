#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include "../../libc/include/types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void terminal_init(void);
void terminal_put_char(char c);

void terminal_write(
    const char * data,
    unsigned int size
);

void terminal_write_string(const char * data);

#ifdef __cplusplus
}
#endif

#endif
