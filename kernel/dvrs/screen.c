#include "../lib/ports.h"
#include "../lib/stdbase.h"
#include "../mem/header/memory.h"

int getOffset(int col, int row);
int getOffsetRow(int offset);
int getOffsetCol(int offset);
int getCursorOffset(void);
void setCursorOffset(int offset);

int printChar(char c, int col, int row, Colors color)
{
    unsigned char *vidmem = (unsigned char *)0xb8000;

    if (col >= COLUMNS_IN_LINE || row >= LINES) {
        vidmem[2 * (COLUMNS_IN_LINE) * (LINES) - 2] = 'E';
        vidmem[2 * (COLUMNS_IN_LINE) * (LINES) - 1] = color;
        return getOffset(col, row);
    }
   
    int offset;
    if (col >= 0 && row >= 0) {
        offset = getOffset(col, row);
    }
    else {
        offset = getCursorOffset();
    }

    if (c == '\n') {
        row = getOffsetRow(offset);
        offset = getOffset(0, row + 1);
    }
    else if (c == 0x08) {
        vidmem[offset] = ' ';
        vidmem[offset + 1] = color;
    }
    else {
        vidmem[offset] = c;
        vidmem[offset + 1] = color;
        offset += 2;
    }

    if (offset >= LINES * COLUMNS_IN_LINE * 2) {
        int i;
        for (i = 1; i < LINES; i++) {
           memmoryCopy((unsigned char*)(getOffset(0, i) + vidptr),
                (unsigned char*)(getOffset(0, i - 1) + vidptr),
                COLUMNS_IN_LINE * 2);
            
        }

        char *lastLine = (char *) (getOffset(0, LINES - 1) + (unsigned char*) vidptr);
        for (i = 0; i < COLUMNS_IN_LINE * 2; i++) {
            lastLine[i] = 0;
        }

        offset -= 2 * COLUMNS_IN_LINE;
    }

    setCursorOffset(offset);
    return offset;
}

void clearScreen(void)
{
    unsigned int i = 0;
    while (i < SCREENSIZE) {
        vidptr[i++] = ' ';
        vidptr[i++] = 0x07;
    }

    setCursorOffset(getOffset(0, 0));
}

int getOffset(int col, int row) {
    return 2 * (row * COLUMNS_IN_LINE + col);
}

int getOffsetRow(int offset) {
    return offset / (2 * COLUMNS_IN_LINE);
}

int getOffsetCol(int offset) {
    return (offset - (getOffsetRow(offset) * 2 * COLUMNS_IN_LINE)) / 2;
}

int getCursorOffset(void) {
    portByteOut(REG_SCREEN_CTRL, 14);
    int offset = portByteIn(REG_SCREEN_DATA) << 8;
    portByteOut(REG_SCREEN_CTRL, 15);
    offset += portByteIn(REG_SCREEN_DATA);
    return offset * 2;
}

void setCursorOffset(int offset) {
    offset /= 2;
    portByteOut(REG_SCREEN_CTRL, 14);
    portByteOut(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    portByteOut(REG_SCREEN_CTRL, 15);
    portByteOut(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

void delay() {
    for (unsigned int n = 0; n < 500000000; n++) { ; }
}
