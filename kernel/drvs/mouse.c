/*
*   Mouse driver for dltkernel
*/

#include "../lib/isr.h"
#include "../fs/headers/fs.h"
#include "../fs/headers/pipe.h"
#include "../lib/ports.h"
#include "../lib/mouse.h"
#include "../lib/screen.h"

uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];
int8_t mouse_x = 0;
int8_t mouse_y = 0;

fs_node_t *mouse_pipe;

void mouse_handler(isr_t *a_r) 
{
    switch (mouse_cycle) {
        case 0:
            mouse_byte[0] = portByteIn(MOUSE_PORT);
            mouse_cycle++;
        break;
        
        case 1:
            mouse_byte[1] = portByteIn(MOUSE_PORT);
            mouse_cycle++;
        break; 

        case 2:
           mouse_byte[2] = portByteIn(MOUSE_PORT);

            if (mouse_byte[0] & 0x80 || mouse_byte[0] & 0x40) {
                break;
            }

            mouse_device_packet_t packet;
			packet.magic = MOUSE_MAGIC;
			packet.x_diff = mouse_byte[1];
			packet.y_diff = mouse_byte[2];
			packet.buttons = 0;

            if(mouse_byte[0] & 0x01) {
				packet.buttons |= LEFT_CLICK;
			}

			if(mouse_byte[0] & 0x02) {
				packet.buttons |= RIGHT_CLICK;
			}

			if(mouse_byte[0] & 0x04) {
				packet.buttons |= MIDDLE_CLICK;
			}

			mouse_x = mouse_byte[1];
			mouse_y = mouse_byte[2];
			mouse_cycle = 0;

			mouse_device_packet_t bitbucket;

            while (pipeSize(mouse_pipe) > (DISCARD_POINT * sizeof(packet))) {
                readFileSys(mouse_pipe, 0, sizeof(packet), (uint8_t *)&packet);
            }
        
            writeFileSys(mouse_pipe, 0, sizeof(packet), (uint8_t *)&packet);
        break;
    } 
}

void mouse_wait(uint8_t a_type) 
{
    uint32_t _time_out = 100000;

    if (a_type == 0) {
        while (_time_out--) {
            if ((portByteIn(MOUSE_STATUS) & 1) == 1) {
                return;
            }
        }
        
        return;
    }
    else {
        while (_time_out--) {
            if ((portByteIn(MOUSE_STATUS) & 2) == 0) {
                return; 
            }
        }

        return;
    }
}

void mouse_write(uint8_t a_write) {
    mouse_wait(1);
    portByteOut(MOUSE_STATUS, MOUSE_WRITE);
    mouse_wait(1);
    portByteOut(MOUSE_PORT, a_write);
}

uint8_t mouse_read(void) {
    mouse_wait(0);
    return portByteIn(MOUSE_PORT);
}

void mouse_install(void) {
    uint8_t _status;

    mouse_pipe = makePipe(sizeof(mouse_device_packet_t) * PACKETS_IN_PIPE);

    mouse_wait(1);
    portByteOut(MOUSE_STATUS, 0xA8);

    mouse_wait(1);
    portByteOut(MOUSE_STATUS, MOUSE_F_BIT);
    mouse_wait(0);
    _status = (portByteIn(MOUSE_PORT) | 2);
    mouse_wait(1);
    portByteOut(MOUSE_PORT, _status);

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();

    register_interrupt_handler(MOUSE_IRQ, (isr_t)mouse_handler);

    kprint("\n[DRVS INFO]: [mouse driver initialized]\n", LCYAN);
}
