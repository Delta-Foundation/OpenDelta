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
#define MAX_FOLDER_LENGTH 128
#define MAX_PATH_LENGTH 128

typedef struct {
    char fileName[MFNL];
    char token[MAX_TOKEN_LENGTH];
    char line[MAX_LINES];
    char folderName[MAX_FOLDER_LENGTH];
    char cwd[MAX_FOLDER_NAME_LENGTH];
    FILE * file;
    int numFiles;
    int isCreated;
    int isDeleted;
    int isDisplaying;
} file_explorer;

void add_file(char fileName[MFNL]);
void add_dir();
void del();
void _remove(const char *flag, const char *name);
void displayFile(const char *fileName);
void showThisDir();
void goToDir(const char *path);
void list();

#endif

