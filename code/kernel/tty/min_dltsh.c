#include "../libc/include/stdbase.h"
#include "../libc/include/string.h"
#include "../libc/include/types.h"
#include "../libc/include/ctype.h"
#include "../kernel-lib/screen.h"
#include "../kernel-lib/keyboard.h"
#include "../kernel-lib/ports.h"
#include "../mem/header/memory.h"
#include "../tools/fat/headers/fat.h"
#include "./header/fs_shell.h"
#include "./header/min_dltsh.h"
#include "./header/dexide.h"

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
    prints("|                                         |\n", CYAN);
    prints("|  Welcome To the DLTSH(minimal version)  |\n", CYAN);
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
    prints("                           init system: cinit\n", CYAN);
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

static void command_upper(int argc, char ** argv) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            prints(" ", WHITE);
        }

        for (int j = 0; argv[i][j] != '\0'; j++) {
            char ch[2];

            ch[0] = (char)toupper((unsigned char)argv[i][j]);
            ch[1] = '\0';
            prints(ch, WHITE);
        }
    }

    prints("\n", WHITE);
}

static void command_lower(int argc, char ** argv) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            prints(" ", WHITE);
        }

        for (int j = 0; argv[i][j] != '\0'; j++) {
            char ch[2];

            ch[0] = (char)lower((unsigned char)argv[i][j]);
            ch[1] = '\0';
            prints(ch, WHITE);
        }
    }

    prints("\n", WHITE);
}

static void command_reverse(int argc, char ** argv) {
    for (int i = argc - 1; i >= 1; i--) {
        int len = 0;

        while (argv[i][len] != '\0') {
            len++;
        }

        for (int j = len - 1; j >= 0; j--) {
            char ch[2] = {
                argv[i][j],
                '\0'
            };

            prints(ch, WHITE);
        }

        if (i > 1) {
            prints(" ", WHITE);
        }
    }

    prints("\n", WHITE);
}

static void command_len(int argc, char ** argv) {
    if (argc < 2 || argv == NULL || argv[1] == NULL) {
        prints("Usage: len <string>\n", WHITE);
        return;
    }

    usize len = strlen(argv[1]);
    char out[sizeof(usize) * 3 + 2];

    out[sizeof(out) - 1] = '\0';
    usize pos = sizeof(out) - 1;

    do {
        out[--pos] = (char)('0' + len % 10);
        len /= 10;
    } while (len != 0);

    prints("length of string: \n", WHITE);
    prints(out + pos, WHITE);

    prints("\n", WHITE);
}

static void command_colors(void) {
    prints("Base colors: \n", WHITE);
    prints("    |######| \n", LGRAY);
    prints("    |######| \n", RED);
    prints("    |######| \n", GREEN);
    prints("    |######| \n", YELLOW);
    prints("    |######| \n", BLUE);
    prints("    |######| \n", MAGENTA);
    prints("    |######| \n", CYAN);
    prints("    |######| \n", WHITE);

    prints("\n Additional colors: \n", WHITE);
    prints("    |######| \n", DGRAY);
    prints("    |######| \n", LRED);
    prints("    |######| \n", LGREEN);
    prints("    |######| \n", YELLOW);
    prints("    |######| \n", LBLUE);
    prints("    |######| \n", LMAGENTA);
    prints("    |######| \n", LCYAN);
}

static int read_double(const char ** pp, double * out) {
    const char * p = *pp;
    double value = 0.0;
    int negative = 0;
    int digits = 0;

    while (*p == ' ' || *p == '\t') {
        p++;
    }

    if (*p == '+' || *p == '-') {
        negative = (*p == '-');
        p++;
    }

    while (*p >= '0' && *p <= '9') {
        value = value * 10.0 + (double)(*p - '0');
        digits++;
        p++;
    }

    if (*p == '.') {
        double place = 0.1;

        p++;
        while (*p >= '0' && *p <= '9') {
            value += (double)(*p - '0') * place;
            place /= 10.0;
            digits++;
            p++;
        }
    }

    if (digits == 0) {
        return 0;
    }

    *out = negative ? -value : value;
    *pp = p;
    return 1;
}

static void print_double(double value) {
    long long scaled;

    if (value < 0.0) {
        prints("-", GREEN);
        value = -value;
    }

    scaled = (long long)(value * 100.0 + 0.5);
    printf("%lld.%02lld", scaled / 100, scaled % 100);
}

static void command_calc(void) {
    prints("[CALCULATOR]\n", BLUE);
    prints("usage: <number> <operation> <number>, e.g. 2 + 2\n", LGRAY);

    char line[MCL + 1];
    prints("matematix > ", WHITE);

    int len = console_readline(line, sizeof(line));
    if (len <= 0) {
        prints("[ERROR]: [Empty input]\n", RED);
        return;
    }

    const char * p = line;
    double first_num;
    double second_num;
    char operation;

    if (!read_double(&p, &first_num)) {
        prints("[ERROR]: [Wrong first number]\n", RED);
        return;
    }

    while (*p == ' ' || *p == '\t') {
        p++;
    }

    operation = *p;
    if (operation == '\0') {
        prints("[ERROR]: [Wrong symbol of operation]\n", RED);
        return;
    }
    p++;

    if (!read_double(&p, &second_num)) {
        prints("[ERROR]: [Wrong second number]\n", RED);
        return;
    }

    while (*p == ' ' || *p == '\t') {
        p++;
    }

    if (*p != '\0') {
        prints("[ERROR]: [Wrong input]\n", RED);
        return;
    }

    double result;

    switch (operation) {
        case '+':
            result = first_num + second_num;
        break;

        case '-':
            result = first_num - second_num;
        break;

        case '*':
            result = first_num * second_num;
        break;

        case '/':
            if (second_num == 0.0) {
                prints("[ERROR]: [Division by zero!]\n", RED);
                return;
            }

            result = first_num / second_num;
        break;

        default:
            prints("[ERROR]: [Wrong symbol of operation]\n", RED);
            return;
        break;
    }

    prints("[result]: ", GREEN);
    print_double(result);
    prints("\n", GREEN);
    prints("[CALCULATOR TERMINATED]\n", BLUE);
}

static void print_help(void) {
    prints("dltsh built-in commands:\n", WHITE);
    prints("  help        show this help\n", LGRAY);
    prints("  fetch       show system information\n", LGRAY);
    prints("  about       shell/kernel version\n", LGRAY);
    prints("  echo    ... print arguments\n", LGRAY);
    prints("  clear       clear the screen\n", LGRAY);
    prints("  reboot      restart the machine\n", LGRAY);
    prints("  poweroff    power the machine off\n", LGRAY);
    prints("  exit        leave the shell\n", LGRAY);
    prints("  calc        calculator\n", LGRAY);
    prints("  upper   ... converting text from lower to upper case\n", LGRAY);
    prints("  lower   ... converting text from upper to lower case\n", LGRAY);
    prints("  reverse ... works like echo, but the text is displayed upside down\n", LGRAY);
    prints("  colors      output all vga colors in tty\n", LGRAY);
    prints("  len     ... output length of string\n", LGRAY);
    prints("  dexide      minimal text editor\n", LGRAY);
    fs_shell_help();
}

static void print_about(void) {
    prints("dltsh - minimal OpenDelta shell\n", CYAN);
    prints("kernel: dltkernel v0.1.0-a\n", LGRAY);
    prints("build:  " __DATE__ " " __TIME__ "\n", LGRAY);
}

static int split_args(
    char * line,
    char ** argv,
    int max
) {
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
        else if (strcmp(argv[0], "calc") == 0) {
            command_calc();
        }
        else if (strcmp(argv[0], "colors") == 0) {
            command_colors();
        }
        else if (strcmp(argv[0], "echo") == 0) {
            command_echo(argc, argv);
        }
        else if (strcmp(argv[0], "dexide") == 0) {
            dexide_run(argc, argv);
        }
        else if (strcmp(argv[0], "len") == 0) {
            command_len(argc, argv);
        }
        else if (strcmp(argv[0], "reverse") == 0) {
            command_reverse(argc, argv);
        }
        else if (strcmp(argv[0], "upper") == 0) {
            command_upper(argc, argv);
        }
        else if (strcmp(argv[0], "lower") == 0) {
            command_lower(argc, argv);
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
        else if (fs_shell_execute(argc, argv)) {
            continue;
        }
        else {
            prints("[ERROR]: command not found: ", RED);
            prints(argv[0], RED);
            prints("\n", RED);
            continue;
        }
    }
}
