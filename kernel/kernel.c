#include "./lib/stdbase.h"
#include "./lib/system.h"
#include "./lib/types.h"
#include "./lib/multiboot.h"
#include "./lib/screen.h"
#include "./mem/header/memory.h"
#include "./lib/tty.h"
#include "./arch/gdt/gdt.h"
#include "./lib/idt.h"
#include "./lib/isr.h"
#include "./fpu/fpu.h"
#include "./lib/pic.h"
#include "./tools/fat/headers/disk.h"
#include "./tools/fat/headers/mbr.h"
#include "./tools/fat/headers/fat.h"
#include "./tools/fat/headers/elf.h"
#include "./lib/bootparams.h"
#include "./tty/header/min_dltsh.h"

extern char readp(uint16_t port);
extern void writep(uint16_t port, uint8_t data);
extern void load_idt(uintptr_t *idt_ptr);

IDTEntry IDT[IDT_SIZE];
uint8_t *boot_drive = (uint8_t *)0x80;
boot_params_t *boot_params = NULL;
void* partition = NULL;

static void kpanic(const char *panic_msg, ...) {
    prints("KERNEL PANIC: ", WHITE);
    prints(panic_msg, WHITE);
    prints("\n", WHITE);

    __asm__ volatile ( "cli" );

    for (;;) { 
        __asm__ volatile ( "hlt" );
    }
}

void __attribute__((cdecl)) kmain(uint32_t magic, boot_params_t* bp)
{
    if (!bp) {
        kpanic("[KERNEL PANIC]: [boot parameters pointer is NULL]\n");
    }

    boot_params = bp;
    *boot_drive = (bp->boot_drive & 0xFF);

    clearScreen();

    const char *welcome = "Welcome to the OpenDelta!\n";
    const char *os_name = "OS: OpenDelta v0.1-a\n";
    const char *kern_name = "Kernel: dltkernel v0.0.9-p\n";    

    prints(welcome, CYAN);
    prints(os_name, WHITE);
    prints(kern_name, WHITE);

    prints("[info]: [initializing i386 GDT]\n", WHITE);
    i386_GDT_init();

    prints("[info]: [initializing isr and irq]\n", WHITE);
    isr_install();
    irq_init();


    prints("[info]: [install memory management and shared memory]\n", WHITE);
    paggingInstall(bp->mem_lower + bp->mem_upper);
    heapInstall();
    shmInstall();
    
    prints("[info]: [install hardware drivers]\n", WHITE);
    prints("[info]: [install fpu driver]\n", WHITE);
    fpu_install();
    syscallInstall();
    
    /* prints("[info]: [initializing disk]\n", WHITE);
     DISK disk;
     if (!disk_init(&disk, (const char *)boot_drive)) {
         prints("[ERROR]: [Disk init error!]\r\n", RED);
         goto end;
     }

     partition_t* part = NULL;
     mbr_detect_part(part, &disk, partition);

     if (!fat_init(part)) {
         prints("[FAT ERROR]: [fat init error]\r\n", RED);
         goto end;
     }

     boot_params->boot_device = (unsigned long)boot_drive;
     if (!elf_read(part, "/boot/bin/kernel.elf", (void**)kmain)) {
         prints("[FATAL ERROR]: [failed to read, booting halted!]", RED);
         goto end;
     } */

    prints("[info]: [starting TTY]\n", WHITE);
    terminalInit();

    prints("[info]: [starting minimal dltsh]", WHITE);
    min_dltsh();

end: 
    while (TRUE) {
        __asm__ __volatile__ ("hlt");
    }
}
