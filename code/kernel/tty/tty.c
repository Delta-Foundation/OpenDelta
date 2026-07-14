#include "../lib/types.h"
#include "../lib/string.h"
#include "../lib/tty.h"
#include "../lib/vga.h"

static const unsigned int VGA_WIDTH = 80;
static const unsigned int VGA_HEIGHT = 25;
static uint32_t* const VGA_MEM = (uint32_t*) 0xB8000;

static unsigned int terminal_row;
static unsigned int terminal_col;
static unsigned int terminal_color;
static unsigned int *terminal_buf;

void terminalInit(void) 
{
    terminal_row = 0;
    terminal_col = 0;
    terminal_color = VGAEntryColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buf = VGA_MEM;

    for (unsigned int y = 0; y < VGA_HEIGHT; y++) {
        for (unsigned int x = 0; x < VGA_WIDTH; x++) {
            const unsigned int index = y * VGA_WIDTH + x;
            terminal_buf[index] = VGAEntry(' ', terminal_color);
        }
    }
}

void terminalSetColor(uint8_t color) {
    terminal_color = color;
}

void terminalPutEntryAt(unsigned char c, uint8_t color, unsigned int x, unsigned int y) {
    const unsigned int index = y * VGA_WIDTH + x;
    terminal_buf[index] = VGAEntry(c, color);
}

void terminalPutChar(char c)
{
    unsigned char uc = c;
    terminalPutEntryAt(uc, terminal_color, terminal_col, terminal_row);
    if (++terminal_col == VGA_WIDTH) {
        terminal_col = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void terminalWrite(const char* data, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        terminalPutChar(data[i]);
    }
}

void terminalWriteString(const char* data) {
    terminalWrite(data, strlen(data));
}
