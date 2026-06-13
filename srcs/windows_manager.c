#include "windows_manager.h"
#include "global_variables.h"
#include "utils.h"

void	move_cursor(uint16_t x, uint16_t y) {
	windows[current_window].cursor_x = terminal_column;
	windows[current_window].cursor_y = terminal_row;

	uint16_t	pos = y * 80 + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));

	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void	scroll() {
	for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;

			terminal_buffer[index] = terminal_buffer[index + VGA_WIDTH];
			windows[current_window].content[y][x] = windows[current_window].content[y + 1][x];
		}
	}

	for (size_t x = 0; x < VGA_WIDTH; x++) {
		const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;

		terminal_buffer[index] = vga_entry(' ', terminal_color);
		windows[current_window].content[VGA_HEIGHT - 1][x] = vga_entry(' ', terminal_color);
	}
}

void	change_window(int n) {
	current_window = (current_window + n) % 3;
	terminal_column = windows[current_window].cursor_x;
	terminal_row = windows[current_window].cursor_y;

	move_cursor(terminal_column, terminal_row);

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = windows[current_window].content[y][x];
		}
	}
}

void	init_windows() {
	for (size_t i = 0; i < 3; i++) {
		windows[i].cursor_x = 0;
		windows[i].cursor_y = 0;
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				windows[i].content[y][x] = vga_entry(' ', terminal_color);
			}
		}
	}
}
