#include "./fat.h"

boot_sect_t g_boot_sect;
uint8_t* g_fat = NULL;
dir_entry_t* g_root_dir = NULL;
uint32_t g_root_dir_end;
