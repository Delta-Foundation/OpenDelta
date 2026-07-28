#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#define KEYBOARD_PORT 0x60
#define KEYBOARD_DATA_PORT  		0x60
#define KEYBOARD_STATUS_PORT 		0x64
#define ENTER_KEY_CODE 			    0x1C
#define ENTER_BACKSPACE_CODE		0x0E

#define KEYBOARD_BUFFER_SIZE 256
#define INTERNAL_SPACE 0x01

#define KEY_LSHIFT 0x2A
#define KEY_RSHIFT 0x36
#define KEY_CAPSLOCK 0x3A

void keyboard_handler(void);

#endif
