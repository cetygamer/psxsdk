#include <stdio.h>
#include <stdlib.h>

static unsigned int rand_seed = 0;
static unsigned int rand_next = 0;

int abs(int x)
{
	if(x<0)return -x;

	return x;
}

void srand(unsigned int seed)
{
	rand_seed = seed;
}

int rand()
{
	rand_next = (rand_next ^ 0x98765432)*0x1357;
	
	return rand_next % RAND_MAX;
}
