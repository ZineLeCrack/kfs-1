#include "utils.h"
#include "defines.h"
#include "global_variables.h"

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

int			strcmp(const char *s1, const char *s2) {
	size_t	i;

	for (i = 0; s1[i] && s2[i] && s1[i] == s2[i]; i++);
	return s1[i] - s2[i];
}

char		*strncpy(char *dest, const char *src, size_t size) {
	size_t	i = 0;

	while (i < size && src[i]) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';

	return dest;
}

uint8_t		inb(uint16_t port) {
	uint8_t	ret;

	__asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port)); // inb dx, al

	return ret;
}

size_t		get_index(void) {
	return (terminal_row * VGA_WIDTH + terminal_column);
}

size_t		get_end_index(void) {
	size_t	index = get_index();

	while ((terminal_buffer[index] & 0xFF) != '\0') {
		index++;
	}
	return index;
}

void		clear_cmd(void) {
	size_t			index = windows[current_window].prompt_index;

	terminal_row = index / VGA_WIDTH;
	terminal_column = index % VGA_WIDTH;

	const size_t	end_index = get_end_index();

	while (index < end_index) {
		terminal_buffer[index] = vga_entry('\0', terminal_color);
		windows[current_window].content[index] = vga_entry('\0', terminal_color);
		index++;
	}
}

uint16_t	keyboard_get_scancode(void) {
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

void		outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}
