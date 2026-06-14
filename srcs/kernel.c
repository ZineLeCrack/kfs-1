#include "global_variables.h"
#include "vga_colors.h"
#include "utils.h"
#include "printk.h"
#include "input_handler.h"
#include "windows_manager.h"

size_t		terminal_row = 0;
size_t		terminal_column = 0;
uint8_t		terminal_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
uint16_t*	terminal_buffer = (uint16_t*)VGA_MEMORY;
size_t		current_window = 0;
t_window	windows[3];

static void	init_kernel(void) {
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const	size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry('\0', terminal_color);
		}
	}
}

void		kernel_main(void) {
	init_kernel();
	init_windows();

	printk("        :::      ::::::::                                                       ", vga_entry_color(VGA_COLOR_LIGHT_GREY,    VGA_COLOR_BLACK));
	printk("      :+:      :+:    :+:                                                       ", vga_entry_color(VGA_COLOR_BLUE,          VGA_COLOR_BLACK));
	printk("    +:+ +:+         +:+                                                         ", vga_entry_color(VGA_COLOR_RED,           VGA_COLOR_BLACK));
	printk("  +#+  +:+       +#+                                                            ", vga_entry_color(VGA_COLOR_CYAN,          VGA_COLOR_BLACK));
	printk("+#+#+#+#+#+   +#+                                                               ", vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK));
	printk("     #+#    #+#                                                                 ", vga_entry_color(VGA_COLOR_LIGHT_BROWN,   VGA_COLOR_BLACK));
	printk("    ###   ########                                                              ", vga_entry_color(VGA_COLOR_LIGHT_GREEN,   VGA_COLOR_BLACK));
	new_prompt();
	move_cursor(terminal_column, terminal_row);

	while (1) {
		handle_input();
	}
}
