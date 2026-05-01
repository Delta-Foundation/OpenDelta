#include "./fat.h"

boot_sect_t g_boot_sect;
uint8_t* g_fat = NULL;
dir_entry_t* g_root_dir = NULL;
uint32_t g_root_dir_end;
static fat_data_t g_actual_data;
static fat_data_t g_data;
static uint8_t *_g_fat = NULL;
static uint32_t g_data_sect_lba;
