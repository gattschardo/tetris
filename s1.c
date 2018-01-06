#include <time.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif

#include "tetris.h"

static void clear(struct SDL_Surface *s, struct SDL_Color c)
{
	uint32_t col = SDL_MapRGB(s->format, c.r, c.g, c.b);
	for (int i = 0; i < s->h; i++)
	{
		for (int j = 0; j < s->w; j++)
		{
			*((uint32_t*)s->pixels + i * s->w + j) = col;
		}
	}
}

static void fill(struct SDL_Surface *s, struct SDL_Rect *rect, struct SDL_Color c)
{
	uint32_t col = SDL_MapRGB(s->format, c.r, c.g, c.b);
	for (int i = rect->y; i < rect->y + rect->h; i++)
	{
		for (int j = rect->x; j < rect->x + rect->w; j++)
		{
			*((uint32_t*)s->pixels + i * s->w + j) = col;
		}
	}
}

void end_cb(struct game_state *gs)
{
	int l = gs->lines;

	printf("%d line%s\n", l, l == 1 ? "" : "s");

	SDL_Quit();
#ifdef __EMSCRIPTEN__
	emscripten_cancel_main_loop();
#endif
	exit(0);
}

static void tick_cb(struct game_state *gs)
{
	if (clock() - gs->lastdrop >= gs->delay) {
#if TICK
		drop_cb(gs);
#endif
		gs->lastdrop = clock();
	}

	SDL_Flip(gs->surface);
}

static void draw_cb(struct game_state *gs)
{
	//if (SDL_MUSTLOCK(gs->surface))
	{
		SDL_LockSurface(gs->surface);
	}

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

	struct SDL_Color bg_col = {100, 100, 100, 255};
	clear(gs->surface, bg_col);

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

			SDL_Rect rect;
			rect.x = cw + i * w;
			rect.y = ch + j * w;
			rect.w = rect.h = w;
			struct SDL_Color rect_col = {clr, col, col, 255};
			fill(gs->surface, &rect, rect_col);
		}
	}

	//if (SDL_MUSTLOCK(gs->surface))
	{
		SDL_UnlockSurface(gs->surface);
	}
	SDL_Flip(gs->surface);
}

static void key_cb(SDLKey k, int shift, struct game_state *gs)
{
	switch (k)
	{
	case SDLK_r: init_game_state(gs); break;
	case SDLK_q: if (shift) end_cb(gs); break;
	case SDLK_j:
	case SDLK_s: drop_block(gs); break;
	case SDLK_h:
	case SDLK_a: move_x(gs, -1); break;
	case SDLK_l:
	case SDLK_d: move_x(gs, 1); break;
	case SDLK_k:
	case SDLK_w: rot_block(gs); break;
	default: break; // ignore other keys
	}

	draw_cb(gs);
}

static struct game_state *init_window(int delay)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	SDL_WM_SetCaption("Tetris", NULL);
	SDL_Surface *r = SDL_SetVideoMode(W_WIDTH, W_HEIGHT, 32, SDL_SWSURFACE);
	printf("screen: %p\n", r);

	struct game_state *gs = malloc(sizeof(struct game_state));
	gs->surface = r;
	gs->delay = delay;
	gs->log = stdout;
	init_game_state(gs);

	/*
	g_timeout_add(10, (GSourceFunc) tick_cb, (gpointer) gs);
	*/

	return gs;
}

void loop(struct game_state *gs)
{
	SDL_Event ev;
	int mods;
	while (SDL_PollEvent(&ev)) {
		SDL_Flip(gs->surface);

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
}

int main(int argc, char **argv)
{
	struct game_state *gs = init_window(125000);
	
	draw_cb(gs);
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(loop, gs, 60, 1);
#else
	while (1) {
		loop(gs);
		SDL_Delay(16);
	}
#endif

	return 0;
}
