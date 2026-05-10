#include "../lib/string.h"
#include "../lib/system.h"
#include "./header/memory.h"
#include "../lib/bootparams.h"
#include "../tools/fat/headers/tools.h"
#include "../lib/stdbase.h"

void *end;
uint32_t *frames;
uint32_t nframes;
uintptr_t placementPointer = (uintptr_t)&end;
uintptr_t heapEnd = (uintptr_t)NULL;
page_dir_t *currentDir;
page_dir_t *kernelDir;
mem_region_t mem_regions[MAX_REGIONS];
int mem_region_count;

void append(char s[], char n)
{
	int len = strlen(s);
	s[len] = n;
	s[len+1] = '\0';
}

void backspace(char s[])
{
	int len = strlen(s);
	s[len-1] = '\0';
}

void flush(char *var)
{
	int i = 0;
	while(var[i] != '\0')  {
		var[i] = '\0';
		i++;
	}
}

int memcmp(const void* ptr1, const void* ptr2, uint16_t num)
{
    const uint8_t* u8ptr1 = (const uint8_t *)ptr1;
    const uint8_t* u8ptr2 = (const uint8_t *)ptr2;

    for (uint16_t i = 0; i < num; i++) {
        if (u8ptr1[i] != u8ptr2[i]) {
            return 1;
        }
    }

    return 0;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	char *d = dest;
	const char *s = src;

	for(size_t i = 0; i < n; i++) {
		d[i] = s[i];
	}
	return dest;
}

void *memset(void *str, int c, size_t n)
{
	char *s = str;

	for(size_t i = 0; i < n; i++) {
		*(s + i) = c;
	}

	return str;
}

void memdetect(mem_info_t* mem_info)
{
   e820_mem_block block;
   uint32_t continuation = 0;
   int ret;

   mem_region_count = 0;
   ret = get_next_block(&block, &continuation);

   while (ret > 0 && continuation != 0) {
        mem_regions[mem_region_count].begin = block.base;
        mem_regions[mem_region_count].length = block.len;
        mem_regions[mem_region_count].type = block.type;
        mem_regions[mem_region_count].ACPI = block.ACPI;
        ++mem_region_count;

        printf("[INFO E820]: [base=0x%llx length=0x%llx type=0x%x\n", block.base, block.len, block.type);

        ret = get_next_block(&block, &continuation);
    }

    mem_info->region_count = mem_region_count;
    mem_info->regions = mem_regions;
}

void memoryCopy(unsigned char *source, unsigned char *dest, int nbytes)
{
	int i;
	for (i = 0; i < nbytes; i++)  {
		*(dest + i) = *(source + i);
	}
}

void memorySet(uint8_t *dest, uint8_t val, uint32_t len)
{
	uint8_t *temp = (uint8_t *)dest;
	for( ; len != 0; len--) {
		*temp++ = val;
	}
}

void* segoffset_to_linear(void* addr) {
    uint32_t offset = (uint32_t)(addr) & 0xFFFF;
    uint32_t seg = (uint32_t)(addr) >> 16;
    return (void *)(seg * 16 + offset);
}

void paggingInstall(uint32_t memsize)
{
	uintptr_t phys;
	
	memsize -= 0xe001e190;
	nframes = memsize / 4;
    frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(nframes * 8), 0, (uint32_t*)&phys); 
	
	memset(frames, 0, INDEX_FROM_BIT(nframes * 8));

    kernelDir = (page_dir_t *)kmalloc(0, sizeof(page_dir_t), (uint32_t *)&phys);
    memset(kernelDir, 0, sizeof(page_dir_t));

    __asm__ volatile (
        "mov $0x277, %%ecx\n"
        "rdmsr\n"
        "or $0x1000000, %%edx\n"
        "and $0xf9ffffff, %%edx\n"
        "wrmsr\n"
        : : : "ecx", "edx", "eax"
    );
}

void heapInstall(void) {
    heapEnd = (placementPointer + 0x1000) & ~0xFFFF;
}

void switchPageDir(page_dir_t *dir)
{
    currentDir = dir;
    __asm__ volatile(
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "orl $0x80000000, %%eax\n"
        "mov %%eax, %%cr3\n"
        :: "r"(dir->physical_address)
        : "%eax"
    );
}
