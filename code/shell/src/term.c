#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/unistd.h>
#include <sys/types.h>

#include "cmdlib/commands.h"
#include "cmdlib/simple_comms.h"
#include "cmdlib/files.h"
#include "cmdlib/editor.h"

#include "lib/dltsh.h"
#include "lib/colors.h"

/* constants */
#define MCL 255 // Максимальная длинна строки
#define MAX_PATH_LENGTH 128
#define MAX_USER_NAME_LENGTH 128
#define MAX_OS_TITLE_LANGTH 128
#define MAX_HOST_NAME_LENGTH 128

struct console {
    char command[MCL];
    unsigned int numberOfCommands;
    char *fileName;
    char *folderName[MAX_PATH_LENGTH];
    char *flag;
};

int main(int argc, char * argv[])
{
    set_keyword();
    set_symbols();
    set_values();
    set_types();

    char input[256];

    write_logo();
    welcome();

    while (true) {
        printf("> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        char *command  = strtok(input, " ");
        if (command == NULL) {
            continue;
        }

        char * arg1 = strtok(NULL, " ");
        char * arg2 = strtok(NULL, " ");

        /* Работа с файлами и папками. Удаление, добавление, переход и так далее... */
        if (strcmp(command, "touch") == 0) {
            if (arg1) {
                if (add_file(arg1)) {
                    printf(T_GREEN "[OK]: файл %s успешно создан!\n" T_RESET, arg1);
                }
                else {
                    perror(T_RED "Ошибка при создании файла\n" T_RESET);
                }
            }

            else {
                printf(T_RED "[ERROR]: укажите имя файла\n" T_RESET);
            }
        }

        else if (strcmp(command, "mkdir") == 0) {
            if (arg1 && add_dir(arg1)) {
                printf(T_GREEN "[OK]: Папка %s успешно создана!\n" T_RESET, arg1);
            }
            else {
                printf(T_RED "[ERROR]: ошибка при создании папки\n" T_RESET);
            }
        }

        else if (strcmp(command, "rm") == 0) {
            char * target = NULL;
            int is_dir = 0;

            if (arg1 && arg1[0] == '-') {
                if (strcmp(arg1, "-d") == 0) {
                    is_dir = 1;
                    target = arg2;
                }
                else if (strcmp(arg1, "-f") == 0) {
                    is_dir = 0;
                    target = arg2;
                }
            }
            else {
                target = arg1;
                is_dir = 0;
            }

            if (target && delete_dir_or_file(target, is_dir)) {
                printf(T_GREEN "[OK]: объект %s удалён\n" T_RESET, target);
            }
            else {
                perror(T_RED "[ERROR]: ошибка удаления\n" T_RESET);
            }
        }

        else if (strcmp(command, "cat") == 0) {
            if (arg1 != NULL) {
                if (display_file(arg1)) {
                    printf("содержимое файла %s: \n", arg1);
                }
                else {
                    perror("[ERROR]: ошибка открытия файла для просмотра содержимого");
                }
            }
            else {
                printf("[ERROR]: укажите имя файла\n");
            }
        }

        else if (strcmp(command, "cd") == 0) {
            char * path = arg1;

            if (path != NULL) {
                go_to_dir(path);
            }
            else {
                char * home = getenv("HOME");
                if (home && chdir(home) == 0) {
                    printf(T_GREEN "[OK]: переход в домашнюю директорию\n" T_RESET);
                }
                else {
                    printf(T_RED "[ERROR]: не удалось найти домашнюю папку" T_RESET);
                }

                // printf(T_RED "[ERROR]: укажите путь\n" T_RESET);
            }
        }

        else if (strcmp(command, "ls") == 0) {
            list_files();
        }

        else if (strcmp(command, "pwd") == 0) {
            show_this_dir();
        }

        /* Остальные команды */
        else if (strcmp(command, "calc") == 0) {
            calculator();
        }

        else if (strcmp(command, "dex") == 0) {
            editor();
        }

        else if (strcmp(command, "dlt-fetch") == 0) {
            print_fetch();
        }

        else if (strcmp(command, "ver") == 0) {
            shell_version();
        }

        else if (strcmp(command, "help") == 0) {
            system("~/OpenDelta/code/shell/bin/table");
        }

        else if (strcmp(command, "clear") == 0) {
            clear_screen();
        }

        else if (strcmp(command, "clocks") == 0) {
            system("~/OpenDelta/code/shell/bin/clocks");
        }

        else if (strcmp(command, "dexide") == 0) {
            clear_screen();
            system("~/OpenDelta/code/shell/bin/dexide");
        }

        else if (strcmp(command, "calcrs") == 0) {
            system("~/OpenDelta/code/shell/bin/calc");
        }

        else if (strcmp(command, "exit") == 0) {
            printf(T_GREEN "[завершение программы]\n" T_RESET);
            break;
        }

        else {
            printf(T_RED "[err]: [неизвестная команда!]\n" T_RESET);
        }
    }

    return 0;
}
