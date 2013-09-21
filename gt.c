#include <time.h>
#include <cairo.h>
#include <stdlib.h>

#include "gt.h"
#include "val.h"

static void end_cb(struct game_state *gs)
{
	int l = gs->lines;

	printf("%d line%s\n", l, l == 1 ? "" : "s");

	fclose(gs->log);
	gtk_main_quit();
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

static void tick_cb(struct game_state *gs)
{
	if (clock() - gs->lastdrop >= gs->delay) {
#if TICK
		drop_cb(gs);
#endif
		gs->lastdrop = clock();
	}

	gtk_widget_queue_draw(gs->surface);
}

static void draw_cb(GtkWidget *surface, cairo_t *cr, struct game_state *gs)
{
	GtkWindow *top = GTK_WINDOW(gtk_widget_get_toplevel(surface));

	int width, heigth;
	gtk_window_get_size(top, &width, &heigth);

	cr = gdk_cairo_create(gtk_widget_get_window(surface));

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

	int i, j;
	for (i = 0; i < G_WIDTH; i++) {
		for (j = 0; j < G_HEIGHT; j++) {

			float col, clr;
			switch (gs->grid[i][j]) {

			case 1: clr = col = 0.5;
				break;

			case 4: clr = col = 0.3;
				break;

			default:
				col = j < 4 ? 0.1 : 0.1;
				clr = j < 4 ? 0.4 : 0.1;
			}

			cairo_set_source_rgb(cr, clr, col, col);
			cairo_rectangle(cr, cw + i * w, ch + j * w, w, w);
			cairo_fill(cr);
		}
	}

	cairo_destroy(cr);
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

static void key_cb(GtkWidget *window, GdkEventKey *event, struct game_state *gs)
{
	switch (event->keyval)
	{
	case GDK_KEY_r: init_game_state(gs); break;
	case GDK_KEY_Q: end_cb(gs); break;
	case GDK_KEY_s: drop_block(gs); break;
	case GDK_KEY_a: move_x(gs, -1); break;
	case GDK_KEY_d: move_x(gs, 1); break;
	case GDK_KEY_w: rot_block(gs); break;
	}

	gtk_widget_queue_draw(gs->surface);
}

static GtkWidget *init_window(int argc, char **argv, int delay)
{
	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *surface = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), surface);

	struct game_state *gs = malloc(sizeof(struct game_state));
	gs->surface = surface;
	gs->delay = delay;
	gs->log = fopen("log", "w");
	init_game_state(gs);

	g_timeout_add(10, (GSourceFunc) tick_cb, (gpointer) gs);

	g_signal_connect(surface, "draw", G_CALLBACK(draw_cb), gs);
	g_signal_connect(window, "key-press-event", G_CALLBACK(key_cb), gs);
	g_signal_connect(window, "destroy", G_CALLBACK(end_cb), gs);

	return window;
}

int main(int argc, char **argv)
{
	gtk_widget_show_all(init_window(argc, argv, 125000));
	gtk_main();

	return 0;
}
