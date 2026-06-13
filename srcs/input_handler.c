#include <stdbool.h>
#include "input_handler.h"
#include "global_variables.h"
#include "utils.h"
#include "printk.h"
#include "windows_manager.h"

static size_t	get_last_index_by_row(size_t row) {
	size_t	x = VGA_WIDTH - 1;

	while (x > 0 && (windows[current_window].content[row][x - 1] & 0xFF) == ' ') {
		x--;
	}

	return x;
}

static void		handle_backspace() {
	size_t	index = terminal_row * VGA_WIDTH + terminal_column - 1;

	terminal_buffer[index] = vga_entry(' ', terminal_color);
	windows->content[terminal_row][terminal_column - 1] = vga_entry(' ', terminal_color);

	if (terminal_column) {
		terminal_column--;
	}
	else if (terminal_row) {
		terminal_row--;
		terminal_column = get_last_index_by_row(terminal_row);
	}
}

static void		handle_newline() {
	terminal_column = 0;
	if (++terminal_row == VGA_HEIGHT) {
		scroll();
		terminal_row = VGA_HEIGHT - 1;
	}
}

static void		handle_direction_keys(uint8_t code) {
	switch (code)
	{
		case KEY_UP: {
			if (terminal_row > 0) {
				terminal_row--;
			}
			break;
		}
		case KEY_DOWN: {
			if (terminal_row < VGA_HEIGHT - 1) {
				terminal_row++;
			} else {
				scroll();
			}
			break;
		}
		case KEY_LEFT: {
			if (terminal_column > 0) {
				terminal_column--;
			} else {
				terminal_row--;
				terminal_column = get_last_index_by_row(terminal_row);
			}
			break;
		}
		case KEY_RIGHT: {
			if (terminal_column < VGA_WIDTH - 1 && terminal_column < get_last_index_by_row(terminal_row)) {
				terminal_column++;
			} else {
				terminal_row++;
			}
			break;
		}
		default: { break; }
	}

	size_t	x = get_last_index_by_row(terminal_row);
	if (terminal_column > x) {
		terminal_column = x;
	}
}

void		handle_input() {
	static bool	is_lshift_on = false;
	static bool	is_rshift_on = false;
	static bool is_caps_lock_on = false;

	uint16_t	scancode = keyboard_get_scancode();

	char	*c = "";
	if ((scancode & 0xFF00) == 0xE000) {
		switch (scancode & 0xFF)
		{
			case KEY_UP:    { handle_direction_keys(KEY_UP); break; }
			case KEY_DOWN:  { handle_direction_keys(KEY_DOWN); break; }
			case KEY_LEFT:  { handle_direction_keys(KEY_LEFT); break; }
			case KEY_RIGHT: { handle_direction_keys(KEY_RIGHT); break; }
			default: { break; }
		}
	} else {
		switch (scancode)
		{
			case KEY_A:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "A") : (c = "a")); break; }
			case KEY_B:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "B") : (c = "b")); break; }
			case KEY_C:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "C") : (c = "c")); break; }
			case KEY_D:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "D") : (c = "d")); break; }
			case KEY_E:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "E") : (c = "e")); break; }
			case KEY_F:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "F") : (c = "f")); break; }
			case KEY_G:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "G") : (c = "g")); break; }
			case KEY_H:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "H") : (c = "h")); break; }
			case KEY_I:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "I") : (c = "i")); break; }
			case KEY_J:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "J") : (c = "j")); break; }
			case KEY_K:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "K") : (c = "k")); break; }
			case KEY_L:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "L") : (c = "l")); break; }
			case KEY_M:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "M") : (c = "m")); break; }
			case KEY_N:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "N") : (c = "n")); break; }
			case KEY_O:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "O") : (c = "o")); break; }
			case KEY_P:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "P") : (c = "p")); break; }
			case KEY_Q:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "Q") : (c = "q")); break; }
			case KEY_R:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "R") : (c = "r")); break; }
			case KEY_S:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "S") : (c = "s")); break; }
			case KEY_T:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "T") : (c = "t")); break; }
			case KEY_U:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "U") : (c = "u")); break; }
			case KEY_V:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "V") : (c = "v")); break; }
			case KEY_W:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "W") : (c = "w")); break; }
			case KEY_X:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "X") : (c = "x")); break; }
			case KEY_Y:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "Y") : (c = "y")); break; }
			case KEY_Z:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "Z") : (c = "z")); break; }
			case KEY_0:             { ((is_lshift_on || is_rshift_on) ? (c = ")") : (c = "0")); break; }
			case KEY_1:             { ((is_lshift_on || is_rshift_on) ? (c = "!") : (c = "1")); break; }
			case KEY_2:             { ((is_lshift_on || is_rshift_on) ? (c = "@") : (c = "2")); break; }
			case KEY_3:             { ((is_lshift_on || is_rshift_on) ? (c = "#") : (c = "3")); break; }
			case KEY_4:             { ((is_lshift_on || is_rshift_on) ? (c = "$") : (c = "4")); break; }
			case KEY_5:             { ((is_lshift_on || is_rshift_on) ? (c = "%") : (c = "5")); break; }
			case KEY_6:             { ((is_lshift_on || is_rshift_on) ? (c = "^") : (c = "6")); break; }
			case KEY_7:             { ((is_lshift_on || is_rshift_on) ? (c = "&") : (c = "7")); break; }
			case KEY_8:             { ((is_lshift_on || is_rshift_on) ? (c = "*") : (c = "8")); break; }
			case KEY_9:             { ((is_lshift_on || is_rshift_on) ? (c = "(") : (c = "9")); break; }
			case KEY_SPACE:         { c = " "; break; }
			case KEY_COMMA:         { ((is_lshift_on || is_rshift_on) ? (c = "<") : (c = ",")); break; }
			case KEY_DOT:           { ((is_lshift_on || is_rshift_on) ? (c = ">") : (c = ".")); break; }
			case KEY_SLASH:         { ((is_lshift_on || is_rshift_on) ? (c = "?") : (c = "/")); break; }
			case KEY_SEMICOLON:     { ((is_lshift_on || is_rshift_on) ? (c = ":") : (c = ";")); break; }
			case KEY_APOSTROPHE:    { ((is_lshift_on || is_rshift_on) ? (c = "\"") : (c = "'")); break; }
			case KEY_GRAVE:         { ((is_lshift_on || is_rshift_on) ? (c = "~") : (c = "`")); break; }
			case KEY_MINUS:         { ((is_lshift_on || is_rshift_on) ? (c = "_") : (c = "-")); break; }
			case KEY_EQUAL:         { ((is_lshift_on || is_rshift_on) ? (c = "+") : (c = "=")); break; }
			case KEY_BACKSLASH:     { ((is_lshift_on || is_rshift_on) ? (c = "|") : (c = "\\")); break; }
			case KEY_LBRACKET:      { ((is_lshift_on || is_rshift_on) ? (c = "{") : (c = "[")); break; }
			case KEY_RBRACKET:      { ((is_lshift_on || is_rshift_on) ? (c = "}") : (c = "]")); break; }
			case KEY_BACKSPACE:     { handle_backspace(); break; }
			case KEY_ENTER:         { handle_newline(); break; }
			case KEY_LSHIFT:        { is_lshift_on = true;  break; }
			case KEY_RSHIFT:        { is_rshift_on = true;  break; }
			case KEY_LSHIFT | 0x80: { is_lshift_on = false; break; }
			case KEY_RSHIFT | 0x80: { is_rshift_on = false; break; }
			case KEY_CAPSLOCK:      { is_caps_lock_on = !is_caps_lock_on; break; }
			default:                { break; }
		}
	}
	(void)is_lshift_on; (void)is_rshift_on;
	if (c[0]) printk(c, terminal_color);
	move_cursor(terminal_column, terminal_row);
}
