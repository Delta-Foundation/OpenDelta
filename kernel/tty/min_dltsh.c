#include "../lib/stdbase.h"
#include "../lib/string.h"
#include "../lib/types.h"
#include "../lib/errno.h"
#include "../lib/ctype.h"
#include "../lib/screen.h"
#include "../lib/ports.h"
#include "../ints/header/int.h"

#define MCL 255
#define MAX_USER_NAME_LENGTH 128
#define MAX_OS_TITLE_LANGTH 128
#define MAX_HOST_NAME_LENGTH 128

typedef struct {
    const char *user_name[MAX_USER_NAME_LENGTH];
    const char *os_title[MAX_OS_TITLE_LANGTH];
    const char *host_name[MAX_HOST_NAME_LENGTH];
    const char *cpu[128];
    const char *gpu[128];
    double disk_memory[32768];
} UserInfo;


struct Console {
    char command[MCL];
    char *fileName;
    char *folderName;
};

void welcome_msg(void) {
    prints("._________________________________________.\n", CYAN);
    prints("|                                         |\n", CYAN);
    prints("|                                         |\n", CYAN);
    prints("|  Welcome To the DLTSH(minimal version)  |\n", CYAN);
    prints("|                                         |\n", CYAN);
    prints("|_________________________________________|\n", CYAN);
}

void print_fetch(void)
{
    UserInfo user = {
  	    .user_name = "User",
  	    .host_name = "Host",
  	    .os_title = "OpenDelta",
  	    .cpu = "cpu",
        .gpu = "ventilator 3000"
    };
  	
    printf("        ____               User:        %s\n", CYAN, *user.user_name);
    printf("       /   /               ---------------------------\n", CYAN);
    printf("      /   / /\\             os:          %s\n", CYAN, *user.os_title);
    printf("     /   / /  \\            kernel:      dltkernel\n", CYAN);
    printf("    /   /  \\   \\           shell:       dltsh\n", CYAN);
    printf("   /   /    \\   \\          packeges:    fpmcp (netu)\n", CYAN);
    printf("  /   /  /\\  \\   \\         terminal:    TTY\n", CYAN);
    printf(" /   /  /  \\  \\   \\        host:        %s\n", CYAN, *user.host_name);
    printf("/   /  /    \\  \\   \\       cpu:         %s\n", CYAN, *user.cpu);
    printf("\\__/  /______\\  \\__/       gpu:         %s\n", CYAN, *user.gpu);
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

        else if (strcmp(console.command, "dlt-fetch") == 0) {
            print_fetch();
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
            prints("[user]: [OpenDelta Root]\n", CYAN);
        }

        else {
            prints("[ERR]: [Unknown command!]\n", RED);
        }

        int c;
        while((c = getchar()) != '\n' && c != EOF) { ; }
    }
}
