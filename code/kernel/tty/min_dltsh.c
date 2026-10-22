#include "../libc/include/stdbase.h"
#include "../libc/include/string.h"
#include "../libc/include/types.h"
#include "../libc/include/errno.h"
#include "../libc/include/ctype.h"
#include "../kernel-lib/screen.h"
#include "../kernel-lib/ports.h"
#include "../ints/header/int.h"
#include "./header/min_dltsh.h"

#define MCL 255
#define MAX_USER_NAME_LENGTH 128
#define MAX_OS_TITLE_LANGTH 128
#define MAX_HOST_NAME_LENGTH 128

typedef struct {
    const char * user_name[MAX_USER_NAME_LENGTH];
    const char * os_title[MAX_OS_TITLE_LANGTH];
    const char * host_name[MAX_HOST_NAME_LENGTH];
    const char * cpu[128];
    const char * gpu[128];
    double disk_memory[32768];
} UserInfo;


struct Console {
    char command[MCL];
    char * file_name;
    char * folder_name;
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
  	    .os_title = "OpenDelta v0.2-a (Via)",
  	    .cpu = "cpu",
        .gpu = "ventilator 3000"
    };
  	
    printf("        ____               User:        %s\n", CYAN, *user.user_name);
    printf("       /   /               ---------------------------\n", CYAN);
    printf("      /   / /\\             os:          %s\n", CYAN, *user.os_title);
    printf("     /   / /  \\            kernel:      dltkernel v0.1.0-a\n", CYAN);
    printf("    /   /  \\   \\           shell:       dltsh (minimal version)\n", CYAN);
    printf("   /   /    \\   \\          packeges:    fpmcp (netu)\n", CYAN);
    printf("  /   /  /\\  \\   \\         terminal:    TTY\n", CYAN);
    printf(" /   /  /  \\  \\   \\        host:        %s\n", CYAN, *user.host_name);
    printf("/   /  /    \\  \\   \\       cpu:         %s\n", CYAN, *user.cpu);
    printf("\\__/  /______\\  \\__/       gpu:         %s\n", CYAN, *user.gpu);
}

void min_dltsh(void) 
{ 
    clear_screen();
    welcome_msg();

    char line[256];
    char * argv[10];
    int argc;
    
    prints("[INFO]: [mode - ROOT]\n", WHITE);
        
    for (;;) {
        prints("#> ", WHITE);

        if (scanf(" %255[^\n]", line) != 1) {
            break;
        }

        argc = 0;

        for (int i = 0; line[i] != '\0'; ) {
            while (line[i] == ' ' || line[i] == '\t') {
                i++;
            }

            if (line[i] == '\0') {
                break;
            }

            if (argc == 10) {
                prints("[ERROR]: [Слишком много аргументов]\n", RED);
                break;
            }

            argv[argc++] = &line[i];

            while (line[i] != '\0' && line[i] != ' ' && line[i] != '\t') {
                i++;
            }

            if (line[i] != '\0') {
                line[i++] != '\0';
            }
        }

        if (argc == 0) {
            continue;
        }

        if (strcmp(argv[0], "exit") == 0) {
            break;
        }
        
        else if (strcmp(argv[0], "clear") == 0) {
            clear_screen();
        }
        
        else if (strcmp(argv[0], "reboot") == 0) {
            prints("reboot...", WHITE);
            delay();
            APM_interface_init();
        }

        else if (strcmp(argv[0], "poweroff") == 0) {
            prints("auf wiedersehen!", WHITE);
            
            clear_screen();

            if (are_ints_enabled()) {
                __asm__ ("cli");
            }

            __asm__ ("hlt");
        }

        else {
            printf("[команда: %s, аргументов: %d]\n", argv[0], argc - 1);   
        }
    }
}
