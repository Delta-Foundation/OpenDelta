#include "./disk.h"

boolean disk_init(DISK *disk, const char *filename) {
    disk->file = fopen((const char *)filename, "r");
    return (disk->file != NULL);
}

void disk_lba2chs(DISK* disk, uint32_t lba, 
                  uint16_t* cylinder_out, 
                  uint16_t* sector_out, 
                  uint16_t head_out) 
{
    *sector_out = lba % disk->sectors + 1;
    *cylinder_out = (lba / disk->sectors) / disk->heads;
    head_out = (lba / disk->sectors) % disk->heads;
}
