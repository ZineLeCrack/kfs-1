#include "utils.h"
#include "vga_colors.h"

uint8_t		vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

uint16_t	vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t)uc | (uint16_t)color << 8;
}

size_t		strlen(const char* str) {
	size_t	len = 0;

	while (str[len]) len++;
	return len;
}

uint8_t		inb(uint16_t port) {
	uint8_t	ret;

	__asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port)); // inb dx, al

	return ret;
}

uint16_t	keyboard_get_scancode() {
	while (!(inb(0x64) & 1));
	uint8_t code = inb(0x60);

	if (code == 0xE0) {
		while (!(inb(0x64) & 1));
		return (0xE000 | inb(0x60));
	}

	return code;
}

void		outb(uint16_t port, uint8_t value) {
	asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}
