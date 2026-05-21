#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH	80
#define VGA_HEIGHT	25
#define VGA_MEMORY	0xB8000

typedef struct {
	uint16_t	cursor_x;
	uint16_t	cursor_y;
	uint16_t	content[VGA_WIDTH][VGA_HEIGHT];
}	t_window;

enum keymap {
	KEY_ESC         = 0x01,
	KEY_1           = 0x02,
	KEY_2           = 0x03,
	KEY_3           = 0x04,
	KEY_4           = 0x05,
	KEY_5           = 0x06,
	KEY_6           = 0x07,
	KEY_7           = 0x08,
	KEY_8           = 0x09,
	KEY_9           = 0x0A,
	KEY_0           = 0x0B,
	KEY_DASH        = 0x0C,
	KEY_EQUAL       = 0x0D,
	KEY_BACKSPACE   = 0x0E,
	KEY_TAB         = 0x0F,
	KEY_Q           = 0x10,
	KEY_W           = 0x11,
	KEY_E           = 0x12,
	KEY_R           = 0x13,
	KEY_T           = 0x14,
	KEY_Y           = 0x15,
	KEY_U           = 0x16,
	KEY_I           = 0x17,
	KEY_O           = 0x18,
	KEY_P           = 0x19,
	KEY_LSQ_BRACKET = 0x1A,
	KEY_RSQ_BRACKET = 0x1B,
	KEY_ENTER       = 0x1C,
	KEY_LCTRL       = 0x1D,
	KEY_A           = 0x1E,
	KEY_S           = 0x1F,
	KEY_D           = 0x20,
	KEY_F           = 0x21,
	KEY_G           = 0x22,
	KEY_H           = 0x23,
	KEY_J           = 0x24,
	KEY_K           = 0x25,
	KEY_L           = 0x26,
	KEY_SEMICOLON   = 0x27,
	KEY_APOSTROPHE  = 0x28,
	KEY_BACKTICK    = 0x29,
	KEY_LSHIFT      = 0x2A,
	KEY_BACKSLASH   = 0x2B,
	KEY_Z           = 0x2C,
	KEY_X           = 0x2D,
	KEY_C           = 0x2E,
	KEY_V           = 0x2F,
	KEY_B           = 0x30,
	KEY_N           = 0x31,
	KEY_M           = 0x32,
	KEY_COMMA       = 0x33,
	KEY_POINT       = 0x34,
	KEY_SLASH       = 0x35,
	KEY_RSHIFT      = 0x36,
	KEY_ASTERISK    = 0x37,
	KEY_LALT        = 0x38,
	KEY_SPACE       = 0x39,
	KEY_CAPSLOCK    = 0x3A,
	KEY_UP		    = 0x48,
	KEY_DOWN		= 0x50,
	KEY_LEFT		= 0x4B,
	KEY_RIGHT		= 0x4D,
};

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
