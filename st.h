#pragma once

#include <SDL2/SDL.h>

#define TICK 0
#define G_WIDTH 10
#define G_HEIGHT 20
#define W_WIDTH 800
#define W_HEIGHT 600

enum shape_t {
	SHAPE_BOX = 0x0660, SHAPE_LOW = 0x0622,
	SHAPE_MID = 0x0262, SHAPE_TOP = 0x2260,
	SHAPE_LEF = 0x2640, SHAPE_RIG = 0x4620,
	SHAPE_BAR = 0x2222,
};

struct game_state {
	int grid[G_WIDTH][G_HEIGHT];
	SDL_Renderer *surface;
	enum shape_t shape;
	int lastdrop;
	int delay;
	int lines;
	int rot;
	int x;
	int y;
};

typedef int (*mod_f)(int *s, int v);
