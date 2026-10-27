#include "../libc/include/types.h"
#include "../libc/include/string.h"
#include "./header/tty.h"
#include "../kernel-lib/screen.h"
#include "../kernel-lib/vga.h"

void terminal_init(void) {
    console_init();
}

void terminal_set_color(uint8_t color) {
    console_set_color(color);
}

void terminal_put_char(char c) {
    console_put_char(c);
}

void terminal_write(
    const char * data,
    unsigned int size
) {
    for (unsigned int i = 0; i < size; i++) {
        console_put_char(data[i]);
    }
}

void terminal_write_string(const char * data) {
    console_write(data);
}
