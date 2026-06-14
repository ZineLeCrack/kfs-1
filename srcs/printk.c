#include "printk.h"
#include "global_variables.h"
#include "utils.h"
#include "windows_manager.h"

static void	putchark(char c, uint8_t color) {
	const size_t	index = get_index();
	const size_t	end_index = get_end_index();

	for (size_t i = end_index; i > index; i--) {
		terminal_buffer[i] = terminal_buffer[i - 1];
		windows[current_window].content[i / VGA_WIDTH][i % VGA_WIDTH] = windows[current_window].content[(i - 1) / VGA_WIDTH][(i - 1) % VGA_WIDTH];
	}

	windows[current_window].content[terminal_row][terminal_column] = vga_entry(c, color);
	terminal_buffer[index] = vga_entry(c, color);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			terminal_row = VGA_HEIGHT - 1;
			scroll();
		}
	}
}

void	printk(const char *str, uint8_t color) {
	size_t	size = strlen(str);

	for (size_t i = 0; i < size; i++) {
		putchark(str[i], color);
	}
}
