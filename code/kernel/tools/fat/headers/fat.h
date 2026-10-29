#ifndef _TOOLS_FAT_H
#define _TOOLS_FAT_H

#pragma once

#include "../../../libc/include/types.h"

#define FAT_SECTOR_SIZE     512
#define FAT_MAX_PATH        256
#define FAT_MAX_HANDLES     4

#define FAT_ATTR_READ_ONLY  0x01
#define FAT_ATTR_HIDDEN     0x02
#define FAT_ATTR_SYSTEM     0x04
#define FAT_ATTR_VOLUME_ID  0x08
#define FAT_ATTR_DIR        0x10
#define FAT_ATTR_ARCHIVE    0x20
#define FAT_ATTR_LFN        0x0F

#define FAT_ENTRY_FREE      0xE5
#define FAT_ENTRY_END       0x00

#pragma pack(push, 1)

typedef struct {
    uint8_t name[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t created_tenths;
    uint16_t created_time;
    uint16_t created_date;
    uint16_t accessed_date;
    uint16_t first_cluster_high;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t first_cluster_low;
    uint32_t size;
} fat_dir_entry_t;

#pragma pack(pop)

typedef struct {
    int handle;
    boolean is_dir;
    boolean opened;
    uint32_t position;
    uint32_t size;
    uint32_t first_cluster;
    uint32_t current_cluster;
    uint32_t sector_in_cluster;
} fat_file_t;

typedef boolean (*fat_dir_callback_t)(
    const char * name,
    boolean is_dir,
    uint32_t size,
    void * context
);

/* Mount / information. */
boolean fat_mount(uint8_t drive);
boolean fat_mounted(void);
const char * fat_volume_label(void);

/* Lookups. */
boolean fat_exists(const char * path);
boolean fat_is_dir(const char * path);
boolean fat_list_dir(
    const char * path,
    fat_dir_callback_t callback,
    void * context
);

/* Mutations. */
boolean fat_create_file(const char * path);
boolean fat_create_dir(const char * path);
boolean fat_delete_file(const char * path);
boolean fat_delete_dir(const char * path);

/* File access. */
fat_file_t * fat_open(const char * path);

uint32_t fat_read(
    fat_file_t * file,
    uint32_t count,
    void * data_out
);

boolean fat_write_file(
    const char * path,
    const void * data,
    uint32_t size
);

void fat_close(fat_file_t * file);

#endif
