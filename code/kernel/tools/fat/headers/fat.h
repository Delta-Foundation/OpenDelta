#ifndef _TOOLS_FAT_H
#define _TOOLS_FAT_H

#include "../../../lib/stdbase.h"
#include "../../../lib/stdlib.h"
#include "../../../lib/string.h" 
#include "../../../lib/types.h"
#include "../../../lib/ctype.h"
#include "./mbr.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

#define SECTOR_SIZE         512
#define MAX_PATH_SIZE       256
#define MAX_FILE_HANDLES    10
#define ROOT_DIR_HANDLE     -1
#define MEMORY_FAT_SIZE     0x10000
#define FAT_CACHE_SIZE      5
#define FAT_LFN_LAST        0x40

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) / (a) : (b))

#pragma pack(push, 1)

typedef struct {
    uint8_t drive_num;
    uint8_t _reversed;
    uint8_t sign;
    uint8_t vol_id;
    uint8_t vol_label[11];
    uint8_t sys_id;
} __attribute__((packed)) fat_ext_boot_record;

typedef struct {
    uint32_t sectors_per_fat;
    uint16_t flags;
    uint16_t fat_ver;
    uint32_t root_dir_cluster;
    uint16_t fs_info_sector;
    uint16_t backup_boot_sector;
    uint8_t _reversed[12];
    fat_ext_boot_record ebr; 
} __attribute__((packed)) fat32_ext_boot_record;

typedef struct {
    uint8_t boot_jumps_instruct[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sect;
    uint8_t sects_per_cluster;
    uint16_t reversed_sects;
    uint8_t fat_count;
    uint16_t dir_entry_count;
    uint16_t total_sects;
    uint8_t media_desc_type;
    uint16_t sects_per_fat;
    uint16_t sects_per_track;
    uint16_t heads;
    uint32_t hidden_sects;
    uint32_t large_sects_count;
    
    union {
        fat_ext_boot_record EBR1216;
        fat32_ext_boot_record EBR32;
    };
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
    uint8_t order;
    uint16_t chars1[5];
    uint8_t attribute;
    uint8_t long_entry_type;
    uint8_t checksum;
    int16_t chars2[6];
    uint16_t _always_zero;
    int16_t chars3[2];
} __attribute__((packed)) fat_long_file_entry_t;

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
    FAT_ATTR_LFN = FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID
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
    uint8_t order;
    int16_t chars[13];
} fat_lfn_blocks_t;

typedef struct {
    
    union {
        boot_sect_t boot_sect;
        uint8_t boot_sect_bytes[SECTOR_SIZE];
    } BS;

    fat_file_data_t root_dir;
    fat_file_data_t opened_files[MAX_FILE_HANDLES];
    uint8_t fat_cache[FAT_CACHE_SIZE + SECTOR_SIZE];
    uint32_t fat_cache_pos;
    fat_lfn_blocks_t lfn_blocks[FAT_LFN_LAST];
    int lfn_count;
} fat_data_t;

int fat_compare_lfn_blocks(const void* block_a, const void* block_b);
boolean fat_read_boot_sect(partition_t* disk);
boolean fat_read_fat(partition_t* disk, unsigned int lba_index);
void fat_detect(partition_t* disk);
uint32_t fat_cluster_to_lba(uint32_t cluster);
boolean fat_init(partition_t* disk);
fat_file_t* fat_open_entry(partition_t* disk, dir_entry_t* entry);
uint32_t fat_next_cluster(partition_t* disk, uint32_t current_cluster);
uint32_t fat_read(partition_t* disk, fat_file_t* file, uint32_t byte_count, void* data_out);
boolean fat_read_entry(partition_t* disk, fat_file_t* file, dir_entry_t* dir_entry);
void fat_close(fat_file_t* file);
void fat_get_short_name(const char* name, char short_name[12]);
boolean fat_find_file(partition_t* disk, fat_file_t* file, const char* name, dir_entry_t* entry_out);
fat_file_t* fat_open(partition_t* disk, const char* path);

#ifdef __cplusplus
}
#endif

#endif
