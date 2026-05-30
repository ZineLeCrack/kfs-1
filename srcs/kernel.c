#include "kernel.h"

size_t		terminal_row;
size_t		terminal_column;
uint8_t		terminal_color;
uint16_t*	terminal_buffer = (uint16_t*)VGA_MEMORY;
size_t		current_window;
t_window	windows[3];

static inline uint8_t	vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t	vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t)uc | (uint16_t)color << 8;
}

static size_t			strlen(const char* str) {
	size_t len = 0;

	while (str[len]) len++;
	return len;
}

static void				init_kernel(void) {
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

static inline void		terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

static inline void		outb(uint16_t port, uint8_t value)
{
	asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void		move_cursor(uint16_t x, uint16_t y)
{
	uint16_t pos = y * 80 + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));

	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void				scroll() {
	for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = terminal_buffer[index + VGA_WIDTH];
			windows[current_window].content[x][y] = windows[current_window].content[x][y + 1];
		}
	}
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
		terminal_buffer[index] = vga_entry(' ', terminal_color);
		windows[current_window].content[x][VGA_HEIGHT - 1] = vga_entry(' ', terminal_color);
	}
}

static void				putchark(char c, uint8_t color)
{
	const size_t	index = terminal_row * VGA_WIDTH + terminal_column;

	windows[current_window].content[terminal_column][terminal_row] = vga_entry(c, color);
	terminal_buffer[index] = vga_entry(c, color);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			terminal_row = VGA_HEIGHT - 1;
			scroll();
		}
	}
	windows[current_window].cursor_x = terminal_column;
	windows[current_window].cursor_y = terminal_row;
}

static inline uint8_t	inb(uint16_t port) {
	uint8_t ret;

	__asm__ volatile ("inb %1, %0"
					  : "=a"(ret)
					  : "Nd"(port));

	return ret;
}

static inline uint8_t	keyboard_get_scancode() {
	uint8_t code = inb(0x60);

	if (code == 0xE0)
		return (0xE000 | inb(0x60));

	return code;
}

static void				printk(const char *str, uint8_t color) {
	size_t	size = strlen(str);

	for (size_t i = 0; i < size; i++)
		putchark(str[i], color);
}

static void				handle_backspace() {
	size_t index = terminal_row * VGA_WIDTH + terminal_column - 1;
	terminal_buffer[index] = vga_entry(' ', terminal_color);
	if (terminal_column) terminal_column--;
	else if (terminal_row) {
		terminal_column = VGA_WIDTH - 1;
		terminal_row--;
	}
}

static void				handle_newline() {
	terminal_column = 0;
	if (++terminal_row == VGA_HEIGHT) {
		scroll();
		terminal_row = VGA_HEIGHT - 1;
	}
	windows[current_window].cursor_x = terminal_column;
	windows[current_window].cursor_y = terminal_row;
}

static void				change_window(int n) {
	current_window = (current_window + n) % 3;
	terminal_column = windows[current_window].cursor_x;
	terminal_row = windows[current_window].cursor_y;
	move_cursor(terminal_column, terminal_row);
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = windows[current_window].content[x][y];
		}
	}
}

void				kernel_main(void) {
	init_kernel();

	for (size_t i = 0; i < 3; i++) {
		windows[i].cursor_x = 0;
		windows[i].cursor_y = 0;
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				windows[i].content[x][y] = vga_entry(' ', terminal_color);
			}
		}
	}
	current_window = 0;

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
			case KEY_BACKSPACE:   { handle_backspace(); break; }
			case KEY_ENTER:       { handle_newline(); break; }
			case KEY_LEFT:        { change_window(2); break; }
			case KEY_RIGHT:       { change_window(1); break; }
			default:              { c = "";  break; }
		}
		printk(c, terminal_color);
		move_cursor(terminal_column, terminal_row);
	}
}
