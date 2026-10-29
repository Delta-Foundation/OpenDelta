#include "../libc/include/stdbase.h"
#include "../libc/include/string.h"
#include "../kernel-lib/screen.h"
#include "../tools/fat/headers/fat.h"
#include "./header/fs_shell.h"

static char fs_cwd[FAT_MAX_PATH] = "/";

static void fs_copy_string(
    char * destination,
    const char * source,
    uint32_t capacity
) {
    uint32_t i = 0;

    while (source[i] != '\0' && i + 1 < capacity) {
        destination[i] = source[i];
        i++;
    }

    destination[i] = '\0';
}

static void fs_normalize_path(
    const char * input,
    char * output
) {
    char work[FAT_MAX_PATH];
    char result[FAT_MAX_PATH];

    fs_copy_string(work, input, FAT_MAX_PATH);
    fs_copy_string(result, "/", FAT_MAX_PATH);

    char * p = work;
    while (*p != '\0') {
        while (*p == '/') {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        char * start = p;
        while (*p != '\0' && *p != '/') {
            p++;
        }

        uint32_t length = (uint32_t)(p - start);
        char component[FAT_MAX_PATH];
        uint32_t i = 0;

        while (i < length && i + 1 < FAT_MAX_PATH) {
            component[i] = start[i];
            i++;
        }
        component[i] = '\0';

        if (strcmp(component, ".") == 0) {
            continue;
        }

        if (strcmp(component, "..") == 0) {
            uint32_t size = strlen(result);

            if (size > 1) {
                if (result[size - 1] == '/') {
                    size--;
                }

                while (size > 1 && result[size - 1] != '/') {
                    size--;
                }

                if (size > 1) {
                    size--;
                }

                result[size] = '\0';
                if (size == 0) {
                    fs_copy_string(result, "/", FAT_MAX_PATH);
                }
            }

            continue;
        }

        uint32_t size = strlen(result);
        if (size > 1) {
            result[size++] = '/';
            result[size] = '\0';
        }

        uint32_t j = 0;
        while (component[j] != '\0' && size + 1 < FAT_MAX_PATH) {
            result[size++] = component[j++];
        }
        result[size] = '\0';
    }

    fs_copy_string(output, result, FAT_MAX_PATH);
}

static void fs_make_path(
    const char * argument,
    char * output
) {
    char raw[FAT_MAX_PATH];

    if (argument == NULL || argument[0] == '\0') {
        fs_copy_string(output, fs_cwd, FAT_MAX_PATH);
        return;
    }

    if (argument[0] == '/') {
        fs_copy_string(raw, argument, FAT_MAX_PATH);
    }
    else {
        fs_copy_string(raw, fs_cwd, FAT_MAX_PATH);
        uint32_t size = strlen(raw);

        if (size > 1) {
            raw[size++] = '/';
            raw[size] = '\0';
        }

        uint32_t i = 0;
        while (argument[i] != '\0' && size + 1 < FAT_MAX_PATH) {
            raw[size++] = argument[i++];
        }
        raw[size] = '\0';
    }

    fs_normalize_path(raw, output);
}

static void fs_error(const char * message, const char * path) {
    prints("[ERROR]: ", RED);
    prints("%s", RED, message);

    if (path != NULL) {
        prints(" '", RED);
        prints("%s", RED, path);
        prints("'", RED);
    }

    prints("\n", RED);
}

static boolean fs_cmd_pwd(int argc, char ** argv) {
    (void)argc;
    (void)argv;

    prints("%s\n", WHITE, fs_cwd);
    return TRUE;
}

static boolean fs_cmd_cd(int argc, char ** argv) {
    char path[FAT_MAX_PATH];

    if (argc < 2) {
        fs_copy_string(path, "/", FAT_MAX_PATH);
    }
    else {
        fs_make_path(argv[1], path);
    }

    if (!fat_is_dir(path)) {
        fs_error("not a directory:", path);
        return TRUE;
    }

    fs_copy_string(fs_cwd, path, FAT_MAX_PATH);
    return TRUE;
}

static boolean fs_list_entry(
    const char * name,
    boolean is_dir,
    uint32_t size,
    void * context
) {
    (void)size;
    (void)context;

    prints("%s%s\n", is_dir ? LCYAN : WHITE, name, is_dir ? "/" : "");
    return TRUE;
}

static boolean fs_cmd_ls(int argc, char ** argv) {
    char path[FAT_MAX_PATH];
    fs_make_path(argc > 1 ? argv[1] : NULL, path);

    if (!fat_mounted()) {
        fs_error("filesystem is not mounted", NULL);
        return TRUE;
    }

    if (!fat_list_dir(path, fs_list_entry, NULL)) {
        fs_error("cannot list directory:", path);
    }

    return TRUE;
}

static boolean fs_cmd_cat(int argc, char ** argv) {
    if (argc < 2) {
        fs_error("usage: cat <file>", NULL);
        return TRUE;
    }

    char path[FAT_MAX_PATH];
    fs_make_path(argv[1], path);

    if (!fat_exists(path)) {
        fs_error("no such file:", path);
        return TRUE;
    }

    if (fat_is_dir(path)) {
        fs_error("is a directory:", path);
        return TRUE;
    }

    fat_file_t * file = fat_open(path);
    if (file == NULL) {
        fs_error("cannot open file:", path);
        return TRUE;
    }

    char buffer[129];
    uint32_t count;

    while ((count = fat_read(file, sizeof(buffer) - 1, buffer)) > 0) {
        buffer[count] = '\0';
        prints("%s", WHITE, buffer);
    }

    fat_close(file);
    prints("\n", WHITE);
    return TRUE;
}

static boolean fs_cmd_touch(int argc, char ** argv) {
    if (argc < 2) {
        fs_error("usage: touch <file>", NULL);
        return TRUE;
    }

    char path[FAT_MAX_PATH];
    fs_make_path(argv[1], path);

    if (fat_exists(path)) {
        if (fat_is_dir(path)) {
            fs_error("is a directory:", path);
        }

        return TRUE;
    }

    if (!fat_create_file(path)) {
        fs_error("cannot create file (use an 8.3 name):", path);
    }

    return TRUE;
}

static boolean fs_cmd_mkdir(int argc, char ** argv) {
    if (argc < 2) {
        fs_error("usage: mkdir <directory>", NULL);
        return TRUE;
    }

    char path[FAT_MAX_PATH];
    fs_make_path(argv[1], path);

    if (fat_exists(path)) {
        fs_error("already exists:", path);
        return TRUE;
    }

    if (!fat_create_dir(path)) {
        fs_error("cannot create directory (use an 8.3 name):", path);
    }

    return TRUE;
}

static boolean fs_cmd_rmdir(int argc, char ** argv) {
    if (argc < 2) {
        fs_error("usage: rmdir <directory>", NULL);
        return TRUE;
    }

    char path[FAT_MAX_PATH];
    fs_make_path(argv[1], path);

    if (!fat_exists(path)) {
        fs_error("no such directory:", path);
        return TRUE;
    }

    if (!fat_is_dir(path)) {
        fs_error("not a directory:", path);
        return TRUE;
    }

    if (!fat_delete_dir(path)) {
        fs_error("directory is not empty:", path);
    }

    return TRUE;
}

static boolean fs_cmd_rmfile(int argc, char ** argv) {
    if (argc < 2) {
        fs_error("usage: rmfile <file>", NULL);
        return TRUE;
    }

    char path[FAT_MAX_PATH];
    fs_make_path(argv[1], path);

    if (!fat_exists(path)) {
        fs_error("no such file:", path);
        return TRUE;
    }

    if (fat_is_dir(path)) {
        fs_error("is a directory:", path);
        return TRUE;
    }

    if (!fat_delete_file(path)) {
        fs_error("cannot remove file:", path);
    }

    return TRUE;
}

void fs_shell_help(void) {
    prints("  pwd         print current directory\n", LGRAY);
    prints("  cd      ... change directory\n", LGRAY);
    prints("  ls      ... list directory\n", LGRAY);
    prints("  cat     ... print a file\n", LGRAY);
    prints("  touch   ... create an empty file\n", LGRAY);
    prints("  mkdir   ... create a directory\n", LGRAY);
    prints("  rmdir   ... remove an empty directory\n", LGRAY);
    prints("  rmfile  ... remove a file\n", LGRAY);
}

boolean fs_shell_execute(
    int argc,
    char ** argv
) {
    if (argc == 0 || argv == NULL) {
        return FALSE;
    }

    const char * command = argv[0];

    if (strcmp(command, "pwd") == 0) {
        return fs_cmd_pwd(argc, argv);
    }

    if (strcmp(command, "cd") == 0) {
        return fs_cmd_cd(argc, argv);
    }

    if (strcmp(command, "ls") == 0) {
        return fs_cmd_ls(argc, argv);
    }

    if (strcmp(command, "cat") == 0) {
        return fs_cmd_cat(argc, argv);
    }

    if (strcmp(command, "touch") == 0) {
        return fs_cmd_touch(argc, argv);
    }

    if (strcmp(command, "mkdir") == 0) {
        return fs_cmd_mkdir(argc, argv);
    }

    if (strcmp(command, "rmdir") == 0) {
        return fs_cmd_rmdir(argc, argv);
    }

    if (strcmp(command, "rmfile") == 0) {
        return fs_cmd_rmfile(argc, argv);
    }

    return FALSE;
}
