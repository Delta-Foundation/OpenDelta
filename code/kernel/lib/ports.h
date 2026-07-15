#ifndef PORTS_H
#define PORTS_H

#include "../lib/types.h"

unsigned char portByteIn(unsigned short port);
void portByteOut(unsigned int port, unsigned char data);
unsigned short portWordIn(uint16_t port);
void portWordOut(uint16_t port, uint16_t data);
void APMInterfaceInit(void);

#endif 
