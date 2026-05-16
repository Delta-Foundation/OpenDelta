#include "../lib/stdbase.h"
#include "../lib/string.h"
#include "../lib/types.h"
#include "../lib/errno.h"
#include "../lib/ctype.h"
#include "../lib/screen.h"
#include "../lib/ports.h"
#include "../ints/header/int.h"

#define MCL 255

struct Console {
    char command[MCL];
    char *fileName;
    char *folderName;
};

void welcome_msg(void) {
    prints("._________________________________________.\n", WHITE);
    prints("|                                         |\n", WHITE);
    prints("|                                         |\n", WHITE);
    prints("|  Welcome To the DLTSH(minimal version)  |\n", WHITE);
    prints("|                                         |\n", WHITE);
    prints("|_________________________________________|\n", WHITE);
}

void min_dltsh(void) 
{ 
    clearScreen();
    welcome_msg();

    struct Console console;

    while (TRUE) {
        prints("[INFO]: [MODE = ROOT]\n", WHITE);
        prints("#: > ", WHITE); 
    
        if (scanf("%255s", console.command) != 1) {
            fprintf((FILE *)stderr, "[err]: [error of input command!]\n");
        }

        else if (strcmp(console.command, "clear") == 0) {
            clearScreen();
        }
        
        else if (strcmp(console.command, "pwd") == 0) {
            prints("/\n", BLUE);
        }

        else if (strcmp(console.command, "reboot") == 0) {
            prints("reboot...", WHITE);
            delay();
            APMInterfaceInit();
        }

        else if (strcmp(console.command, "poweroff") == 0) {
            prints("auf wiedersehen!", WHITE);
            
            clearScreen();

            if (are_ints_enabled()) {
                __asm__ ("cli");
            }

            __asm__ ("hlt");
        }

        else if (strcmp(console.command, "whoami") == 0) {
            prints("HZHZ chyo za user\n", CYAN);
        }

        else {
            prints("[ERR]: [Unknown command!]\n", RED);
        }

        int c;
        while((c = getchar()) != '\n' && c != EOF) { ; }
    }
}
