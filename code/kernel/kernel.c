#include "./libc/include/stdbase.h"
#include "./libc/include/types.h"

#include "./kernel-lib/system.h"
#include "./kernel-lib/multiboot.h"
#include "./kernel-lib/mouse.h"
#include "./kernel-lib/speaker.h"
#include "./kernel-lib/keyboard.h"
#include "./kernel-lib/screen.h"
#include "./kernel-lib/vga.h"

#include "./mem/header/memory.h"

#include "./fpu/fpu.h"

#include "./tools/fat/headers/fat.h"

#include "./tty/header/min_dltsh.h"
#include "./tty/header/tty.h"

#include "./hal/hal.h"

#include "../cinit/lib/cinit_main.h"

#define DEF_MEM_LOWER 320
#define DEF_MEM_UPPER (32 * 1024)

extern char readp(uint16_t port);
extern void writep(uint16_t port, uint8_t data);

static uint8_t boot_drive = 0x80;

static void kpanic(const char *panic_msg, ...) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    const char* msg = "KERNEL PANIC: ";

    for(int i = 0; msg[i]; i++) {
        vga[i] = (uint16_t)(0x4F00 | (uint8_t)msg[i]);
    }

    for(int i = 0; panic_msg[i]; i++) {
        vga[14 + i] = (uint16_t)(0x4F00 | (uint8_t)panic_msg[i]);
    }

    __asm__ volatile ( "cli" );

    for (;;) {
        __asm__ volatile ( "hlt" );
    }
}

static inline void dump_gdtr(void) {
    uint8_t buf[6];
    __asm__ __volatile__ ( "sgdt %0" : "=m" (buf));

    uint32_t base  = *(uint32_t *)(buf + 2);
    uint16_t limit = *(uint16_t *)(buf);

    (void)base;
    (void)limit;
}

__attribute__((noinline))
static void early_kprint(const char * str, uint8_t color) {
    console_write_colored(str, color);
}

__attribute__((noreturn))
__attribute__((section(".text")))
void kmain(void)
{
    __asm__ volatile ("cli");

    console_init();

    early_kprint("Welcome to the OpenDelta!\n", CYAN);
    early_kprint("OS: OpenDelta v0.2-a (Via)\n", CYAN);
    early_kprint("Kernel: dltkernel v0.1.0-a\n", CYAN);

    early_kprint("[INFO]: Initializing HAL(GDT, IDT, ISR, IRQ)...   ", WHITE);
    hal_init();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Setting up memory management and shared memory...     ", WHITE);
    // pagging_install(DEF_MEM_LOWER + DEF_MEM_UPPER);
    heap_install();
    shm_install();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Installing syscalls...    ", WHITE);
    syscall_install();
    //tasking_install();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Initializing FPU...   ", WHITE);
    fpu_install();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Mounting FAT32...    ", WHITE);
    if (fat_mount(boot_drive)) {
        early_kprint("[OK] ", GREEN);
        early_kprint(fat_volume_label(), LGRAY);
        early_kprint("\n", WHITE);
    }
    else {
        early_kprint("[FAIL]\n", RED);
    }

    early_kprint("[INFO]: Installing device drivers...    ", WHITE);
    //mouse_install();
    keyboard_install();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Enabling interrupts...    ", WHITE);
    outb(0x21, 0xF9);
    outb(0xA1, 0xFF);
    __asm__ volatile ( "sti" );
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Starting cinit... ", WHITE);
    cinit_main();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: System halted!\n", CYAN);

    __asm__ volatile ( "cli" );

    while (TRUE) {
        __asm__ __volatile__ ("hlt");
    }
}
