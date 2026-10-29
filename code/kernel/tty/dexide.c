#include "../libc/include/stdbase.h"
#include "../libc/include/string.h"
#include "../libc/include/types.h"
#include "../kernel-lib/screen.h"
#include "../kernel-lib/keyboard.h"
#include "../mem/header/memory.h"
#include "../tools/fat/headers/fat.h"
#include "./header/dexide.h"

#define DEXIDE_MAX_LINES    512
#define DEXIDE_MAX_LINE_LEN 255
#define DEXIDE_PATH_LEN     256

static char   dexide_lines[DEXIDE_MAX_LINES][DEXIDE_MAX_LINE_LEN + 1];
static uint32_t dexide_line_count = 0;
static char   dexide_file_name[DEXIDE_PATH_LEN];

static void dexide_logo(void) {
    prints("dddddd    eeeee  u   u  iii  dddddd   eeeee\n", MAGENTA);
    prints("d    d    e      u   u   i   d    d   e    \n", MAGENTA);
    prints("d    d    eeee     u     i   d    d   eeee \n", MAGENTA);
    prints("d    d    e      u   u   i   d    d   e    \n", MAGENTA);
    prints("dddddd    eeeee  u   u  iii  dddddd   eeeee\n", MAGENTA);
}

static void dexide_copy_str(char * dst, const char * src, uint32_t cap) {
    uint32_t i = 0;
    while (src[i] != '\0' && i + 1 < cap) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static void dexide_trim_newline(char * str) {
    uint32_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

static void dexide_print_uint(uint32_t value) {
    char buf[12];
    int pos = (int)sizeof(buf) - 1;
    buf[pos] = '\0';

    if (value == 0) {
        buf[--pos] = '0';
    }
    else {
        while (value > 0 && pos > 0) {
            buf[--pos] = (char)('0' + (value % 10));
            value /= 10;
        }
    }

    prints(&buf[pos], WHITE);
}

static boolean dexide_load_file(const char * path) {
    dexide_line_count = 0;

    if (!fat_exists(path)) {
        return TRUE;
    }

    if (fat_is_dir(path)) {
        prints("[ERROR]: [path is a directory]\n", RED);
        return FALSE;
    }

    fat_file_t * file = fat_open(path);
    if (file == NULL) {
        prints("[ERROR]: [cannot open file]\n", RED);
        return FALSE;
    }

    char buffer[129];
    uint32_t count;
    uint32_t line_pos = 0;

    while ((count = fat_read(file, sizeof(buffer) - 1, buffer)) > 0) {
        for (uint32_t i = 0; i < count; i++) {
            char ch = buffer[i];

            if (ch == '\r') {
                continue;
            }

            if (ch == '\n') {
                dexide_lines[dexide_line_count][line_pos] = '\0';
                dexide_line_count++;
                line_pos = 0;

                if (dexide_line_count >= DEXIDE_MAX_LINES) {
                    fat_close(file);
                    prints("[WARN]: [file truncated at max lines]\n", YELLOW);
                    return TRUE;
                }
                continue;
            }

            if (line_pos < DEXIDE_MAX_LINE_LEN) {
                dexide_lines[dexide_line_count][line_pos++] = ch;
            }
        }
    }

    if (line_pos > 0 && dexide_line_count < DEXIDE_MAX_LINES) {
        dexide_lines[dexide_line_count][line_pos] = '\0';
        dexide_line_count++;
    }

    fat_close(file);
    return TRUE;
}

static boolean dexide_save_file(const char * path) {
    static char out_buf[DEXIDE_MAX_LINES * (DEXIDE_MAX_LINE_LEN + 2)];
    uint32_t pos = 0;

    for (uint32_t i = 0; i < dexide_line_count; i++) {
        uint32_t len = strlen(dexide_lines[i]);
        for (uint32_t j = 0; j < len; j++) {
            if (pos >= sizeof(out_buf) - 1) {
                prints("[ERROR]: [buffer overflow]\n", RED);
                return FALSE;
            }
            out_buf[pos++] = dexide_lines[i][j];
        }
        if (pos >= sizeof(out_buf) - 1) {
            prints("[ERROR]: [buffer overflow]\n", RED);
            return FALSE;
        }
        out_buf[pos++] = '\n';
    }

    if (!fat_exists(path)) {
        if (!fat_create_file(path)) {
            prints("[ERROR]: [cannot create file]\n", RED);
            return FALSE;
        }
    }

    if (!fat_write_file(path, out_buf, pos)) {
        prints("[ERROR]: [cannot write file]\n", RED);
        return FALSE;
    }

    return TRUE;
}

static void dexide_display_lines(void) {
    if (dexide_line_count == 0) {
        prints("[info]: [buffer is empty]\n", LGRAY);
        return;
    }

    for (uint32_t i = 0; i < dexide_line_count; i++) {
        dexide_print_uint(i + 1);
        prints(" | ", LGRAY);
        prints(dexide_lines[i], WHITE);
        prints("\n", WHITE);
    }
}

static void dexide_mode_write(void) {
    prints("[MODE]: [Inputing text; input 'E' as a separate line to finish]\n", CYAN);

    char line[DEXIDE_MAX_LINE_LEN + 2];

    for (;;) {
        if (dexide_line_count >= DEXIDE_MAX_LINES) {
            prints("[WARN]: [max lines reached]\n", YELLOW);
            break;
        }

        dexide_print_uint(dexide_line_count + 1);
        prints(": ", WHITE);

        int len = console_readline(line, sizeof(line));
        if (len < 0) {
            break;
        }

        dexide_trim_newline(line);

        if (line[0] == 'E' && line[1] == '\0') {
            break;
        }

        dexide_copy_str(dexide_lines[dexide_line_count], line, DEXIDE_MAX_LINE_LEN + 1);
        dexide_line_count++;
    }
}

static void dexide_prompt_filename(void) {
    char input[DEXIDE_PATH_LEN];

    prints("input file name to edit: ", WHITE);
    int len = console_readline(input, sizeof(input));

    if (len <= 0) {
        dexide_file_name[0] = '\0';
        return;
    }

    dexide_trim_newline(input);
    dexide_copy_str(dexide_file_name, input, DEXIDE_PATH_LEN);
}

void dexide_run(int argc, char ** argv) {
    dexide_logo();
    prints("welcome to Dexide!\n", CYAN);

    dexide_line_count = 0;
    dexide_file_name[0] = '\0';

    if (argc >= 2) {
        dexide_copy_str(dexide_file_name, argv[1], DEXIDE_PATH_LEN);
    }
    else {
        dexide_prompt_filename();
    }

    if (dexide_file_name[0] == '\0') {
        prints("[ERROR]: [File name must not be empty!]\n", RED);
        return;
    }

    if (!dexide_load_file(dexide_file_name)) {
        return;
    }

    char cmd[16];

    for (;;) {
        prints("[w - write, r - read, s - save, q - save & quit, x - quit without save]\n", LGRAY);
        prints("dexide > ", WHITE);

        int len = console_readline(cmd, sizeof(cmd));
        if (len < 0) {
            dexide_save_file(dexide_file_name);
            break;
        }

        dexide_trim_newline(cmd);
        char c = cmd[0];

        if (c == 'w') {
            dexide_mode_write();
        }
        else if (c == 'r') {
            dexide_display_lines();
        }
        else if (c == 's') {
            if (dexide_save_file(dexide_file_name)) {
                prints("[OK]: [File Saved!]\n", GREEN);
            }
        }
        else if (c == 'q') {
            if (dexide_save_file(dexide_file_name)) {
                prints("[OK]: [File Saved!]\n", GREEN);
            }
            break;
        }
        else if (c == 'x') {
            prints("[INFO]: [exit without saving]\n", YELLOW);
            break;
        }
        else {
            prints("[ERROR]: [unknown command]\n", RED);
        }
    }
}
