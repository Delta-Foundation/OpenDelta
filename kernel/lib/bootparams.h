#ifndef BOOT_PARAMS_H
#define BOOT_PARAMS_H

#pragma once

#include "./types.h"

typedef struct {
    uint64_t begin, length;
    uint32_t type;
    uint32_t ACPI;
} mem_region_t;

typedef struct {
    int region_count;
    mem_region_t* regions;
} mem_info_t;

typedef struct {
    mem_info_t memory;
    uint8_t boot_device;
} boot_params_t;

#endif 
