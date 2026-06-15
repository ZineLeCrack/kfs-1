#ifndef T_WINDOW_H
# define T_WINDOW_H

#include <stdint.h> // uint16_t
#include "defines.h"

/* WINDOW STRUCTURE */
typedef struct {
	uint16_t	cursor_x;
	uint16_t	cursor_y;
	uint16_t	content[VGA_HEIGHT * VGA_WIDTH];
	size_t		prompt_index;
	char		history[HISTORY_SIZE][CMD_MAX_SIZE + 1];
	int			history_index;
}	t_window;

#endif
