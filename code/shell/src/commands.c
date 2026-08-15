#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/unistd.h>

#include "cmdlib/commands.h"
#include "lib/colors.h"

Versions versions = {
    .number_of_versions = 16,
    .phoenix = "phoenix",
    .signalmann = "signalmann",
    .teemagnat = "TeeMagnt",
    .hanz_hanz = "HanzHanz",
    .iwgnig = "Iwgnig",
    .neu_delta = "Neu Delta",
    .deltonium = "Deltonium",
    .open_dlt_sh = "OpenDltSH",
    .v_0_0_14_d = "v0.0.14-d"
};

void shell_version() {
    Versions ver;
    *ver.number_of_versions = 8;
    const char *installed_ver[128];
    
    *installed_ver = "v0.0.14-d\n";    
    printf(T_CYAN "[installed version dltsh]: %s" T_RESET, *installed_ver);
   
    *ver.v_0_0_14_d = "v0.0.14-d\n";
    printf(T_CYAN "[actualy dltsh version]: %s" T_RESET, *ver.v_0_0_14_d);
}

void clear_screen() {
    const char *clear_screen_ansi = "\e[1;1H\e[2J";
    write(STDOUT_FILENO, clear_screen_ansi, 11);
}

void calculator() {
    printf(T_BLUE "[калькулятор]\n" T_RESET);
    double FRST_NUMBER;
    double SCND_NUMBER;
    char SYMBOL;

    int inputResult = scanf("%lf\n %c\n %lf", &FRST_NUMBER, &SYMBOL, &SCND_NUMBER);
    if (inputResult != 3) {
        fprintf(stderr, T_RED "[ERR]: [Неверный ввод]\n" T_RESET);
        while (getchar() != '\n');
    }

    double RES;

    switch (SYMBOL) {
        case '+':
            RES = FRST_NUMBER + SCND_NUMBER;
            break;
        case '-':
            RES = FRST_NUMBER - SCND_NUMBER;
            break;
        case '*':
            RES = FRST_NUMBER * SCND_NUMBER;
            break;
        case '/':
            if (SCND_NUMBER == 0) {
                fprintf(stderr, T_RED "[ERR]: [Деление на ноль!]\n" T_RESET);
                while (getchar() != '\n');
            }
            RES = FRST_NUMBER / SCND_NUMBER;
            break;
        default:
            fprintf(stderr, T_RED "[ERR]: [Неверный символ операции!]\n" T_RESET);
            while (getchar() != '\n');
    }

    printf(T_GREEN "[результат]: " T_RESET);
    printf("%.2lf \n", RES);
    printf(T_CYAN "[калькулятор завершил работу]\n" T_RESET);
}

void print_fetch() {
    UserInfo user = {
  	    .user_name = "User",
  	    .host_name = "Host",
  	    .os_title = "OpenDelta",
  	    .cpu = "potato-cpu-gigachad",
        .gpu = "ventilator 3000"
    };

    printf(T_CYAN "        ____               User:        %s\n" T_RESET, *user.user_name);
    printf(T_CYAN "       /   /               ---------------------------\n" T_RESET);
    printf(T_CYAN "      /   / /\\             os:          %s\n" T_RESET, *user.os_title);
    printf(T_CYAN "     /   / /  \\            kernel:      dltkernel\n" T_RESET);
    printf(T_CYAN "    /   /  \\   \\           shell:       dltsh\n" T_RESET);
    printf(T_CYAN "   /   /    \\   \\          packeges:    n fpmcp\n" T_RESET);
    printf(T_CYAN "  /   /  /\\  \\   \\         terminal:    Terminal\n" T_RESET);
    printf(T_CYAN " /   /  /  \\  \\   \\        host:        %s\n" T_RESET, *user.host_name);
    printf(T_CYAN "/   /  /    \\  \\   \\       cpu:         %s\n" T_RESET, *user.cpu);
    printf(T_CYAN "\\__/  /______\\  \\__/       gpu:         %s\n" T_RESET, *user.gpu);
}
