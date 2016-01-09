#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "tetris.h"

void end_cb(struct game_state *gs)
{
	int l = gs->lines;

	printf("%d line%s\n", l, l == 1 ? "" : "s");

	SDL_Quit();
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

static void key_cb(SDL_Keycode k, int shift, struct game_state *gs)
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
	gs->log = stdout;
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
