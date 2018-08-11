#ifndef RANDOM_INCLUDED
#define RANDOM_INCLUDED

#define SFMT_MEXP (19937)
#include"SFMT/SFMT.h"

typedef struct random_seed {
    sfmt_t sfmt;
    void *array;
    int used;
} RandomSeed;

void init_random(RandomSeed *seed);
int get_random_int(RandomSeed *seed);
int random_below(RandomSeed *seed, int n);
int randrange(RandomSeed *seed, int a, int b);

#endif
