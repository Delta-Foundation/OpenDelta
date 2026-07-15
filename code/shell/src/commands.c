#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include "cmdlib/files.h"
#include "cmdlib/simple_comms.h"
#include "lib/colors.h"

/* constants */
#define MAX_LINES 512
#define MAX_LINE_LENGTH 1024
#define MAX_ARGUMENT_LENGTH 256
#define MAX_USER_NAME_LENGTH 128
#define MAX_OS_TITLE_LANGTH 128
#define MAX_HOST_NAME_LENGTH 128
#define MAX_QUANTITY_OF_COMMANDS 256

/* structures */
typedef struct {
    const char *user_name[MAX_USER_NAME_LENGTH];
    const char *os_title[MAX_OS_TITLE_LANGTH];
    const char *host_name[MAX_HOST_NAME_LENGTH];
    const char *cpu[128];
    const char *gpu[128];
    double disk_memory[32768];
} UserInfo;

typedef struct {
    int number_of_versions[16];
    const char *phoenix[128];
    const char *signalmann[128];
    const char *teemagnat[128];
    const char *hanz_hanz[128];
    const char *iwgnig[128];
    const char *neu_delta[128];
    const char *deltonium[128];
    const char *open_dlt_sh[128];    
    const char *v_0_0_12_c[128];
} Versions;

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
    .v_0_0_12_c = "v0.0.12-c"
};

#ifndef COMMANDS_H
#define COMMANDS_H

void your_version()
{
    Versions ver;
    *ver.number_of_versions = 8;
    const char *installed_ver[128];
    *installed_ver = "v0.0.12-c\n";
    printf(T_CYAN "[installed version dltsh]: %s" T_RESET, *installed_ver);
    *ver.v_0_0_12_c = "v0.0.12-c\n";
    printf(T_CYAN "[actualy dltsh version]: %s" T_RESET, *ver.v_0_0_12_c);
}

// for editor
void addFile(char fileName[MFNL])
{
    int isCreated = 0;
    FILE * file = fopen(fileName, "a");

    if (file == NULL) {
        printf(T_RED "[FAILED TO CREATE FILE!]\n" T_RESET);
        isCreated = 0;
    }
    else {
        fclose(file);
        isCreated = 1;
    }
}

void editorLogo()
{
    printf(T_MAGENTA "████████        ███████     █       █   ██  ████████        ███████\n" T_RESET);
    printf(T_MAGENTA "██     ██      ██     ██    ██     ██       ██     ██      ██     ██\n" T_RESET);
    printf(T_MAGENTA "██      ██    ██       ██    ██   ██    ██  ██      ██    ██       ██\n" T_RESET);
    printf(T_MAGENTA "██       ██  ██         ██    █████     ██  ██       ██  ██         ██\n" T_RESET);
    printf(T_MAGENTA "██       ██  █████████████   ██   ██    ██  ██       ██  █████████████\n" T_RESET);
    printf(T_MAGENTA "██      ██    ██            ██     ██   ██  ██      ██    ██\n" T_RESET);
    printf(T_MAGENTA "█████████      ██████████   █       █   ██  █████████      ██████████\n" T_RESET);
}

void editor()
{
    char lines[MAX_LINES][MAX_LINE_LENGTH];
   //char inputLine[MAX_LINE_LENGTH];
    char filename[100];
    char mode;
    int lineCount;
    FILE * file;

    system("clear");
    editorLogo();
    printf(T_BLUE "[🗒 текстовый редактор]\n" T_RESET);

    printf(T_CYAN "[Введите имя файла для редактирования]: " T_RESET);
    scanf("%s", filename);
    getchar();
    addFile(filename);

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, T_RED "[err]: [не удалось открыть файл: %s]\n" T_RESET, filename);
        return;
    }

    while (fgets(lines[lineCount], sizeof(lines[lineCount]), file) != NULL && lineCount < MAX_LINES) {
        lineCount++;
    }
    fclose(file);

    printf(T_CYAN "[нажмите 'w' для ввода текста, 'r' для чтения, 'q' для выхода]: \n" T_RESET);
    while ((mode = getchar()) != 'q') {
        if (mode == 'w') {
            printf(T_CYAN "[начните ввод (введите 'E' для выхода)]: \n" T_RESET);
            while (lineCount < MAX_LINES) {
                printf("%d: ", lineCount + 1);
                if (fgets(lines[lineCount], sizeof(lines[lineCount]), stdin) == NULL) {
                    fprintf(stderr, T_RED "[err]: [ошибка чтения строки.]\n" T_RESET);
                    break;
                }

                lines[lineCount][strcspn(lines[lineCount], "\n")] = 0;

                if (strlen(lines[lineCount]) == 0 || strcmp(lines[lineCount], "E") == 0) {
                    break;
                }

                strcpy(lines[lineCount], lines[lineCount]);
                lineCount++;

                if (lineCount >= MAX_LINES) {
                    printf(T_YELLOW "[warn]: [достигнуто максимальное количество строк. Завершите ввод]\n" T_RESET);
                    break;
                }
            }

            if (mode == 'r') {
                displayFile(filename);
            }
        }
    }

    file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, T_RED "[err]: [не удалось открыть файл для записи: %s]\n" T_RESET, filename);
        return;
    }

    for (int i = 0; i < lineCount; i++) {
        fprintf(file, "%s\n", lines[i]);
    }
    fclose(file);

    printf(T_CYAN "[Редактор завершил работу]\n" T_RESET);
    system("clear");
    welcome();
}

void print_fetch()
{
    UserInfo user = {
  	    .user_name = "User",
  	    .host_name = "Host",
  	    .os_title = "OpenDelta",
  	    .cpu = "cpu",
        .gpu = "ventilator 3000"
    };
  	
    printf(T_CYAN "        ____               User:        %s\n" T_RESET, *user.user_name);
    printf(T_CYAN "       /   /               ---------------------------\n" T_RESET);
    printf(T_CYAN "      /   / /\\             os:          %s\n" T_RESET, *user.os_title);
    printf(T_CYAN "     /   / /  \\            kernel:      Linux\n" T_RESET);
    printf(T_CYAN "    /   /  \\   \\           shell:       dltsh\n" T_RESET);
    printf(T_CYAN "   /   /    \\   \\          packeges:    fpkg\n" T_RESET);
    printf(T_CYAN "  /   /  /\\  \\   \\         terminal:    Terminal\n" T_RESET);
    printf(T_CYAN " /   /  /  \\  \\   \\        host:        %s\n" T_RESET, *user.host_name);
    printf(T_CYAN "/   /  /    \\  \\   \\       cpu:         %s\n" T_RESET, *user.cpu);
    printf(T_CYAN "\\__/  /______\\  \\__/       gpu:         %s\n" T_RESET, *user.gpu);
}

#endif

