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
struct multiboot *mboot = NULL;
boot_params_t *boot_drive;
boot_params_t *boot_params;
void* partition;

static void kpanic(const char *panic_msg, ...) {
    prints("KERNEL PANIC: ", WHITE);
    prints(panic_msg, WHITE);
    prints("\n", WHITE);
    for (;;) { 
        __asm__ volatile ( "hlt" );
    }
}

__attribute__((noreturn)) void __stack_chk_fail(void) {
    while (1) {
        kpanic("[KERNEL PANIC]: [?????????? IDI NAHUY!]");
        __asm__ volatile ("hlt");
    }
}

void kmain(void)
{

    const char *welcome = "Welcome to the OpenDelta!\n";
    const char *os_name = "OS: OpenDelta v0.1-a\n";
    const char *kern_name = "Kernel: dltkernel v0.0.9-p\n";    

    clearScreen();

    prints(welcome, CYAN);
    prints(os_name, WHITE);
    prints(kern_name, WHITE);

    prints("[info]: [initializing i386 GDT]\n", WHITE);
    i386_GDT_init();

    delay();

    prints("[info]: [initializing isr and irq]\n", WHITE);
    isr_install();
    irq_init();

    __asm__ volatile ( "int $3" );

    delay();

    prints("[info]: [install memory management and shared memory]\n", WHITE);

    mboot_ptr = mboot;
    paggingInstall(mboot_ptr->mem_upper + mboot_ptr->mem_lower);
    heapInstall();
    delay();

    shmInstall();
    
    prints("[info]: [install hardware drivers]\n", WHITE);
    prints("[info]: [install fpu driver]\n", WHITE);
    fpu_install();
    syscallInstall();
    
    // prints("[info]: [initializing disk]\n", WHITE);
    // DISK disk;
    // if (!disk_init(&disk, (const char *)boot_drive)) {
    //     prints("[ERROR]: [Disk init error!]\r\n", RED);
    //     goto end;
    // }

    // partition_t* part = NULL;
    // mbr_detect_part(part, &disk, partition);

    // if (!fat_init(part)) {
    //     prints("[FAT ERROR]: [fat init error]\r\n", RED);
    //     goto end;
    // }

    // boot_params->boot_device = (unsigned long)boot_drive;
    // if (!elf_read(part, "/boot/bin/kernel.elf", (void**)kmain)) {
    //     prints("[FATAL ERROR]: [failed to read, booting halted!]", RED);
    //     goto end;
    // }

    prints("[info]: [starting TTY]\n", WHITE);
    terminalInit();

    prints("[info]: [starting minimal dltsh]", WHITE);
    min_dltsh();

    delay();

// end:
    while (TRUE) {
        __asm__ __volatile__ ("hlt");
    }

    kpanic("[KERNEL PANIC]: [end of kernel main function]", CYAN);
}
