#!/usr/bin/env python3
"""Create a small FAT32 partition inside the OpenDelta disk image.

The kernel image is stored at the beginning of the raw disk, so the FAT32
volume lives at a fixed LBA after it (LBA 2048).  This tool is intentionally
tiny and only creates what the kernel driver needs: one FAT32 volume with a
root directory and a README.TXT file.
"""

import os
import struct
import sys

SECTOR = 512
PART_START = 2048
SECTORS_PER_CLUSTER = 1
RESERVED_SECTORS = 32
FAT_COUNT = 2
ROOT_CLUSTER = 2
README_CLUSTER = 3

README_DATA = (
    b"OpenDelta FAT32 volume.\n"
    b"Use: ls, cd, cat, touch, mkdir, rmdir, rmfile.\n"
)


def partition_size(total_sectors):
    part_size = total_sectors - PART_START
    if part_size <= 0:
        raise RuntimeError("image is too small for a FAT32 partition")

    return part_size


def fat_layout(total_sectors):
    """Return (fat_sectors, cluster_count) for the requested volume size."""
    fat_sectors = 1

    for _ in range(32):
        data_sectors = total_sectors - RESERVED_SECTORS - FAT_COUNT * fat_sectors
        cluster_count = data_sectors // SECTORS_PER_CLUSTER
        needed = ((cluster_count + 2) * 4 + SECTOR - 1) // SECTOR

        if needed == fat_sectors:
            return fat_sectors, cluster_count

        fat_sectors = needed

    raise RuntimeError("could not calculate FAT size")


def write_at(image, offset, data):
    image.seek(offset)
    image.write(data)


def build_boot_sector(total_sectors, fat_sectors, volume_id):
    bpb = b"\xEB\x58\x90" + b"OPENDFAT"
    bpb += struct.pack(
        "<HBHBHHBHHHII",
        SECTOR,
        SECTORS_PER_CLUSTER,
        RESERVED_SECTORS,
        FAT_COUNT,
        0,
        0,
        0xF8,
        0,
        63,
        255,
        PART_START,
        total_sectors,
    )
    bpb += struct.pack(
        "<IHHIHH12sBBBI11s8s",
        fat_sectors,
        0,
        0,
        ROOT_CLUSTER,
        1,
        6,
        b"\x00" * 12,
        0x80,
        0,
        0x29,
        volume_id,
        b"OPEN DELTA ",
        b"FAT32   ",
    )

    if len(bpb) != 90:
        raise RuntimeError("bad FAT32 BPB size")

    sector = bytearray(SECTOR)
    sector[:len(bpb)] = bpb
    sector[510:512] = struct.pack("<H", 0xAA55)
    return bytes(sector)


def build_fsinfo(free_clusters, next_free):
    sector = bytearray(SECTOR)
    struct.pack_into("<I", sector, 0, 0x41615252)
    struct.pack_into("<I", sector, 484, 0x61417272)
    struct.pack_into("<I", sector, 488, free_clusters)
    struct.pack_into("<I", sector, 492, next_free)
    struct.pack_into("<I", sector, 508, 0xAA550000)
    return bytes(sector)


def build_fat(fat_sectors):
    fat = bytearray(fat_sectors * SECTOR)
    struct.pack_into("<I", fat, 0, 0x0FFFFFF8)
    struct.pack_into("<I", fat, 4, 0x0FFFFFFF)
    struct.pack_into("<I", fat, 8, 0x0FFFFFFF)
    struct.pack_into("<I", fat, 12, 0x0FFFFFFF)
    return bytes(fat)


def build_readme_entry():
    entry = bytearray(32)
    entry[0:11] = b"README  TXT"
    entry[11] = 0x20
    struct.pack_into("<H", entry, 16, 0x0021)
    struct.pack_into("<H", entry, 18, 0x0021)
    struct.pack_into("<H", entry, 24, 0x0021)
    struct.pack_into("<H", entry, 26, README_CLUSTER)
    struct.pack_into("<I", entry, 28, len(README_DATA))
    return bytes(entry)


def write_fat32(image, total_sectors):
    fat_sectors, cluster_count = fat_layout(total_sectors)
    volume_id = 0x4F50454E
    free_clusters = cluster_count - 2

    part_offset = PART_START * SECTOR
    boot = build_boot_sector(total_sectors, fat_sectors, volume_id)
    fsinfo = build_fsinfo(free_clusters, README_CLUSTER + 1)
    fat = build_fat(fat_sectors)

    # Primary boot sector and its backup.
    write_at(image, part_offset, boot)
    write_at(image, part_offset + 6 * SECTOR, boot)

    # FSInfo and its backup.
    write_at(image, part_offset + 1 * SECTOR, fsinfo)
    write_at(image, part_offset + 7 * SECTOR, fsinfo)

    # Both FAT copies.
    fat_offset = part_offset + RESERVED_SECTORS * SECTOR
    write_at(image, fat_offset, fat)
    write_at(image, fat_offset + fat_sectors * SECTOR, fat)

    data_offset = fat_offset + FAT_COUNT * fat_sectors * SECTOR
    cluster_bytes = SECTORS_PER_CLUSTER * SECTOR
    zero_cluster = b"\x00" * cluster_bytes

    # Clear the clusters we reuse so re-formatting an existing image does not
    # leave old directory entries or file data behind.
    write_at(image, data_offset, zero_cluster)
    write_at(image, data_offset + (README_CLUSTER - 2) * cluster_bytes, zero_cluster)

    write_at(image, data_offset, build_readme_entry())
    write_at(
        image,
        data_offset + (README_CLUSTER - 2) * cluster_bytes,
        README_DATA,
    )

    return fat_sectors, cluster_count


def main():
    if len(sys.argv) != 2:
        print("usage: mkfat32.py <disk.img>", file=sys.stderr)
        return 1

    path = sys.argv[1]
    total_sectors = os.path.getsize(path) // SECTOR

    if total_sectors <= PART_START:
        print("image is too small", file=sys.stderr)
        return 1

    with open(path, "r+b") as image:
        write_fat32(image, partition_size(total_sectors))

    return 0


if __name__ == "__main__":
    sys.exit(main())
