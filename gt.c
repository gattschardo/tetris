#include <cairo.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "tetris.h"
#include "val.h"

void end_cb(struct game_state *gs)
{
	int l = gs->lines;

	printf("%d line%s\n", l, l == 1 ? "" : "s");

	fclose(gs->log);
	gtk_main_quit();
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
