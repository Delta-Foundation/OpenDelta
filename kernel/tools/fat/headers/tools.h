#ifndef TOOLS_H
#define TOOLS_H

#pragma once

#include "../../../lib/types.h"

#define ASMCALL __attribute__((cdecl))

typedef struct {
    uint64_t base;
    uint64_t len;
    uint32_t type;
    uint32_t ACPI;
} e820_mem_block;

enum E820_MEM_BLOCK_TYPE {
    E820_USABLE = 1,
    E820_REVERSED = 2,
    E820_ACPI_RECLAIMABLE = 3,
    E820_ACPI_NVS = 4,
    E820_BAD_MEM = 5,
}; 

void ASMCALL disk_get_drive_params(uint8_t drive,
        uint8_t* drive_type_out,
        uint16_t* cylindres_out,
        uint16_t* sectors_out,
        uint16_t* heads_out);

boolean ASMCALL disk_reset(uint8_t drive);
boolean ASMCALL disk_read(uint8_t drive,
        uint16_t cylinder,
        uint16_t sector,
        uint16_t head,
        uint8_t count,
        void* lower_data_out);

int ASMCALL get_next_block(e820_mem_block* block, uint32_t* continuation_id);

#endif 
