#include "command_manager.h"
#include "global_variables.h"
#include "utils.h"
#include "windows_manager.h"
#include "printk.h"

static void	which_command(const char *cmd) {
	if (!strcmp(cmd, "poweroff")) {
		outw(0x604, 0x2000);
	} else if (!strcmp(cmd, "restart")) {
		outb(0x64, 0xFE);
	} else if (!strcmp(cmd, "chcolor")) {
		terminal_color = (((terminal_color & 0xF) + 1) % 16) | (((terminal_color >> 4) & 0xF) << 4);
	} else if (!strcmp(cmd, "chbcolor")) {
		terminal_color = (terminal_color & 0xF) | (((((terminal_color >> 4) & 0xF) + 1) % 16) << 4);
	} else {
		printk("command not found: \"", terminal_color);
		printk(cmd, terminal_color);
		printk("\"", terminal_color);
	}
}

static void	tmp(const size_t N, const char *buf) {
	char	cmd[N + 1];

	for (size_t i = 0; i < N; i++) {
		cmd[i] = buf[i * 2];
	}
	cmd[N] = '\0';

	which_command(cmd);
}

void	handle_command() {
	void	*data = &(terminal_buffer[windows[current_window].prompt_index]);
	char	*buf = (char *)data;

	if (!*buf) return;

	tmp(strlen(buf) / 2, buf);

	terminal_column = 0;
	if (++terminal_row == VGA_HEIGHT) {
		scroll();
		terminal_row = VGA_HEIGHT - 1;
	}
}
