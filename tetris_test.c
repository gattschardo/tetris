#include "tetris.h"

/* const_modf does not modify state */
int test_const_modf_state_const(void)
{
	int s = 5;
	const_modf(&s, 3);
	return s == 5;
}

/* const_modf returns state as result */
int test_const_modf_ret_const(void)
{
	int s = 3;
	int v = 7;
	int r = const_modf(&s, v);
	return r == 3;
}

#define TEST(f) if (!f()) { printf("Test failed: %s\n", #f); exit(1); };

void end_cb(struct game_state *gs)
{
}

int main(void)
{
	TEST(test_const_modf_state_const)
	TEST(test_const_modf_ret_const)

	return 0;
}
