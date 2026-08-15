#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/unistd.h>

#include "cmdlib/commands.h"
#include "cmdlib/files.h"
#include "cmdlib/simple_comms.h"
#include "lib/colors.h"

// !!realizations for editor!!
static void editor_add_file(char filename[MFNL]) {
    int is_created = 0; 
    FILE * file = fopen(filename, "a");

    if (file == NULL) {
        printf(T_RED "[FAILED TO CREATE FILE!]\n" T_RESET);
        is_created = 0;
    }
    else {
        fclose(file);
        is_created = 1;
    }
}

static void editor_display_file(const char *filename) {
    file_explorer var;
    var.file = NULL;
    var.is_displaying = 0;

    printf(T_CYAN "[введите имя файла]: " T_RESET);
    if (scanf("%511s", filename) != 1) {
        printf(T_RED "[ошибка чтения имени файла]\n" T_RESET);
        return;
    }

    var.file = fopen(filename, "r");
    if (var.file == NULL) {
        printf(T_RED "[ошибка открытия файла для чтения: %s]\n" T_RESET, filename);
        return;
    }

    var.is_displaying = 1;
    while (fgets(var.line, sizeof(var.line), var.file) != NULL) {
        fputs(var.line, stdout);
    }

    fclose(var.file);
    var.is_displaying = 0;
}

static void editor_logo() {
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
    int line_count;
    FILE * file;

    clear_screen();
    editor_logo();
    printf(T_BLUE "[текстовый редактор]\n" T_RESET);

    printf(T_CYAN "[Введите имя файла для редактирования]: " T_RESET);
    scanf("%s", filename);
    getchar();
    editor_add_file(filename);

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, T_RED "[err]: [не удалось открыть файл: %s]\n" T_RESET, filename);
        return;
    }

    while (fgets(lines[line_count], sizeof(lines[line_count]), file) != NULL && line_count < MAX_LINES) {
        line_count++;
    }
    fclose(file);

    printf(T_CYAN "[нажмите 'w' для ввода текста, 'r' для чтения, 'q' для выхода]: \n" T_RESET);
    while ((mode = getchar()) != 'q') {
        if (mode == 'w') {
            
            printf(T_CYAN "[начните ввод (введите 'Q' для выхода)]: \n" T_RESET);
            
            while (line_count < MAX_LINES) {
                printf("%d: ", line_count + 1);

                if (fgets(lines[line_count], sizeof(lines[line_count]), stdin) == NULL) {
                    fprintf(stderr, T_RED "[err]: [ошибка чтения строки.]\n" T_RESET);
                    break;
                }

                lines[line_count][strcspn(lines[line_count], "\n")] = 0;

                if (strlen(lines[line_count]) == 0 || strcmp(lines[line_count], "Q") == 0) {
                    break;
                }

                strcpy(lines[line_count], lines[line_count]);
                line_count++;

                if (line_count >= MAX_LINES) {
                    printf(T_YELLOW "[warn]: [достигнуто максимальное количество строк. Завершите ввод]\n" T_RESET);
                    break;
                }
            }

            if (mode == 'r') {
                editor_display_file(filename);
            }
        }
    }

    file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, T_RED "[err]: [не удалось открыть файл для записи: %s]\n" T_RESET, filename);
        return;
    }

    for (int i = 0; i < line_count; i++) {
        fprintf(file, "%s\n", lines[i]);
    }
    fclose(file);

    printf(T_CYAN "[Редактор завершил работу]\n" T_RESET);
    clear_screen();
    welcome();
}


