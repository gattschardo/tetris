#include "tetris.h"

#define TEST(f) if (!f()) { printf("Test failed: %s\n", #f); exit(1); }
#define ASEQ(a, b) assert_eq(__func__, a, b)

int assert_eq(char const *f, int a, int b)
{
	if (a != b) {
		printf("%s: expected %d, got %d\n", f, b, a);
	}

	return a == b;
}

/* const_modf does not modify state */
int test_const_modf_state_const(void)
{
	int s = 5;
	const_modf(&s, 3);
	return ASEQ(s, 5);
}

/* const_modf returns state as result */
int test_const_modf_ret_const(void)
{
	int s = 3;
	int v = 7;
	int r = const_modf(&s, v);
	return ASEQ(r, 3);
}

void end_cb(struct game_state *gs)
{
}

int main(void)
{
	TEST(test_const_modf_state_const);
	TEST(test_const_modf_ret_const);

	return 0;
}
