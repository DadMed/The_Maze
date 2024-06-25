#ifndef MAZE_H
#define MAZE_H

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

extern const int MAP_WIDTH;
extern const int MAP_HEIGHT;

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

// Function prototypes
void initialize_SDL();
void close_SDL();

#endif // MAZE_H

