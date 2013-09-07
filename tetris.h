#include <gtk/gtk.h>

#define TICK 0
#define G_WIDTH 10
#define G_HEIGHT 20

enum shape_t {
	SHAPE_BOX = 0x0660, SHAPE_LOW = 0x0622,
	SHAPE_MID = 0x0262, SHAPE_TOP = 0x2260,
	SHAPE_LEF = 0x2640, SHAPE_RIG = 0x4620,
	SHAPE_BAR = 0x2222,
};

struct game_state {
	int grid[G_WIDTH][G_HEIGHT];
	GtkWidget *surface;
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
