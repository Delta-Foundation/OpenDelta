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

extern char readp(uint16_t port);
extern void writep(uint16_t port, uint8_t data);
extern void load_idt(uintptr_t *idt_ptr);

IDTEntry IDT[IDT_SIZE];
struct Multiboot *mboot = NULL;

static void kpanic(const char *panic_msg) {
    prints("KERNEL PANIC: ", WHITE);
    prints(panic_msg, WHITE);
    prints("\n", WHITE);
    for (;;) { 
        __asm__ volatile ( "hlt" );
    }
}

void IdtInit(void) 
{
    unsigned long idt_address;
    unsigned long idt_ptr[2];

    IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
    IDT[0x21].zero = 0;
    IDT[0x21].type_attr = INTERRUPT_GATE;

    writep(0x20, 0x11);
    writep(0xA0, 0x11);

    writep(0x21, 0x20);
    writep(0xA1, 0x28);

    writep(0x21, 0x00);
    writep(0xA1, 0x00);

    writep(0x21, 0x01);
    writep(0xA1, 0x01);

    writep(0x21, 0xff);
    writep(0xA1, 0xff);

    idt_address = (unsigned long)IDT;
    idt_ptr[0] = (sizeof(IDTEntry) *IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;

    load_idt(idt_ptr);
}

void entry_point(void)
{
    const char *welcome = "Welcome to the OpenDelta!\n";
    const char *os_name = "OS: OpenDelta v0.1-a\n";
    const char *kern_name = "Kernel: dltkernel v0.0.7-p\n";    

    clearScreen();

    prints(welcome, CYAN);
    prints(os_name, WHITE);
    prints(kern_name, WHITE);

    delay();

    prints("[info]: [initializing isr and irq]\n", WHITE);
    isr_install();
    irq_init();

    __asm__ volatile ( "int $2" );
    __asm__ volatile ( "int $3" );

    prints("[info]: [initializing i386 GDT]\n", WHITE);
    i386_GDT_init();

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
    pic_driver();
    
    return;
}

void kmain(void)
{
    prints("[info]: [Initializing IDT]\n", WHITE);
    IdtInit();

    prints("[info]: [starting TTY]\n", WHITE);
    terminalInit();
    delay();

    while (TRUE) {
        // entry_point();
        for (volatile uint32_t i = 0; i < 10000; i++) {
            __asm__ __volatile__ ("hlt");
        }
    }
}

