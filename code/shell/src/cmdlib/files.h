#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

#ifndef FILES_H
#define FILES_H

#define MFNL 256 //maximum file name length
#define MAX_TOKEN_LENGTH 1024
#define MAX_FOLDER_NAME_LENGTH 1024
#define MAX_LINES 512
#define MAX_LINE_LENGTH 1024
#define MAX_PATH_LENGTH 128

typedef struct {
    char file_name[MFNL];
    char token[MAX_TOKEN_LENGTH];
    char line[MAX_LINES];
    char folder_name[MAX_FOLDER_NAME_LENGTH];
    char cwd[MAX_FOLDER_NAME_LENGTH];
    FILE * file;
    int num_files;
    int is_created;
    int is_deleted;
    int is_displaying;
} file_explorer;

int add_file(const char fileName[MFNL]);
int add_dir(const char dirName[MAX_FOLDER_NAME_LENGTH]);
int delete(const char *target, const char *is_directory);
int display_file(const char *fileName);
void show_this_dir();
void go_to_dir(const char *path);
void list_files();

#endif
