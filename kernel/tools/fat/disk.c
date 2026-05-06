#include "./disk.h"
#include "./tools.h"

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

boolean disk_read_sects(DISK* disk, uint32_t lba, uint8_t sectors, void* data_out) 
{
    uint16_t cylinder, sector, head;

    disk_lba2chs(disk, lba, &cylinder, &sector, *&head);

    for (int i = 0; i < 3; i++) {
        if (disk_read(disk->id, cylinder, sector, head, sectors, data_out)) {
            return (int)TRUE;
        }

        disk_reset(disk->id);
    }

    return (int)FALSE;
}

