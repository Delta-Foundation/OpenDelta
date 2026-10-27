#include "../libc/include/stdbase.h"
#include "../libc/include/string.h"
#include "../libc/include/types.h"
#include "../kernel-lib/screen.h"
#include "../kernel-lib/keyboard.h"
#include "../kernel-lib/ports.h"
#include "./header/min_dltsh.h"

#define MCL 255
#define MAX_ARGS 10

typedef struct {
    const char * user_name;
    const char * os_title;
    const char * host_name;
    const char * cpu;
    const char * gpu;
} UserInfo;

static void welcome_msg(void) {
    prints("._________________________________________.\n", CYAN);
//    prints("|                                         |\n", CYAN);
    prints("|                                         |\n", CYAN);
    prints("|  Welcome To the DLTSH(minimal version)  |\n", CYAN);
//    prints("|                                         |\n", CYAN);
    prints("|_________________________________________|\n", CYAN);
}

static void print_fetch(void) {
    UserInfo user = {
        .user_name = "Root",
        .host_name = "Host",
        .os_title  = "OpenDelta v0.2-a (Via)",
        .cpu       = "cpu",
        .gpu       = "ventilator 3000"
    };

    prints("        ____               User:        %s\n", CYAN, user.user_name);
    prints("       /   /               ---------------------------\n", CYAN);
    prints("      /   / /\\             os:          %s\n", CYAN, user.os_title);
    prints("     /   / /  \\            kernel:      dltkernel v0.1.0-a\n", CYAN);
    prints("    /   /  \\   \\           shell:       dltsh (minimal version)\n", CYAN);
    prints("   /   /    \\   \\          packages:    fpmcp (netu)\n", CYAN);
    prints("  /   /  /\\  \\   \\         terminal:    TTY\n", CYAN);
    prints(" /   /  /  \\  \\   \\        host:        %s\n", CYAN, user.host_name);
    prints("/   /  /    \\  \\   \\       cpu:         %s\n", CYAN, user.cpu);
    prints("\\__/  /______\\  \\__/       gpu:         %s\n", CYAN, user.gpu);
}

static void command_echo(int argc, char ** argv) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            prints(" ", WHITE);
        }
        prints(argv[i], WHITE);
    }
    prints("\n", WHITE);
}

static void print_help(void) {
    prints("dltsh built-in commands:\n", WHITE);
    prints("  help      show this help\n", LGRAY);
    prints("  fetch     show system information\n", LGRAY);
    prints("  about     shell/kernel version\n", LGRAY);
    prints("  echo ...  print arguments\n", LGRAY);
    prints("  clear     clear the screen\n", LGRAY);
    prints("  reboot    restart the machine\n", LGRAY);
    prints("  poweroff  power the machine off\n", LGRAY);
    prints("  exit      leave the shell\n", LGRAY);
}

static void print_about(void) {
    prints("dltsh - minimal OpenDelta shell\n", CYAN);
    prints("kernel: dltkernel v0.1.0-a\n", LGRAY);
    prints("build:  " __DATE__ " " __TIME__ "\n", LGRAY);
}

static int split_args(char * line, char ** argv, int max) {
    int argc = 0;
    char * p = line;

    while (*p != '\0') {
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        if (argc >= max) {
            break;
        }

        argv[argc++] = p;

        while (*p != '\0' && *p != ' ' && *p != '\t') {
            p++;
        }

        if (*p != '\0') {
            *p++ = '\0';
        }
    }

    return argc;
}

void min_dltsh(void) {
//    clear_screen();
    welcome_msg();
    prints("[INFO]: [mode - ROOT]\n", WHITE);
    prints("Type 'help' to see available commands.\n", LGRAY);

    char line[MCL + 1];
    char * argv[MAX_ARGS];

    for (;;) {
        prints("#> ", WHITE);

        int len = console_readline(line, sizeof(line));
        if (len < 0) {
            break;
        }

        int argc = split_args(line, argv, MAX_ARGS);
        if (argc == 0) {
            continue;
        }

        if (strcmp(argv[0], "exit") == 0) {
            prints("logout\n", CYAN);
            break;
        }
        else if (strcmp(argv[0], "help") == 0) {
            print_help();
        }
        else if (strcmp(argv[0], "fetch") == 0) {
            print_fetch();
        }
        else if (strcmp(argv[0], "about") == 0 || strcmp(argv[0], "version") == 0) {
            print_about();
        }
        else if (strcmp(argv[0], "echo") == 0) {
            command_echo(argc, argv);
        }
        else if (strcmp(argv[0], "clear") == 0) {
            clear_screen();
        }
        else if (strcmp(argv[0], "reboot") == 0) {
            prints("reboot...\n", WHITE);
            delay();
            system_reboot();
        }
        else if (strcmp(argv[0], "poweroff") == 0 || strcmp(argv[0], "halt") == 0) {
            prints("auf wiedersehen!\n", WHITE);
            delay();
            system_poweroff();
        }
        else {
            prints("[ERROR]: command not found: ", RED);
            prints(argv[0], RED);
            prints("\n", RED);
            continue;
        }
    }
}
