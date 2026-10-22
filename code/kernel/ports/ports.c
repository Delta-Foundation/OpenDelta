#include "../kernel-lib/ports.h"
#include "../kernel-lib/screen.h"
#include "../libc/include/stdbase.h"  

unsigned char port_byte_in(unsigned short port)
{
	unsigned char result;
	
	__asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

void port_byte_out(unsigned int port, unsigned char data)
{
	__asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned short port_word_in(uint16_t port)
{
	uint16_t result;
	__asm__ ("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

void port_word_out(uint16_t port, uint16_t data)
{
	__asm__ volatile("out %%ax, %%dx" : : "a" (data), "d" (port));
}

void APM_interface_init(void)
{
	__asm__ (
		"mov $0x53, %ah\n\t" 		// Check if APM is supported
		"mov $0x00, %ah\n\t"       
		"xor %bx, %bx\n\t"          
		"int $0x15\n\t"
		"jc  APM_Error\n\t"
		"mov $0x53, %ah\n\t"     	// Disconnect to any existing interface          
		"mov $0x04, %al\n\t"             
		"xor %bx, %bx\n\t"               
		"int $0x15\n\t"
		"jc  Disconnect_Error\n\t"
		"cmp $0x03, %ah\n\t"
		"jne APM_Error\n\t"
		"mov $0x53, %ah\n\t"          	// Connect to real mode interface
		"mov $0x01, %al\n\t"
		"xor %bx, %bx\n\t"               
		"int $0x15\n\t"
		"jc  APM_Error\n\t"
		"mov $0x53, %ah\n\t"         	// Enable power management for all devices    
		"mov $0x08, %al\n\t"              
		"mov $0x0001, %bx\n\t"         
		"mov $0x0001, %cx\n\t"         
		"int $0x15\n\t" 
		"jc  APM_Error\n\t"
		"mov $0x53, %ah\n\t"      	// Set power state to off
		"mov $0x07, %al\n\t"          
		"mov $0x0001, %bx\n\t"        
		"mov $0x0003, %cx\n\t"  
		"int $0x15\n\t"
		"jc  APM_Error\n\t"
		"jmp No_Error"
	);

	__asm__("APM_Error:");
	prints("APM Error", 0x07);

	__asm__("Disconnect_Error:");
	prints("Disconnect Error", 0x07);

	__asm__("No_Error:");	
	return;
}

