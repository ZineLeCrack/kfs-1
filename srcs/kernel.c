#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum keymap {
	KEY_ESC         = 0x01,
	KEY_1           = 0x02,
	KEY_2           = 0x03,
	KEY_3           = 0x04,
	KEY_4           = 0x05,
	KEY_5           = 0x06,
	KEY_6           = 0x07,
	KEY_7           = 0x08,
	KEY_8           = 0x09,
	KEY_9           = 0x0A,
	KEY_0           = 0x0B,
	KEY_DASH        = 0x0C,
	KEY_EQUAL       = 0x0D,
	KEY_BACKSPACE   = 0x0E,
	KEY_TAB         = 0x0F,
	KEY_Q           = 0x10,
	KEY_W           = 0x11,
	KEY_E           = 0x12,
	KEY_R           = 0x13,
	KEY_T           = 0x14,
	KEY_Y           = 0x15,
	KEY_U           = 0x16,
	KEY_I           = 0x17,
	KEY_O           = 0x18,
	KEY_P           = 0x19,
	KEY_LSQ_BRACKET = 0x1A,
	KEY_RSQ_BRACKET = 0x1B,
	KEY_ENTER       = 0x1C,
	KEY_LCTRL       = 0x1D,
	KEY_A           = 0x1E,
	KEY_S           = 0x1F,
	KEY_D           = 0x20,
	KEY_F           = 0x21,
	KEY_G           = 0x22,
	KEY_H           = 0x23,
	KEY_J           = 0x24,
	KEY_K           = 0x25,
	KEY_L           = 0x26,
	KEY_SEMICOLON   = 0x27,
	KEY_APOSTROPHE  = 0x28,
	KEY_BACKTICK    = 0x29,
	KEY_LSHIFT      = 0x2A,
	KEY_BACKSLASH   = 0x2B,
	KEY_Z           = 0x2C,
	KEY_X           = 0x2D,
	KEY_C           = 0x2E,
	KEY_V           = 0x2F,
	KEY_B           = 0x30,
	KEY_N           = 0x31,
	KEY_M           = 0x32,
	KEY_COMMA       = 0x33,
	KEY_POINT       = 0x34,
	KEY_SLASH       = 0x35,
	KEY_RSHIFT      = 0x36,
	KEY_ASTERISK    = 0x37,
	KEY_LALT        = 0x38,
	KEY_SPACE       = 0x39,
	KEY_CAPSLOCK    = 0x3A,
};

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t)uc | (uint16_t)color << 8;
}

size_t strlen(const char* str) {
	size_t len = 0;

	while (str[len])
		len++;
	return len;
}

#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_MEMORY	0xB8000

size_t		terminal_row;
size_t		terminal_column;
uint8_t		terminal_color;
uint16_t*	terminal_buffer = (uint16_t*)VGA_MEMORY;

void	init_kernel(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const	size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void	terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

static inline void	outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void move_cursor(uint16_t x, uint16_t y)
{
    uint16_t pos = y * 80 + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void scroll() {
	for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = terminal_buffer[index + VGA_WIDTH];
		}
	}
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
		terminal_buffer[index] = vga_entry(' ', terminal_color);
	}
}

void	putchark(char c, uint8_t color) 
{
	const size_t	index = terminal_row * VGA_WIDTH + terminal_column;

	terminal_buffer[index] = vga_entry(c, color);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			terminal_row = VGA_HEIGHT - 1;
			scroll();
		}
	}
}

static inline uint8_t	inb(uint16_t port) {
    uint8_t ret;

    __asm__ volatile ("inb %1, %0"
                      : "=a"(ret)
                      : "Nd"(port));

    return ret;
}

static inline uint8_t	keyboard_get_scancode() {
	return inb(0x60);
}

void	printk(const char *str, uint8_t color) {
	size_t	size = strlen(str);

	for (size_t i = 0; i < size; i++)
		putchark(str[i], color);
}

void	kernel_main(void) {
	init_kernel();

	printk("        :::      ::::::::                                                       ", vga_entry_color(VGA_COLOR_LIGHT_GREY,    VGA_COLOR_BLACK));
	printk("      :+:      :+:    :+:                                                       ", vga_entry_color(VGA_COLOR_BLUE,          VGA_COLOR_BLACK));
	printk("    +:+ +:+         +:+                                                         ", vga_entry_color(VGA_COLOR_RED,           VGA_COLOR_BLACK));
	printk("  +#+  +:+       +#+                                                            ", vga_entry_color(VGA_COLOR_CYAN,          VGA_COLOR_BLACK));
	printk("+#+#+#+#+#+   +#+                                                               ", vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK));
	printk("     #+#    #+#                                                                 ", vga_entry_color(VGA_COLOR_LIGHT_BROWN,   VGA_COLOR_BLACK));
	printk("    ###   ########                                                              ", vga_entry_color(VGA_COLOR_LIGHT_GREEN,   VGA_COLOR_BLACK));
	move_cursor(terminal_column, terminal_row);

	while (1) {
        uint8_t	scancode = keyboard_get_scancode();

		while (!(inb(0x64) & 1));

		char	*c = "";
		switch (scancode)
		{
			case KEY_A:           { c = "A"; break; }
			case KEY_B:           { c = "B"; break; }
			case KEY_C:           { c = "C"; break; }
			case KEY_D:           { c = "D"; break; }
			case KEY_E:           { c = "E"; break; }
			case KEY_F:           { c = "F"; break; }
			case KEY_G:           { c = "G"; break; }
			case KEY_H:           { c = "H"; break; }
			case KEY_I:           { c = "I"; break; }
			case KEY_J:           { c = "J"; break; }
			case KEY_K:           { c = "K"; break; }
			case KEY_L:           { c = "L"; break; }
			case KEY_M:           { c = "M"; break; }
			case KEY_N:           { c = "N"; break; }
			case KEY_O:           { c = "O"; break; }
			case KEY_P:           { c = "P"; break; }
			case KEY_Q:           { c = "Q"; break; }
			case KEY_R:           { c = "R"; break; }
			case KEY_S:           { c = "S"; break; }
			case KEY_T:           { c = "T"; break; }
			case KEY_U:           { c = "U"; break; }
			case KEY_V:           { c = "V"; break; }
			case KEY_W:           { c = "W"; break; }
			case KEY_X:           { c = "X"; break; }
			case KEY_Y:           { c = "Y"; break; }
			case KEY_Z:           { c = "Z"; break; }
			case KEY_0:           { c = "0"; break; }
			case KEY_1:           { c = "1"; break; }
			case KEY_2:           { c = "2"; break; }
			case KEY_3:           { c = "3"; break; }
			case KEY_4:           { c = "4"; break; }
			case KEY_5:           { c = "5"; break; }
			case KEY_6:           { c = "6"; break; }
			case KEY_7:           { c = "7"; break; }
			case KEY_8:           { c = "8"; break; }
			case KEY_9:           { c = "9"; break; }
			case KEY_SPACE:       { c = " "; break; }
			case KEY_COMMA:       { c = ","; break; }
			case KEY_POINT:       { c = "."; break; }
			case KEY_SLASH:       { c = "/"; break; }
			case KEY_SEMICOLON:   { c = ";"; break; }
			case KEY_APOSTROPHE:  { c = "'"; break; }
			case KEY_BACKTICK:    { c = "`"; break; }
			case KEY_DASH:        { c = "-"; break; }
			case KEY_EQUAL:       { c = "="; break; }
			case KEY_BACKSLASH:   { c = "\\"; break; }
			case KEY_LSQ_BRACKET: { c = "["; break; }
			case KEY_RSQ_BRACKET: { c = "]"; break; }
			case KEY_BACKSPACE:   {
				size_t index = terminal_row * VGA_WIDTH + terminal_column - 1;
				terminal_buffer[index] = vga_entry(' ', terminal_color);
				if (terminal_column) terminal_column--;
				else if (terminal_row) {
					terminal_column = VGA_WIDTH - 1;
					terminal_row--;
				}
				break;
			}
			case KEY_ENTER:       {
				terminal_column = 0;
				if (++terminal_row == VGA_HEIGHT) {
					scroll();
					terminal_row = VGA_HEIGHT - 1;
				}
				break;
			}
			default:              { c = "";  break; }
		}
		printk(c, terminal_color);
		move_cursor(terminal_column, terminal_row);
    }
}
