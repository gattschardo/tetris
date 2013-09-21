#pragma once

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

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
	void *surface;
	enum shape_t shape;
	int lastdrop;
	int delay;
	int lines;
	int rot;
	int x;
	int y;
	FILE *log;
};

typedef int (*mod_f)(int *s, int v);

int const_modf(int *s, int v);

int iter_shape(struct game_state *gs, int x, int y, mod_f mf, int *s);

int collision(struct game_state *gs, int xinc, int yinc);

void put_shape(struct game_state *gs, int k);

int fit_block(struct game_state *gs);


void load_block(struct game_state *gs);


void init_game_state(struct game_state *gs);


void fix_grid(struct game_state *gs);
void move_x(struct game_state *gs, int inc);
void drop_block(struct game_state *gs);
void rot_block(struct game_state *gs);

/* external */

void end_cb(struct game_state *gs);
