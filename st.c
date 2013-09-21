#include <time.h>
#include <stdlib.h>

#include "st.h"

static void end_cb(struct game_state *gs)
{
	int l = gs->lines;

	printf("%d line%s\n", l, l == 1 ? "" : "s");

	SDL_Quit();
	exit(0);
}

static int const_modf(int *s, int v)
{
	return *s;
}

static int orf(int *s, int v)
{
	*s = *s || (v == 1);
	return v;
}

static int iter_shape(struct game_state *gs, int x, int y, mod_f mf, int *s)
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

static int collision(struct game_state *gs, int xinc, int yinc)
{
	int s = 0;
	return !iter_shape(gs, gs->x + xinc, gs->y + yinc, orf, &s) || s;
}

static void put_shape(struct game_state *gs, int k)
{
	iter_shape(gs, gs->x, gs->y, const_modf, &k);
}

static int fit_block(struct game_state *gs)
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

static void load_block(struct game_state *gs)
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

static void init_game_state(struct game_state *gs)
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

static void fix_grid(struct game_state *gs)
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

static void drop_cb(struct game_state *gs)
{
	if (collision(gs, 0, 1)) {

		/*int old = value(gs);*/
		put_shape(gs, 1);
		fix_grid(gs);
		/*printf("%d\n", value(gs) - old);*/
		load_block(gs);
	} else {
		put_shape(gs, 0);
		gs->y += 1;
		put_shape(gs, 4);
	}
}

static void tick_cb(struct game_state *gs)
{
	if (clock() - gs->lastdrop >= gs->delay) {
#if TICK
		drop_cb(gs);
#endif
		gs->lastdrop = clock();
	}

	SDL_RenderPresent(gs->surface);
}

static void draw_cb(struct game_state *gs)
{
	int width = W_WIDTH, heigth = W_HEIGHT;

	int w;
	int cw = width / G_WIDTH;
	int ch = heigth / G_HEIGHT;
	if (cw > ch) {
		w = ch;
		cw = (width - w * G_WIDTH) / 2;
		ch = 0;
	} else {
		w = cw;
		cw = 0;
		ch = (heigth - w * G_HEIGHT) / 2;
	}

	SDL_SetRenderDrawColor(gs->surface, 100, 100, 100, 255);
	SDL_RenderClear(gs->surface);

	int i, j;
	for (i = 0; i < G_WIDTH; i++) {
		for (j = 0; j < G_HEIGHT; j++) {

			int col, clr;
			switch (gs->grid[i][j]) {

			case 1: clr = col = 128;
				break;

			case 4: clr = col = 84;
				break;

			default:
				col = j < 4 ? 26 : 26;
				clr = j < 4 ? 52 : 26;
			}

			SDL_SetRenderDrawColor(gs->surface, clr, col, col, 255);
			SDL_Rect rect;
			rect.x = cw + i * w;
			rect.y = ch + j * w;
			rect.w = rect.h = w;
			SDL_RenderFillRect(gs->surface, &rect);
		}
	}

	SDL_RenderPresent(gs->surface);
}

static void move_x(struct game_state *gs, int inc)
{
	if (collision(gs, inc, 0)) return;

	put_shape(gs, 0);
	gs->x += inc;
	put_shape(gs, 4);
}

static void drop_block(struct game_state *gs)
{
	int k = -1;
	while (!collision(gs, 0, k + 1)) {
		k += 1;
	}

	put_shape(gs, 0);
	gs->y += k;

	drop_cb(gs);
}

static void rot_block(struct game_state *gs)
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

static void key_cb(SDL_Keycode k, int shift, struct game_state *gs)
{
	switch (k)
	{
	case SDLK_r: init_game_state(gs); break;
	case SDLK_q: if (shift) end_cb(gs); break;
	case SDLK_s: drop_block(gs); break;
	case SDLK_a: move_x(gs, -1); break;
	case SDLK_d: move_x(gs, 1); break;
	case SDLK_w: rot_block(gs); break;
	}

	draw_cb(gs);
}

static struct game_state *init_window(int delay)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);

	SDL_Window *screen = SDL_CreateWindow("Tetris",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT, 0);
	SDL_Renderer *r = SDL_CreateRenderer(screen, -1, 0);

	struct game_state *gs = malloc(sizeof(struct game_state));
	gs->surface = r;
	gs->delay = delay;
	init_game_state(gs);

	/*
	g_timeout_add(10, (GSourceFunc) tick_cb, (gpointer) gs);
	*/

	return gs;
}

int main(int argc, char **argv)
{
	struct game_state *gs = init_window(125000);
	SDL_Event ev;
	int mods;
	
	draw_cb(gs);
	while (1) {
		SDL_RenderPresent(gs->surface);
		SDL_WaitEvent(&ev);

		switch (ev.type) {
		case SDL_QUIT:
			end_cb(gs);
			break;
		case SDL_KEYDOWN:
			mods = SDL_GetModState();
			key_cb(ev.key.keysym.sym, mods & KMOD_SHIFT, gs);
			break;
		}
	}

	return 0;
}
