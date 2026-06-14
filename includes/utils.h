#ifndef UTILS_H
# define UTILS_H

#include <stddef.h> // size_t
#include <stdint.h> // uint8_t, uint16_t
#include "vga_colors.h"

/* UTILS FUNCTIONS */
uint8_t		vga_entry_color(enum vga_color fg, enum vga_color bg);
uint16_t	vga_entry(unsigned char uc, uint8_t color);
size_t		strlen(const char* str);
uint8_t		inb(uint16_t port);
size_t		get_index();
size_t		get_end_index();
uint16_t	keyboard_get_scancode();
void		outb(uint16_t port, uint8_t value);

#endif
