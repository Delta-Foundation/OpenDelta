#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "lib/colors.h"
#include "cmdlib/files.h"

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
    #define chdir _chdir
    #define getcwd _getcwd
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <stdlib.h>
#endif

int add_file(const char fileName[MFNL])
{
    FILE * file = fopen(fileName, "w");
    
    if (file == NULL) {
        perror("[ERROR]: ошибка при создании файла");
        return 0;
    }

    fclose(file);
    return 1;
}

int add_dir(const char dirName[MAX_FOLDER_NAME_LENGTH])
{
    if (mkdir(dirName, 0777) == -1) {
        perror("[ERROR]: ошибка при создании папки");
        return 0;
    }

    return 1;
}

int delete(const char *target, const char *is_directory) 
{
    if (is_directory) {
        return rmdir(target) == 0;
    }
    else {
        return remove(target) == 0;
    }
}

int display_file(const char *file_name)
{
    char line[MAX_LINES];
    FILE * file = fopen(file_name, "r");
    
    if (file == NULL) {
        printf(T_RED "[ошибка открытия файла для чтения: %s]\n" T_RESET, file_name);
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        fputs(line, stdout);
    }

    fclose(file);
    return 1;
}

void go_to_dir(const char *path)
{
    if (path == NULL) {
        fprintf(stderr, T_RED "[err]: [пустой путь]\n" T_RESET);
        return;
    }

    if (chdir(path) == 0) {
        printf(T_GREEN "[Успешно перешли в директорию]: %s\n" T_RESET, path);
    }
    else {
        perror("Ошибка при переходе в директорию");
    }
}

void show_this_dir()
{
    file_explorer var = {0};

    if (getcwd(var.cwd, sizeof(var.cwd)) != NULL) {
        printf(T_CYAN "[текущая папка: '%s']\n" T_RESET, var.cwd);
    }
    else {
        printf(T_RED "[err]: [удалось определить путь к папке]\n" T_RESET);
    }
}

void list_files() 
{
    struct dirent *dirent;
    DIR *dir = opendir(".");

    if (dir == NULL) {
        printf("Не удалось открыть текущую директорию.\n");
        return;
    }

    printf(T_CYAN "[Файлы в текущей директории]:\n" T_RESET);
    while ((dirent = readdir(dir)) != NULL) {

        if (dirent->d_name[0] != '.') {
            printf("%s\n", dirent->d_name);
        }
    }

    closedir(dir);
}
