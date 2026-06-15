#ifndef UTILS_H
# define UTILS_H

#include <stddef.h> // size_t
#include <stdint.h> // uint8_t, uint16_t
#include "vga_colors.h"

/* UTILS FUNCTIONS */
uint8_t		vga_entry_color(enum vga_color fg, enum vga_color bg);
uint16_t	vga_entry(unsigned char uc, uint8_t color);
size_t		strlen(const char* str);
int			strcmp(const char *s1, const char *s2);
char		*strncpy(const char *src, char *dest, size_t size);
uint8_t		inb(uint16_t port);
size_t		get_index(void);
size_t		get_end_index(void);
void		clear_cmd(void);
uint16_t	keyboard_get_scancode(void);
void		outb(uint16_t port, uint8_t value);
void		outw(uint16_t port, uint16_t value);

#endif
