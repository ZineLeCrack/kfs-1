#include "input_handler.h"
#include "global_variables.h"
#include "utils.h"
#include "printk.h"
#include "windows_manager.h"

static void	handle_backspace() {
	size_t	index = terminal_row * VGA_WIDTH + terminal_column - 1;

	terminal_buffer[index] = vga_entry(' ', terminal_color);

	if (terminal_column) {
		terminal_column--;
	}
	else if (terminal_row) {
		terminal_column = VGA_WIDTH - 1;
		terminal_row--;
	}
}

static void	handle_newline() {
	terminal_column = 0;
	if (++terminal_row == VGA_HEIGHT) {
		scroll();
		terminal_row = VGA_HEIGHT - 1;
	}
	windows[current_window].cursor_x = terminal_column;
	windows[current_window].cursor_y = terminal_row;
}

void	handle_input() {
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
		case KEY_DOT:         { c = "."; break; }
		case KEY_SLASH:       { c = "/"; break; }
		case KEY_SEMICOLON:   { c = ";"; break; }
		case KEY_APOSTROPHE:  { c = "'"; break; }
		case KEY_GRAVE:       { c = "`"; break; }
		case KEY_MINUS:       { c = "-"; break; }
		case KEY_EQUAL:       { c = "="; break; }
		case KEY_BACKSLASH:   { c = "\\"; break; }
		case KEY_LBRACKET:    { c = "["; break; }
		case KEY_RBRACKET:    { c = "]"; break; }
		case KEY_BACKSPACE:   { handle_backspace(); break; }
		case KEY_ENTER:       { handle_newline(); break; }
		case KEY_LEFT:        { change_window(2); break; }
		case KEY_RIGHT:       { change_window(1); break; }
		default:              { c = "";  break; }
	}
	printk(c, terminal_color);
	move_cursor(terminal_column, terminal_row);
}
