#ifndef WINDOWS_MANAGER_H
# define WINDOWS_MANAGER_H

#include <stdint.h> // uint16_t

/* WINDOWS MANAGERMENT */
void	move_cursor(uint16_t x, uint16_t y);
void	scroll();
void	change_window(int n);
void	init_windows();

#endif
