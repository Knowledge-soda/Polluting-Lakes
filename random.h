#ifndef RANDOM_INCLUDED
#define RANDOM_INCLUDED

#include"SFMT/SFMT.h"

typedef struct random_seed {
    sfmt_t sfmt;
    uint32_t *array;
    int used;
} RandomSeed;

void init_random(RandomSeed *seed);
int get_random_int(RandomSeed *seed);
int rand_below(RandomSeed *seed, int n)

#endif
