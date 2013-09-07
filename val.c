#include "val.h"

int value(struct game_state *gs)
{
	int i, j, v = 0;
	for (i = 0; i < G_WIDTH; i++) {
		int l = 1, f = 1;
		for (j = G_HEIGHT - 1; j > -1; j--) {
			if (gs->grid[i][j] == 1) {
				l = 1;
				v += f;
			} else {
				if (l == 1) {
					f *= 2;
				}
				l = 0;
			}
		}
	}

	return v;
}

