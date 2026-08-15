#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_USER_NAME_LENGTH 128
#define MAX_OS_TITLE_LANGTH 128
#define MAX_HOST_NAME_LENGTH 128
#define MAX_QUANTITY_OF_COMMANDS 256

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
    const char *v_0_0_14_d[128];
} Versions;

void shell_version();
void clear_screen();
void calculator();
void editor();
void print_fetch();

#endif
