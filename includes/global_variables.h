#ifndef GLOBAL_VARIABLES_H
# define GLOBAL_VARIABLES_H

# include <stddef.h> // size_t
# include <stdint.h> // uint8_t, uint16_t
# include "t_window.h"

/* ALL GLOBAL VARIABLES */
extern size_t		terminal_row;
extern size_t		terminal_column;
extern uint8_t		terminal_color;
extern uint16_t*	terminal_buffer;
extern size_t		current_window;
extern t_window		windows[3];

#endif
