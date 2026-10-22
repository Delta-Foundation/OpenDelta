#ifndef MEMORY_H
#define MEMORY_H

#pragma once

#include "../../libc/include/types.h"
#include "../../libc/include/stdbase.h"
#include "../../kernel-lib/system.h"
#include "../../kernel-lib/bootparams.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define INDEX_FROM_BIT(b) (b / 0x20)
#define OFFSET_FROM_BIT(b) (b % 0x20)
#define MAX_REGIONS 256

#define MEMORY_MIN          0x00000500
#define MEMORY_MAX          0x00080000

#define MEMORY_FAT_ADDR     ((void*)0x20000)
#define MEMORY_FAT_SIZE     0x10000

#define MEMORY_ELF_ADDR     ((void*)0x30000)
#define MEMORY_ELF_SIZE     0x00010000

#define MEMORY_LOAD_KERNEL  ((void*)0x40000)
#define MEMORY_LOAD_SIZE    0x00010000

#define MEMORY_KERNEL_ADDR  ((void*)0x100000)

void append(char s[], char n);
void backspace(char s[]);
void flush(char *var);

int memcmp(
    const void * ptr1,
    const void * ptr2, 
    uint16_t num
);

void * memcpy(
    void * dest,
    const void * src,
    size_t n
);

void * memset(
    void * str,
    int c,
    size_t n
);

void memory_copy(
    unsigned char * source, 
    unsigned char * dest,
    int nbytes
);

void memory_set(
    uint8_t * dest, 
    uint8_t val, 
    uint32_t len
);

void memdetect(mem_info_t * mem_info);
void * segoffset_to_linear(void * addr);

void pagging_install(uint32_t memsize);
void heap_install(void);
void switch_page_dir(page_dir_t *dir);

#ifdef __cplusplus 
}
#endif

#endif 
