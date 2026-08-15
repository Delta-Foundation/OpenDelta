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

int main(void)
{
    set_keyword();
    set_symbols();
    set_values();
    set_types();

    char input[256];

    welcome();

    while (true) {

        printf("> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        /* Variables */
        input[strcspn(input, "\0")] = 0;

        char *command  = strtok(input, " ");
        char *filename = strtok(NULL, " ");
        char *dirname  = strtok(NULL, " ");
        char *flag     = strtok(NULL, " ");
        char *target   = strtok(NULL, " ");
        
        int is_dir = 0;

        /* 
           +------------------+
           | Обработка команд |
           +------------------+
        */

        /* Работа с файлами и папками. Удаление, добавление, переход и так далее... */
        if (command != NULL && strcmp(command, "touch") == 0) {
            if (filename != NULL) {
                if (add_file(filename)) {
                    printf("[OK]: файл %s успешно создан!\n", filename);
                }
                else {
                    perror("Ошибка при создании файла");
                }
            }

            else {
                printf("[ERROR]: укажите имя файла\n");
            }
        }

        else if (command != NULL && strcmp(command, "mkdir") == 0) {
            if (dirname && add_dir(dirname)) {
                printf("[OK]: Папка %s успешно создана!\n", dirname);
            }
            else {
                printf("[ERROR]: ошибка при создании папки\n");
            }
        }  

        else if (command != NULL && strcmp(command, "rm") == 0) {
            if (flag != NULL && (flag[0] == '-')) {
                if (strcmp(flag, "-d") == 0) {
                    is_dir = 1;
                }
                else if (strcmp(flag, "-f") == 0) {
                    is_dir = 0;
                }

                target = strtok(NULL, " ");
            }
            else {
                target = flag;
                is_dir = 0;
            }

            if (target && delete(target, (const char *)is_dir)) {
                printf("[OK]: объект %s успешно удалён\n", target);
            }
            else {
                perror("[ERROR]: ошибка при удалении объекта");
            }
        }    

        else if (command != NULL && strcmp(command, "cat") == 0) {
            if (filename != NULL) {
                if (display_file(filename)) {
                    printf("содержимое файла %s: \n");
                }
                else {
                    perror("[ERROR]: ошибка открытия файла для просмотра содержимого");
                }
            }
            else {
                printf("[ERROR]: укажите имя файла\n");
            }
        }

        else if (command != NULL && strcmp(command, "ls") == 0) {
            list_files();
        }

        else if (command != NULL && strcmp(command, "pwd") == 0) {
            show_this_dir();
        }

        /* Остальные команды */
        else if (command != NULL && strcmp(command, "calc") == 0) {
            calculator();
        }

        else if (command != NULL && strcmp(command, "dex") == 0) {
            editor();
        }

        else if (command != NULL && strcmp(command, "dlt-fetch") == 0) {
            print_fetch();
        }

        else if (command != NULL && strcmp(command, "ver") == 0) {
            shell_version();
        }

        else if (command != NULL && strcmp(command, "help") == 0) {
            system("~/open-delta/code/shell/bin/table");
        }

        else if (command != NULL && strcmp(command, "clear") == 0) {
            clear_screen();
        }

        else if (command != NULL && strcmp(command, "clocks") == 0) {
            system("~/open-delta/code/shell/bin/clocks");
        }
 
        else if (command != NULL && strcmp(command, "dexide") == 0) {
            clear_screen();
            system("~/open-delta/code/shell/bin/dexide");
        }
 
        else if (command != NULL && strcmp(command, "exit") == 0) {
            printf(T_GREEN "[завершение программы]\n" T_RESET);
            break;
        }

        else {
            printf(T_RED "[err]: [неизвестная команда!]\n" T_RESET);
        }

        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }

    return 0;
}
