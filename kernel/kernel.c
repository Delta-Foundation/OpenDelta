#include "./lib/stdbase.h"
#include "./lib/system.h"
#include "./lib/types.h"
#include "./lib/multiboot.h"
#include "./lib/screen.h"
#include "./mem/header/memory.h"
#include "./lib/tty.h"
#include "./arch/gdt/gdt.h"
#include "./lib/isr.h"
#include "./fpu/fpu.h"
#include "./lib/mouse.h"
#include "./lib/pic.h"
#include "./lib/speaker.h"
/* #include "./tools/fat/headers/disk.h"
#include "./tools/fat/headers/mbr.h"
#include "./tools/fat/headers/fat.h"
#include "./tools/fat/headers/elf.h" */
#include "./tty/header/min_dltsh.h"

#define DEF_MEM_LOWER 320
#define DEF_MEM_UPPER (32 * 1024)

extern char readp(uint16_t port);
extern void writep(uint16_t port, uint8_t data);

static uint8_t boot_drive = 0x80;
static int debug_row = 0;

static void kpanic(const char *panic_msg, ...) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    const char* msg = "KERNEL PANIC: ";
    
    for(int i=0; msg[i]; i++) {  
        vga[i] = (msg[i] << 8) | 0x4F; 
    }
    
    for(int i=0; panic_msg[i]; i++) { 
        vga[14+i] = (panic_msg[i] << 8) | 0x4F; 
    }

    __asm__ volatile ( "cli" );

    for (;;) {
        __asm__ volatile ( "hlt" );
    }
}

static void early_kprint(const char *str, uint8_t color) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    int col = 0;

    for (int i = 0; str[i]; i++) {
        if (str[i] == '\n') {
            debug_row++;
            col = 0;
            continue;
        }

        vga[debug_row * 80 + col] = (uint16_t)str[i] | ((uint16_t)color << 8);
        col++;
    }

    debug_row++;
}

__attribute__((noreturn))
void kmain(void)
{
    __asm__ volatile ("cli");

    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;

    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = 0x0F20; // 0x0F = чёрный фон, 0x20 = пробел
    }

    early_kprint("Welcome to the OpenDelta!\n", CYAN);
    early_kprint("OS: OpenDelta v0.2-a\n", CYAN);
    early_kprint("Kernel: dltkernel v0.0.10-b\n", CYAN);
   
    early_kprint("[INFO]: Initializing GDT...   ", WHITE);
    i386_GDT_init();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Masking all IRQs...   ", WHITE);
    mask_all();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Remapping PIC...   ", WHITE);
    pic_remap();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Initializing ISR and IRQ...   ", WHITE);
    install_isr_and_irq();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Setting up memory management and shared memory...     ", WHITE);
    paggingInstall(DEF_MEM_LOWER + DEF_MEM_UPPER);
    heapInstall();
    shmInstall();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Installing syscalls...    ", WHITE);
    syscallInstall();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Initializing FPU...   ", WHITE);
    fpu_install();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Installing device drivers...    ", WHITE);
    mouse_install();
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Enabling interrupts...    ", WHITE);
    outb(0x21, 0xF8);
    outb(0xA1, 0xEF);
    __asm__ volatile ( "sti" );
    early_kprint("[OK]\n", GREEN);

    early_kprint("[INFO]: Starting terminal and shell...    ", WHITE);
    beep_boop();

    terminalInit();
    min_dltsh();

    early_kprint("[INFO]: System halted!\n", CYAN);

    __asm__ volatile ( "cli" );

    while (TRUE) {
        __asm__ __volatile__ ("hlt");
    }
}
