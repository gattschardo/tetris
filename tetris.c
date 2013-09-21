#include "val.h"
#include "tetris.h"

int const_modf(int *s, int v)
{
	return *s;
}

int orf(int *s, int v)
{
	*s = *s || (v == 1);
	return v;
}

int iter_shape(struct game_state *gs, int x, int y, mod_f mf, int *s)
{
	int xf, yf, xa, ya, xx;
	switch (gs->rot) {
	case 0: xa = 0; xf = 1;
		ya = 0; yf = 1;
		xx = 1; break;

	case 1: xa = 3; xf = -1;
		ya = 0; yf = 1;
		xx = 0; break;

	case 2: xa = 3; xf = -1;
		ya = 3; yf = -1;
		xx = 1; break;

	case 3: xa = 0; xf = 1;
		ya = 3; yf = -1;
		xx = 0;
	}

	int i, j, v = 1, p = 1;
	enum shape_t shape = gs->shape;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (shape & p) {

				int xp = x + xa + xf*(xx ? j : i);
				int yp = y + ya + yf*(xx ? i : j);

				if (xp < 0 || xp >= G_WIDTH || yp < 0 || yp >= G_HEIGHT) {

					return 0;
				}

				v = gs->grid[xp][yp];
				v = mf(s, v);
				gs->grid[xp][yp] = v;
			}

			p = p << 1;
		}
	}

	return 1;
}

int collision(struct game_state *gs, int xinc, int yinc)
{
	int s = 0;
	return !iter_shape(gs, gs->x + xinc, gs->y + yinc, orf, &s) || s;
}

void put_shape(struct game_state *gs, int k)
{
	iter_shape(gs, gs->x, gs->y, const_modf, &k);
}

int fit_block(struct game_state *gs)
{
	int xds[] = {0, -1, 1, -2, 2};
	int xdc = sizeof(xds) / sizeof(xds[0]);

	int i, xd, yd;
	for (yd = 0; yd > -2; yd -= 1) {
		for (i = 0; i < xdc; i += 1) {
			xd = xds[i];
			if (!collision(gs, xd, yd)) {
				gs->x += xd;
				gs->y += yd;
				return 1;
			}
		}
	}

	return 0;
}

void load_block(struct game_state *gs)
{
	enum shape_t all_shapes[] = { SHAPE_BOX, SHAPE_LOW,
		SHAPE_MID, SHAPE_TOP, SHAPE_LEF, SHAPE_RIG,
		SHAPE_BAR };
	int k = sizeof(all_shapes) / sizeof(all_shapes[0]);

	gs->shape = all_shapes[rand()%k];
	gs->y = 0;

	if (!fit_block(gs)) {
		end_cb(gs);
	}

	put_shape(gs, 4);
}

void init_game_state(struct game_state *gs)
{
	gs->x = G_WIDTH / 2;
	gs->lines = 0;
	gs->rot = 0;

	int i,j;
	for (i = 0; i < G_WIDTH; i++) {
		for (j = 0; j < G_HEIGHT; j++) {
			gs->grid[i][j] = 0;
		}
	}

	load_block(gs);
	gs->lastdrop = clock();
}

static void drop_cb(struct game_state *gs)
{
	if (collision(gs, 0, 1)) {

		int old = value(gs);
		put_shape(gs, 1);
		fix_grid(gs);
		fprintf(gs->log, "%d %d\n", old, gs->lines);
		printf("%d\n", value(gs) - old);
		load_block(gs);
	} else {
		put_shape(gs, 0);
		gs->y += 1;
		put_shape(gs, 4);
	}
}

void fix_grid(struct game_state *gs)
{
	int x, y;
	for (y = G_HEIGHT - 1; y > -1; y--) {

		int full = 1;
		for (x = 0; x < G_WIDTH; x++) {
			full = full && gs->grid[x][y] == 1;
		}

		if (full) {

			int i, j;
			for (j = y; j > 0; j--) {
				for (i = 0; i < G_WIDTH; i++) {
					gs->grid[i][j] = gs->grid[i][j-1];
				}
			}
			gs->lines += 1;
			y += 1;
		}
	}
}

void move_x(struct game_state *gs, int inc)
{
	if (collision(gs, inc, 0)) return;

	put_shape(gs, 0);
	gs->x += inc;
	put_shape(gs, 4);
}

void drop_block(struct game_state *gs)
{
	int k = -1;
	while (!collision(gs, 0, k + 1)) {
		k += 1;
	}

	put_shape(gs, 0);
	gs->y += k;

	drop_cb(gs);
}

void rot_block(struct game_state *gs)
{
	int x0 = gs->x;
	int y0 = gs->y;
	int r0 = gs->rot;
	int r1 = (r0 + 1) % 4;

	gs->rot = r1;
	if (!fit_block(gs)) {
		r1 = r0;
	}

	int k = 0;
	gs->rot = r0;
	iter_shape(gs, x0, y0, const_modf, &k);

	gs->rot = r1;
	put_shape(gs, 4);
}

