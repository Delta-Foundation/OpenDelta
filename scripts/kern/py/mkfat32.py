#!/usr/bin/env python3

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
PROC_CLUSTER = 4
PROC1_CLUSTER = 5
PROC2_CLUSTER = 6
PROC3_CLUSTER = 7

README_DATA = (
    b"OpenDelta FAT32 volume.\n"
    b"Use: ls, pwd, cd, cat, touch, mkdir, rmdir, rmfile.\n"
)

PROC_FILES = [
    (b"PROC1   TXT", PROC1_CLUSTER, b"1: CINIT"),
    (b"PROC2   TXT", PROC2_CLUSTER, b"2: TTY"),
    (b"PROC3   TXT", PROC3_CLUSTER, b"3: DLTSH"),
]


def partition_size(total_sectors):
    part_size = total_sectors - PART_START
    if part_size <= 0:
        raise RuntimeError("image is too small for a FAT32 partition")
    return part_size


def fat_layout(total_sectors):
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

    for cluster, value in enumerate(
        [0x0FFFFFF8, 0x0FFFFFFF] + [0x0FFFFFFF] * 6
    ):
        struct.pack_into("<I", fat, cluster * 4, value)

    return bytes(fat)


def build_file_entry(name, cluster, data):
    entry = bytearray(32)
    entry[0:11] = name
    entry[11] = 0x20  # обычный файл
    struct.pack_into("<H", entry, 16, 0x0021)
    struct.pack_into("<H", entry, 18, 0x0021)
    struct.pack_into("<H", entry, 24, 0x0021)
    struct.pack_into("<H", entry, 26, cluster)
    struct.pack_into("<I", entry, 28, len(data))
    return bytes(entry)


def build_directory_entry(name, cluster):
    entry = bytearray(32)
    entry[0:11] = name
    entry[11] = 0x10  # каталог
    struct.pack_into("<H", entry, 26, cluster)
    return bytes(entry)


def build_dot_entry(name, cluster):
    entry = bytearray(32)
    entry[0:11] = name
    entry[11] = 0x10  # каталог
    struct.pack_into("<H", entry, 26, cluster)
    return bytes(entry)


def write_fat32(image, total_sectors):
    fat_sectors, cluster_count = fat_layout(total_sectors)

    free_clusters = cluster_count - 6
    next_free = 8

    part_offset = PART_START * SECTOR
    boot = build_boot_sector(total_sectors, fat_sectors, 0x4F50454E)
    fsinfo = build_fsinfo(free_clusters, next_free)
    fat = build_fat(fat_sectors)

    write_at(image, part_offset, boot)
    write_at(image, part_offset + 6 * SECTOR, boot)

    write_at(image, part_offset + 1 * SECTOR, fsinfo)
    write_at(image, part_offset + 7 * SECTOR, fsinfo)

    fat_offset = part_offset + RESERVED_SECTORS * SECTOR
    write_at(image, fat_offset, fat)
    write_at(image, fat_offset + fat_sectors * SECTOR, fat)

    data_offset = fat_offset + FAT_COUNT * fat_sectors * SECTOR
    cluster_bytes = SECTORS_PER_CLUSTER * SECTOR

    def cluster_offset(cluster):
        return data_offset + (cluster - 2) * cluster_bytes

    for cluster in range(ROOT_CLUSTER, PROC3_CLUSTER + 1):
        write_at(image, cluster_offset(cluster), b"\x00" * cluster_bytes)

    root_entries = (
        build_file_entry(b"README  TXT", README_CLUSTER, README_DATA)
        + build_directory_entry(b"PROC       ", PROC_CLUSTER)
    )
    write_at(image, cluster_offset(ROOT_CLUSTER), root_entries)

    write_at(image, cluster_offset(README_CLUSTER), README_DATA)

    proc_entries = (
        build_dot_entry(b".          ", PROC_CLUSTER)
        + build_dot_entry(b"..         ", ROOT_CLUSTER)
    )
    for name, cluster, data in PROC_FILES:
        proc_entries += build_file_entry(name, cluster, data)
    write_at(image, cluster_offset(PROC_CLUSTER), proc_entries)

    for _, cluster, data in PROC_FILES:
        write_at(image, cluster_offset(cluster), data)

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
