#ifndef _TOOLS_FAT_INTERNAL_H
#define _TOOLS_FAT_INTERNAL_H

#pragma once

#include "./fat.h"
#include "./mbr.h"

#define FAT_CLUSTER_MASK        0x0FFFFFFF
#define FAT_CLUSTER_FREE        0x00000000
#define FAT_CLUSTER_BAD         0x0FFFFFF7
#define FAT_CLUSTER_EOC_MIN     0x0FFFFFF8

#define FAT_ROOT_PARENT         0

#pragma pack(push, 1)

typedef struct {
    uint8_t jump[3];
    uint8_t oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t root_entry_count;
    uint16_t total_sectors16;
    uint8_t media;
    uint16_t fat_size16;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors32;
    uint32_t fat_size32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
    uint16_t fsinfo_sector;
    uint16_t backup_boot_sector;
    uint8_t reserved[12];
    uint8_t drive_number;
    uint8_t reserved1;
    uint8_t boot_signature;
    uint32_t volume_id;
    uint8_t volume_label[11];
    uint8_t fs_type[8];
} fat32_bpb_t;

#pragma pack(pop)

typedef boolean (*fat_dir_visit_t)(
    fat_dir_entry_t * entry,
    uint32_t entry_lba,
    uint32_t entry_offset,
    void * context
);

typedef struct {
    DISK disk;
    partition_t part;
    fat32_bpb_t bpb;

    uint32_t fat_lba;
    uint32_t data_lba;
    uint32_t cluster_count;
    uint32_t root_cluster;
    uint32_t sectors_per_cluster;
    uint32_t bytes_per_cluster;
    uint32_t total_sectors;
    uint32_t next_free;

    boolean mounted;
    char label[12];

    uint8_t sector[FAT_SECTOR_SIZE];
    uint8_t fat_sector[FAT_SECTOR_SIZE];
    uint8_t fat_io[FAT_SECTOR_SIZE];
    uint32_t fat_sector_lba;
    boolean fat_sector_valid;

    fat_file_t handles[FAT_MAX_HANDLES];
} fat_state_t;

fat_state_t * fat_get_state(void);

/* Low level sector and FAT access. */
boolean fat_read_sector(uint32_t lba, void * data_out);
boolean fat_write_sector(uint32_t lba, const void * data_in);
uint32_t fat_cluster_lba(uint32_t cluster, uint32_t sector);
uint32_t fat_next_cluster(uint32_t cluster);
uint32_t fat_get_entry(uint32_t cluster);
boolean fat_set_entry(uint32_t cluster, uint32_t value);
uint32_t fat_alloc_cluster(void);
boolean fat_free_chain(uint32_t first_cluster);
boolean fat_zero_cluster(uint32_t cluster);

/* Directory helpers shared by fat_dir.c and fat_file.c. */
uint32_t fat_entry_cluster(const fat_dir_entry_t * entry);

boolean fat_dir_walk(
    uint32_t dir_cluster,
    fat_dir_visit_t visit,
    void * context
);

boolean fat_dir_find(
    uint32_t dir_cluster,
    const char * name,
    fat_dir_entry_t * entry_out,
    uint32_t * lba_out,
    uint32_t * offset_out
);

boolean fat_dir_resolve(
    const char * path,
    uint32_t * cluster_out
);

boolean fat_lookup(
    const char * path,
    fat_dir_entry_t * entry_out,
    uint32_t * lba_out,
    uint32_t * offset_out
);

#endif
