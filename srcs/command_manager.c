#include "command_manager.h"
#include "global_variables.h"
#include "utils.h"
#include "windows_manager.h"
#include "input_handler.h"
#include "printk.h"

static void	handle_clear(void) {
	terminal_row = 0;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const	size_t		index = y * VGA_WIDTH + x;
			const	uint16_t	entry = vga_entry('\0', terminal_color);
			terminal_buffer[index] = entry;
			windows[current_window].content[index] = entry;
		}
	}
}

static int	which_command(const char *cmd) {
	if (!strcmp(cmd, "poweroff")) {
		outw(0x604, 0x2000);
		return NEWLINE;
	} else if (!strcmp(cmd, "restart")) {
		outb(0x64, 0xFE);
		return NEWLINE;
	} else if (!strcmp(cmd, "clear")) {
		handle_clear();
		return NO_NEWLINE;
	} else if (!strcmp(cmd, "chcolor")) {
		terminal_color = (((terminal_color & 0xF) + 1) % 16) | (((terminal_color >> 4) & 0xF) << 4);
		return NO_NEWLINE;
	} else if (!strcmp(cmd, "chbcolor")) {
		terminal_color = (terminal_color & 0xF) | (((((terminal_color >> 4) & 0xF) + 1) % 16) << 4);
		return NO_NEWLINE;
	} else {
		printk("command not found: \"", terminal_color);
		printk(cmd, terminal_color);
		printk("\"", terminal_color);
		return NEWLINE;
	}
}

static int	parse_buffer(const size_t N, const char *buf) {
	char	cmd[N + 1];

	for (size_t i = 0; i < N; i++) {
		cmd[i] = buf[i * 2];
	}
	cmd[N] = '\0';

	const int	status = which_command(cmd);

	for (size_t i = HISTORY_SIZE - 1; i > 0; i--) {
		strncpy(windows[current_window].history[i], windows[current_window].history[i - 1], CMD_MAX_SIZE);
	}
	strncpy(windows[current_window].history[0], cmd, CMD_MAX_SIZE);
	return status;
}

void	handle_command(void) {
	void	*data = &(terminal_buffer[windows[current_window].prompt_index]);
	char	*buf = (char *)data;

	if (!*buf) return;

	size_t	len = 0;
	while (buf[len * 2]) {
		len++;
	}

	if (parse_buffer(len, buf) == NO_NEWLINE) {
		terminal_column = 0;
	} else {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) {
			scroll();
			terminal_row = VGA_HEIGHT - 1;
		}
	}
}
