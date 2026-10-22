#include "../kernel/tty/header/min_dltsh.h"
#include "../kernel/lib/stdbase.h"
#include "../kernel/lib/system.h"
#include "../kernel/tty/header/tty.h"

int cinit_main() {
    prints("[CINIT START]", CYAN);

    shed_init();

    prints("[CINIT INFO]: Starting TTY", CYAN);
    terminal_init();

    prints("[INFO]: Start minimal dltsh", CYAN);
    min_dltsh();

    while (TRUE) {
        shedule();
    }

    return 0;
}
