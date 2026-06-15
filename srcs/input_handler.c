#include <stdbool.h>
#include "input_handler.h"
#include "global_variables.h"
#include "utils.h"
#include "printk.h"
#include "windows_manager.h"
#include "command_manager.h"

static size_t	get_last_index_by_row(size_t row) {
	size_t	x = VGA_WIDTH - 1;

	while (x > 0 && (windows[current_window].content[(row * VGA_WIDTH) + (x - 1)] & 0xFF) == '\0') {
		x--;
	}

	return x;
}

static void		handle_tab(void) {
	const size_t	spaces = 4 - ((get_index() + 1) % 4);

	for (size_t i = 0; i < spaces; i++) {
		printk(" ", terminal_color);
	}
}

static void		handle_backspace(void) {
	const size_t	index = get_index();

	if (index == windows[current_window].prompt_index) return;

	const size_t	end_index = get_end_index();

	for (size_t i = index - 1; i < end_index; i++) {
		terminal_buffer[i] = terminal_buffer[i + 1];
		windows[current_window].content[i] = windows[current_window].content[i + 1];
	}

	if (terminal_column) {
		terminal_column--;
	}
	else if (terminal_row) {
		terminal_row--;
		terminal_column = get_last_index_by_row(terminal_row);
	}
}

static void		handle_delete(void) {
	const size_t	index = get_index();
	const size_t	end_index = get_end_index();

	if (index == end_index) return;

	for (size_t i = index; i < end_index; i++) {
		terminal_buffer[i] = terminal_buffer[i + 1];
		windows[current_window].content[i] = windows[current_window].content[i + 1];
	}
}

static void		handle_newline(void) {
	terminal_column = 0;
	if (++terminal_row == VGA_HEIGHT) {
		scroll();
		terminal_row = VGA_HEIGHT - 1;
	}
	handle_command();
	new_prompt();
	windows[current_window].history_index = -1;
}

static void		handle_direction_keys(uint8_t code) {
	switch (code)
	{
		case KEY_UP: {
			int	history_index = windows[current_window].history_index;
			if (history_index + 1 < HISTORY_SIZE) {
				if (windows[current_window].history[history_index + 1][0]) {
					windows[current_window].history_index++;
					clear_cmd();
					printk(windows[current_window].history[history_index + 1], terminal_color);
				}
			}
			// if (terminal_row > 0) {
			// 	terminal_row--;
			// }
			break;
		}
		case KEY_DOWN: {
			int	history_index = windows[current_window].history_index;
			if (history_index > 0) {
				windows[current_window].history_index--;
				clear_cmd();
				printk(windows[current_window].history[history_index - 1], terminal_color);
			} else if (history_index == 0) {
				windows[current_window].history_index--;
				clear_cmd();
			}
			// if (terminal_row < VGA_HEIGHT - 1) {
			// 	terminal_row++;
			// } else {
			// 	terminal_column = VGA_WIDTH - 1;
			// }
			break;
		}
		case KEY_LEFT: {
			if (get_index() == windows[current_window].prompt_index) {
				break;
			}
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
			}
			break;
		}
		default: { break; }
	}

	size_t	x = get_last_index_by_row(terminal_row);
	if (x == 0) {
		terminal_row--;
		terminal_column = get_last_index_by_row(terminal_row);
	} else if (terminal_column > x) {
		terminal_column = x;
	}

	size_t	index = get_index();
	if (index < windows[current_window].prompt_index) {
		terminal_row = windows[current_window].prompt_index / VGA_WIDTH;
		terminal_column = windows[current_window].prompt_index % VGA_WIDTH;
	}
}

void	handle_ctrl_c(void) {
	const size_t	end_index = get_end_index();

	terminal_column = 0;
	terminal_row = end_index / VGA_WIDTH;

	if (++terminal_row == VGA_HEIGHT) {
		terminal_row--;
		scroll();
	}
	new_prompt();
}

void		new_prompt(void) {
	printk("$> ", terminal_color);
	windows[current_window].prompt_index = get_index();
}

void		handle_input(void) {
	static bool	is_lshift_on = false;
	static bool	is_rshift_on = false;
	static bool is_caps_lock_on = false;
	static bool	is_lctrl_on = false;
	static bool	is_rctrl_on = false;

	uint16_t	scancode = keyboard_get_scancode();

	char	*c = "";
	if ((scancode & 0xFF00) == 0xE000) {
		switch (scancode & 0xFF)
		{
			case KEY_UP:    		{ handle_direction_keys(KEY_UP); break; }
			case KEY_DOWN:  		{ handle_direction_keys(KEY_DOWN); break; }
			case KEY_LEFT:  		{ (is_lshift_on || is_rshift_on) ? change_window(2) : handle_direction_keys(KEY_LEFT); break; }
			case KEY_RIGHT: 		{ (is_lshift_on || is_rshift_on) ? change_window(1) : handle_direction_keys(KEY_RIGHT); break; }
			case KEY_DELETE:		{ handle_delete(); break; }
			case KEY_END:			{ terminal_column = get_last_index_by_row(terminal_row); break; }
			case KEY_RCTRL:			{ is_rctrl_on = true; break; }
			case KEY_RCTRL | 0x80:	{ is_rctrl_on = false; break; }
			default:				{ break; }
		}
	} else {
		switch (scancode)
		{
			case KEY_A:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "A") : (c = "a")); break; }
			case KEY_B:             { ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "B") : (c = "b")); break; }
			case KEY_C:             { (is_lctrl_on || is_rctrl_on) ? (handle_ctrl_c()) : (((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "C") : (c = "c"))); break; }
			case KEY_D:             { (is_lctrl_on || is_rctrl_on) ? (outw(0x604, 0x2000)) : ((is_caps_lock_on ^ (is_lshift_on || is_rshift_on)) ? (c = "D") : (c = "d")); break; }
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
			case KEY_TAB:			{ handle_tab(); break; }
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
			case KEY_LCTRL:			{ is_lctrl_on = true; break; }
			case KEY_LCTRL | 0x80:	{ is_lctrl_on = false; break; }
			default:                { break; }
		}
	}
	if (c[0]) printk(c, terminal_color);
	move_cursor(terminal_column, terminal_row);
}
