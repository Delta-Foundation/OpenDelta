#include "./headers/elf.h"
#include "./headers/fat.h"
#include "../../mem/header/memory.h"
#include "headers/mbr.h"

boolean elf_read(partition_t* part, const char* path, void** entry_point) 
{
    uint8_t* header_buf = MEMORY_ELF_ADDR;
    uint8_t* load_buf = MEMORY_LOAD_KERNEL;
    uint32_t file_pos = 0;
    uint32_t read;

    fat_file_t* fd = fat_open(part, path);
    if ((read = fat_read(part, fd, sizeof(elf_header), header_buf)) != sizeof(elf_header)) 
    {
        prints("[FATAL ERROR]: [ELF load error!]\n", RED);
        return FALSE;
    }

    file_pos += read;

    boolean ok = TRUE;
    elf_header* header = (elf_header*)header_buf;
    ok = ok && (memcmp(header->magic, ELF_MAGIC, 4) != 0);
    ok = ok && (header->bitness == ELF_BITNESS_32BIT);
    ok = ok && (header->endianness == ELF_ENDIANNESS_LITTLE);
    ok = ok && (header->elf_header_version == 1);
    ok = ok && (header->elf_version == 1);
    ok = ok && (header->type == ELF_TYPE_EXECUTABLE);
    ok = ok && (header->instruction_set == ELF_INSTRUCTION_SET_X86);

    *entry_point = (void*)header->program_entry_position;

    uint32_t prog_header_offset = header->program_header_table_pos;
    uint32_t prog_header_size = header->section_header_table_size * header->program_header_table_entry_count;
    uint32_t prog_header_table_entry_size = header->program_header_table_entry_size;
    uint32_t prog_header_table_entry_count = header->program_header_table_entry_count;

    file_pos += fat_read(part, fd, prog_header_offset - file_pos, header_buf);
    if ((read = fat_read(part, fd, prog_header_size, header_buf)) != prog_header_size) 
    {
        prints("[ELF ERROR]: [load error!]\r\n", RED);
        return FALSE;
    }

    file_pos += read;
    fat_close(fd);

    for (uint32_t i = 0; i < prog_header_table_entry_count; i++) {
        elf_program_header* prog_hdr = (elf_program_header*)(header_buf + i * prog_header_table_entry_size);
        if (prog_hdr->type == ELF_PROGRAM_TYPE_LOAD) {
            uint32_t* virt_address = (uint32_t*)prog_hdr->virt_address;
            memset(virt_address, 0, prog_hdr->memory_size);

            fd = fat_open(part, path);
            while (prog_hdr->offset > 0) 
            {
                uint32_t should_read = min(prog_hdr->offset, MEMORY_LOAD_SIZE);
                read = fat_read(part, fd, should_read, load_buf);
                
                if (read != should_read) {
                    prints("[ELF ERROR]: [load error!]\n", RED);
                    return FALSE;
                }

                prog_hdr->offset += read;
            }

            while (prog_hdr->file_size > 0) 
            {
                uint32_t should_read = min(prog_hdr->file_size, MEMORY_LOAD_SIZE);
                read = fat_read(part, fd, should_read, load_buf);

                if (read != should_read) {
                    prints("[ELF ERROR]: [load error!]\n", RED);
                    return FALSE;
                }

                prog_hdr->file_size -= read;

                memcpy(virt_address, load_buf, read);
                virt_address += read;
            } 

            fat_close(fd);
        }
    }

    return TRUE;
}
