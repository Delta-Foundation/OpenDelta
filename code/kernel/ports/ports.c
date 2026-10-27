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

void system_reboot(void)
{
	__asm__ volatile ("cli");

	/* Pulse the keyboard controller reset line. */
	for (int i = 0; i < 100000 && (port_byte_in(0x64) & 0x02); i++) {
		;
	}
	port_byte_out(0x64, 0xFE);

	/* Give the reset pulse time to take effect before trying anything else. */
	for (volatile int i = 0; i < 1000000; i++) {
		;
	}

	/* ACPI reset control register (QEMU/Bochs/VirtualBox). */
	port_byte_out(0xCF9, 0x06);
	port_byte_out(0xCF9, 0x0E);

	/* Last resort: triple fault. */
	struct {
		uint16_t limit;
		uint64_t base;
	} __attribute__((packed)) null_idt = { 0, 0 };

	__asm__ volatile ("lidt %0; int $3" :: "m"(null_idt));

	for (;;) {
		__asm__ volatile ("hlt");
	}
}

void system_poweroff(void)
{
	__asm__ volatile ("cli");

	/* QEMU, Bochs and VirtualBox ACPI shutdown ports. */
	port_word_out(0x604, 0x2000);
	port_word_out(0xB004, 0x2000);
	port_word_out(0x4004, 0x3400);

	for (;;) {
		__asm__ volatile ("hlt");
	}
}

void APM_interface_init(void)
{
	system_poweroff();
}
