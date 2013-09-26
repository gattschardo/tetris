#pragma once

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

enum { TICK = 0 };

enum {
	W_WIDTH = 800,
	W_HEIGHT = 600,
};

enum {
	G_WIDTH = 10,
	G_HEIGHT = 20,
};

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

/* A function that returns a new value given the input value v and state s, possibly modifying s. */
typedef int (*mod_f)(int *s, int v);

/* Constant modifying function - never modifies s, always returns *s as new v */
int const_modf(int *s, int v);

/* Apply mf with initial state s to each block in the falling shape in gs, if positioned at x/y */
int iter_shape(struct game_state *gs, int x, int y, mod_f mf, int *s);

/* return true if modifying the position of the falling shape of gs by xinc/yinc results in a collision */
int collision(struct game_state *gs, int xinc, int yinc);

/* put the falling shape into the grid with constant value k */
void put_shape(struct game_state *gs, int k);

/* fit the falling shape into a nearby place, return false if impossible */
int fit_block(struct game_state *gs);

/* load a new random shape as falling in gs or exit the program */
void load_block(struct game_state *gs);

/* initialize gs to a valid game state with empty grid */
void init_game_state(struct game_state *gs);

/* remove any full lines from the game grid in gs and drop higher-up blocks */
void fix_grid(struct game_state *gs);

/* move the falling shape in gs by xinc/0 */
void move_x(struct game_state *gs, int inc);

/* drop the falling shape in gs until it hits the ground */
void drop_block(struct game_state *gs);

/* rotate the falling shape */
void rot_block(struct game_state *gs);

/* External */

/* This function must be implemented externally. It is called when the program ends */
void end_cb(struct game_state *gs);
