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

static void	parse_buffer(const size_t N, const char *buf) {
	char	cmd[N + 1];

	for (size_t i = 0; i < N; i++) {
		cmd[i] = buf[i * 2];
	}
	cmd[N] = '\0';

	which_command(cmd);
	for (size_t i = HISTORY_SIZE - 1; i > 0; i--) {
		strncpy(windows[current_window].history[i], windows[current_window].history[i - 1], CMD_MAX_SIZE);
	}
	strncpy(windows[current_window].history[0], cmd, CMD_MAX_SIZE);
}

void	handle_command(void) {
	void	*data = &(terminal_buffer[windows[current_window].prompt_index]);
	char	*buf = (char *)data;

	if (!*buf) return;

	size_t	len = 0;
	while (buf[len * 2]) {
		len++;
	}

	parse_buffer(len, buf);

	terminal_column = 0;
	if (++terminal_row == VGA_HEIGHT) {
		scroll();
		terminal_row = VGA_HEIGHT - 1;
	}
}
