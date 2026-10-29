#include "./headers/disk.h"
#include "./headers/fat.h"
#include "../../kernel-lib/ports.h"

#define ATA_PRIMARY_IO      0x1F0
#define ATA_PRIMARY_CONTROL 0x3F6

#define ATA_REG_DATA        0
#define ATA_REG_ERROR       1
#define ATA_REG_COUNT       2
#define ATA_REG_LBA0        3
#define ATA_REG_LBA1        4
#define ATA_REG_LBA2        5
#define ATA_REG_DRIVE       6
#define ATA_REG_STATUS      7
#define ATA_REG_COMMAND     7

#define ATA_CMD_READ        0x20
#define ATA_CMD_WRITE       0x30
#define ATA_CMD_FLUSH       0xE7

#define ATA_STATUS_BSY      0x80
#define ATA_STATUS_DRQ      0x08
#define ATA_STATUS_ERR      0x01

#define ATA_TIMEOUT         1000000

static uint8_t ata_status(void) {
    return port_byte_in(ATA_PRIMARY_IO + ATA_REG_STATUS);
}

static boolean ata_wait_not_busy(void) {
    for (int i = 0; i < ATA_TIMEOUT; i++) {
        uint8_t status = ata_status();

        if ((status & ATA_STATUS_BSY) == 0) {
            return (status & ATA_STATUS_ERR) == 0;
        }
    }

    return FALSE;
}

static boolean ata_wait_drq(void) {
    for (int i = 0; i < ATA_TIMEOUT; i++) {
        uint8_t status = ata_status();

        if (status & ATA_STATUS_ERR) {
            return FALSE;
        }

        if ((status & ATA_STATUS_BSY) == 0 && (status & ATA_STATUS_DRQ) != 0) {
            return TRUE;
        }
    }

    return FALSE;
}

static void ata_select(uint32_t lba) {
    port_byte_out(
        ATA_PRIMARY_IO + ATA_REG_DRIVE,
        0xE0 | ((lba >> 24) & 0x0F)
    );

    for (int i = 0; i < 4; i++) {
        (void)ata_status();
    }
}

static void ata_set_lba(uint32_t lba) {
    port_byte_out(ATA_PRIMARY_IO + ATA_REG_COUNT, 1);
    port_byte_out(ATA_PRIMARY_IO + ATA_REG_LBA0, (uint8_t)(lba & 0xFF));
    port_byte_out(ATA_PRIMARY_IO + ATA_REG_LBA1, (uint8_t)((lba >> 8) & 0xFF));
    port_byte_out(ATA_PRIMARY_IO + ATA_REG_LBA2, (uint8_t)((lba >> 16) & 0xFF));
}

static boolean ata_read_sector(uint32_t lba, void * data_out) {
    uint16_t * out = (uint16_t *)data_out;

    if (!ata_wait_not_busy()) {
        return FALSE;
    }

    ata_select(lba);
    ata_set_lba(lba);
    port_byte_out(ATA_PRIMARY_IO + ATA_REG_COMMAND, ATA_CMD_READ);

    if (!ata_wait_drq()) {
        return FALSE;
    }

    for (int i = 0; i < FAT_SECTOR_SIZE / 2; i++) {
        out[i] = port_word_in(ATA_PRIMARY_IO + ATA_REG_DATA);
    }

    return ata_wait_not_busy();
}

static boolean ata_write_sector(const void * data_in) {
    const uint16_t * in = (const uint16_t *)data_in;

    if (!ata_wait_not_busy()) {
        return FALSE;
    }

    for (int i = 0; i < FAT_SECTOR_SIZE / 2; i++) {
        port_word_out(ATA_PRIMARY_IO + ATA_REG_DATA, in[i]);
    }

    if (!ata_wait_not_busy()) {
        return FALSE;
    }

    return TRUE;
}

boolean disk_init(
    DISK * disk,
    uint8_t drive
) {
    if (disk == NULL || drive != 0x80) {
        return FALSE;
    }

    disk->id = drive;

    port_byte_out(ATA_PRIMARY_CONTROL, 0x02);
    return ata_wait_not_busy();
}

boolean disk_read_sects(
    DISK * disk,
    uint32_t lba,
    uint8_t sectors,
    void * data_out
) {
    uint8_t * out = (uint8_t *)data_out;

    for (uint8_t i = 0; i < sectors; i++) {
        if (!ata_read_sector(lba + i, out + i * FAT_SECTOR_SIZE)) {
            return FALSE;
        }
    }

    return TRUE;
}

boolean disk_write_sects(
    DISK * disk,
    uint32_t lba,
    uint8_t sectors,
    const void * data_in
) {
    const uint8_t * in = (const uint8_t *)data_in;

    for (uint8_t i = 0; i < sectors; i++) {
        if (!ata_wait_not_busy()) {
            return FALSE;
        }

        ata_select(lba + i);
        ata_set_lba(lba + i);
        port_byte_out(ATA_PRIMARY_IO + ATA_REG_COMMAND, ATA_CMD_WRITE);

        if (!ata_wait_drq()) {
            return FALSE;
        }

        if (!ata_write_sector(in + i * FAT_SECTOR_SIZE)) {
            return FALSE;
        }
    }

    port_byte_out(ATA_PRIMARY_IO + ATA_REG_COMMAND, ATA_CMD_FLUSH);
    return ata_wait_not_busy();
}
