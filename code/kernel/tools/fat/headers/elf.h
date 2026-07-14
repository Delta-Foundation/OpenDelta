#ifndef _TOOLS_ELF_H
#define _TOOLS_ELF_H

#pragma once

#include "../../../lib/types.h"
#include "./mbr.h"

#define ELF_MAGIC ("\x7F" "ELF")

typedef struct {
    uint8_t magic[4];
    uint8_t bitness;
    uint8_t endianness;
    uint8_t elf_header_version;
    uint8_t ABI;
    uint8_t _padding[8];
    uint16_t type;
    uint16_t instruction_set;
    uint32_t elf_version;
    uint64_t program_entry_position;
    uint32_t program_header_table_pos;
    uint32_t section_header_table_pos;
    uint32_t flags;
    uint16_t header_size;
    uint16_t program_header_table_entry_size;
    uint16_t program_header_table_entry_count;
    uint16_t section_header_table_size;
    uint16_t section_header_table_count;
    uint16_t section_names_index;
} __attribute__((packed)) elf_header;

typedef struct {
    uint32_t type;
    uint32_t offset;
    uint64_t virt_address;
    uint32_t phys_address;
    uint32_t file_size;
    uint32_t memory_size;
    uint32_t flags;
    uint32_t align;
} elf_program_header;

enum ELF_BITNESS {
    ELF_BITNESS_32BIT = 1,
    ELF_BITNESS_64BIT = 2,
};

enum ELF_ENDIANNESS {
    ELF_ENDIANNESS_LITTLE = 1,
    ELF_ENDIANNESS_BIG = 2,
};

enum ELF_INSTRUCTION_SET {
    ELF_INSTRUCTION_SET_NONE = 0,
    ELF_INSTRUCTION_SET_X86 = 3,
    ELF_INSTRUCTION_SET_ARM = 0x28,
    ELF_INSTRUCTION_SET_X64 = 0x3E,
    ELF_INSTRUCTION_SET_ARM64 = 0xB7,
    ELF_INSTRUCTION_SET_RISCV = 0xF3,
};

enum ELF_TYPE {
    ELF_TYPE_RELOCATABLE = 1,
    ELF_TYPE_EXECUTABLE = 2,
    ELF_TYPE_SHARED = 3,
    ELF_TYPE_CORE = 4,
};

enum ELFProgramType {
    ELF_PROGRAM_TYPE_NULL = 0,
    ELF_PROGRAM_TYPE_LOAD = 1,
    ELF_PROGRAM_TYPE_DYNAMIC = 2,
    ELF_PROGRAM_TYPE_INTERP = 3,
    ELF_PROGRAM_TYPE_NOTE = 4,
    ELF_PROGRAM_TYPE_SHLIB = 5,
    ELF_PROGRAM_TYPE_PHDR = 6,
    ELF_PROGRAM_TYPE_TLS = 7,
    ELF_PROGRAM_TYPE_LOOS = 0x60000000,
    ELF_PROGRAM_TYPE_HIOS = 0x6FFFFFFF,
    ELF_PROGRAM_TYPE_LOPROC = 0x70000000,
    ELF_PROGRAM_TYPE_HIPROC = 0x7FFFFFFF,
};

boolean elf_read(partition_t* part, const char* path, void** entry_point);

#endif 
