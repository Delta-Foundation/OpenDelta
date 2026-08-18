# Стиль кода.

## Основные правила:
  * стиль наименования: pascal_case (в редком случае camelCase)
  * 4 пробела или 1 таб.
  * Максимальная длина функции: ~= 100/150 строк.
  * Оптимальная длина функции: ~= 25/30 строк.
  * Максимальаня длина файла: ~= 500/700 строк.
  * Оптимальная длина файла: ~= 50/70 строк.
  * Максимальное количество аргументов в функции: 7.
  * Оптимальное количество аргументов в функции: от 0 до 3.

-------------------------------------
-------------------------------------

## Примеры кода.
### C. Структуры, перечисления, объединения.

* #### Структуры:

``` c
/* Первый вариант */
typedef struct {
    uint16_t low_offset;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t high_offset;
} idt_gate_t;

typedef struct {
    uint16_t limit;
    idt_gate_t* base;
} idt_reg_t; 

/* Второй вариант */
struct timeval {
    uint32_t tv_sec;
    uint32_t tv_usec;
};
```

* #### Перечисления:

``` c
/* Первый вариант */
typedef enum {
    GDT_FLAGS_64BIT =    0x20,
    GDT_FLAG_32BIT  =    0x40,
    GDT_FLAG_16BIT  =    0x00,

    GDT_FLAG_GRANULARITY_1B = 0x00,
    GDT_FLAG_GRANULARITY_4K = 0x80,
} GDT_FLAGS;

/* Второй вариант */
enum ELF_INSTRUCTION_SET {
    ELF_INSTRUCTION_SET_NONE = 0,
    ELF_INSTRUCTION_SET_X86 = 3,
    ELF_INSTRUCTION_SET_ARM = 0x28,
    ELF_INSTRUCTION_SET_X64 = 0x3E,
    ELF_INSTRUCTION_SET_ARM64 = 0xB7,
    ELF_INSTRUCTION_SET_RISCV = 0xF3,
};

```

* #### Объединения:

``` c 
union header {
    struct {
        union header *ptr;
        unsigned size;
    } s;
    Align x;
};

/* или */

union soviet {
    char russia;
    char belarus;
    char ukraine;
    char kazakhstan; // и другие республики
};
```

### C. Расстоновка макросов и импортов.

#### Подключение стандартных библиотек. Все необязательно подключать. Но обязательно именно в таком порядке.

``` c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/unistd.h>
#include <sys/types.h>
```

#### Порядок расстоновки макросов . 

``` c
// условия для header файла
#ifndef HEADER_H
#define HEADER_H

#pragma once // или хз чё там 

// числовая константа
#define NIRRER 1

// проверка условий с константами
#if !defined(NIRRER)
    #define NIRRER 0
#else
    #define NIRRER 1
#endif 

// строковая константа
#define NIRRER_NAME "jumba"

// функция из define
#define spctabnl(c)	((c) == ' ' || (c) == '\t' || (c) == '\n')

#endif 
```
