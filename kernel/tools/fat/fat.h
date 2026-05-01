#ifndef TOOLS_FAT_H
#define TOOLS_FAT_H

#include "../../lib/stdbase.h"
#include "../../lib/stdlib.h"
#include "../../lib/string.h" 
#include "../../lib/types.h"
#include "../../lib/ctype.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

#define SECTOR_SIZE         512
#define MAX_PATH_SIZE       256
#define MAX_FILE_HANDLES    10
#define ROOT_DIR_HANDLE     -1
#define MEMORY_FAT_SIZE     0x10000

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) / (a) : (b))

#pragma pack(push, 1)

typedef struct {
    uint8_t boot_jumps_instruct[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sect;
    uint8_t sects_per_cluster;
    uint16_t reversed_sects;
    uint8_t fat_count;
    uint16_t total_sects;
    uint8_t media_desc_type;
    uint16_t sects_per_fat;
    uint16_t sects_per_track;
    uint16_t heads;
    uint32_t hidden_sects;
    uint32_t large_sects_count;

    uint8_t drive_num;
    uint8_t _reversed;
    uint8_t signature;
    uint32_t vol_id;
    uint8_t vol_label[11];
    uint8_t sys_id[8];
} __attribute__((packed)) boot_sect_t;

typedef struct {
    uint8_t name[11];
    uint8_t attributes;
    uint8_t _reversed;
    uint8_t created_time_tenths;
    uint16_t created_time;
    uint16_t created_date;
    uint16_t accessed_date;
    uint16_t first_cluster_high;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t first_cluster_low;
    uint32_t size;
} __attribute__((packed)) dir_entry_t;

#pragma pack(pop)

typedef struct {
    int handle;
    boolean is_dir;
    uint32_t position;
    uint32_t size;
} fat_file_t;

enum FAT_Attributes {
    FAT_ATTR_READ_ONLY = 0x01,
    FAT_ATTR_HIDDEN = 0x02,
    FAT_ATTR_SYSTEM = 0x04,
    FAT_ATTR_VOLUME_ID = 0x08,
    FAT_ATTR_DIR = 0x10,
    FAT_ATTR_ARCHIVE = 0x20,
    FAT_ATTR_LEN = FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID
};

typedef struct {
    fat_file_t fspublic;
    boolean opened;
    uint32_t first_cluster;
    uint32_t current_cluster;
    uint32_t current_sector_in_cluster;
    uint8_t buffer[SECTOR_SIZE];
} fat_file_data_t;

typedef struct {
    
    union {
        boot_sect_t boot_sect;
        uint8_t boot_sect_bytes[SECTOR_SIZE];
    } BS;

    fat_file_data_t root_dir;
    fat_file_data_t opened_files[MAX_FILE_HANDLES];

} fat_data_t;

#ifdef __cplusplus
}
#endif

#endif
