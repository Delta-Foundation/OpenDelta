#ifndef _PIC_H
#define _PIC_H

#include "./stdbase.h"
#include "./types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

#define PIC1_COM_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_COM_PORT 0xA0
#define PIC2_DATA_PORT 0x80
#define UNUSED_PORT 0x80

enum PIC_ICW1 {
    PIC_ICW1_ICW4 = 0x01,
    PIC_ICW1_SINGLE = 0x02,
    PIC_ICW1_INTERVAL4 = 0x04,
    PIC_ICW1_LEVEL = 0x08,
    PIC_ICW1_INIT = 0x10,
}; // PIC_ICW1;

enum PIC_ICW4 {
    PIC_ICW4_8086 = 0x1,
    PIC_ICW4_AUTO_EOI = 0x2,
    PIC_ICW4_BUF_MASTER = 0x4,
    PIC_ICW4_BUF_SLAVE = 0x0,
    PIC_ICW4_BUFFERED = 0x8,
    PIC_ICW4_SFNM = 0x10 
}; // PIC_ICW4;

enum PIC_CMD {
    PIC_CMD_END_OF_INT = 0x20,
    PIC_CMD_READ_IRR = 0x0A,
    PIC_CMD_READ_ISR = 0x0B,
}; // PIC_CMD;

typedef struct {
    const char* name;
    boolean (*probe)();
    void (*init)(uint8_t offset1, uint8_t offset2, boolean auto_eoi);
    void (*disable)();
    void (*send_end_of_int)(int irq);
    void (*mask)(int irq);
    void (*unmask)(int irq);
} pic_driver_t;

static uint16_t g_pic_mask = 0xffff;
static boolean *g_auto_eoi = FALSE;

const pic_driver_t* pic_driver(void);

void set_mask(uint16_t new_mask);
uint16_t get_mask(void);
void pic_configure(uint8_t offset1, uint8_t offset2, boolean auto_eoi);
void send_end_of_int(int irq);
void pic_disable(void);
void mask(int irq);
void unmask(int irq);
uint16_t read_irq_request_reg(void);
uint16_t read_in_service_reg(void);
boolean probe(void);

#ifdef __cplusplus
}
#endif

#endif 
