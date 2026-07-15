#ifndef ARCH_I386_VGA_H
#define ARCH_I386_VGA_H

#include "./types.h"

enum VGAColor {
	VGA_COLOR_BLACK = 0x0,
	VGA_COLOR_BLUE = 0x1,
	VGA_COLOR_GREEN = 0x2,
	VGA_COLOR_CYAN = 0x3,
	VGA_COLOR_RED = 0x4,
	VGA_COLOR_MAGENTA = 0x5,
	VGA_COLOR_BROWN = 0x6,
	VGA_COLOR_LIGHT_GREY = 0x7,
	VGA_COLOR_DARK_GREY = 0x8,
	VGA_COLOR_LIGHT_BLUE = 0x9,
	VGA_COLOR_LIGHT_GREEN = 0x10,
	VGA_COLOR_LIGHT_CYAN = 0x11,
	VGA_COLOR_LIGHT_RED = 0x12,
	VGA_COLOR_LIGHT_MAGENTA = 0x13,
	VGA_COLOR_LIGHT_BROWN = 0x14,
	VGA_COLOR_WHITE = 0x15,
};

static inline uint8_t VGAEntryColor(enum VGAColor fg, enum VGAColor bg) {
	return fg | bg << 4;
}

static inline uint16_t VGAEntry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

#endif
