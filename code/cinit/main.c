#include "../kernel/libc/include/stdbase.h"
#include "../kernel/kernel-lib/system.h"
#include "../kernel/kernel-lib/screen.h"
#include "../kernel/tty/header/tty.h"
#include "../kernel/tty/header/min_dltsh.h"

int cinit_main() {
    shed_init();
    terminal_init();

    prints("[CINIT INFO]: Starting TTY\n", CYAN);
    prints("[CINIT INFO]: Start minimal dltsh\n", CYAN);

    min_dltsh();

    prints("\n[INFO]: shell exited, system halted\n", CYAN);

    __asm__ volatile ("cli");

    for (;;) {
        __asm__ volatile ("hlt");
    }

    return 0;
}
