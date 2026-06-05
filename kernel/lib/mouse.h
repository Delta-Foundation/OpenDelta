#ifndef DRVS_MOUSE_H
#define DRVS_MOUSE_H

#include "./types.h"
#include "./isr.h"

#define PACKETS_IN_PIPE 1024
#define DISCARD_POINT 32

#define MOUSE_MAGIC 0xFEED1234

#define MOUSE_IRQ 12 

#define MOUSE_PORT 0x60 
#define MOUSE_STATUS 0x64 
#define MOUSE_ABIT 0x02 
#define MOUSE_BBIT 0x01 
#define MOUSE_WRITE 0xD4 
#define MOUSE_F_BIT 0x20 
#define MOUSE_V_BIT 0x08

typedef enum {
    LEFT_CLICK = 0x01,
    RIGHT_CLICK = 0x02,
    MIDDLE_CLICK = 0x04
} mouse_click_t;

typedef struct {
    uint32_t magic;
    int8_t x_diff;
    int8_t y_diff;
    mouse_click_t buttons;
} mouse_device_packet_t;

void mouse_handler(isr_t *a_r);
void mouse_wait(uint8_t a_type);
void mouse_write(uint8_t a_write);
uint8_t mouse_read(void);
void mouse_install(void);

#endif 
