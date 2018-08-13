#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#include"random.h"

#define DBG_SEED (1534173957)

void init_random(RandomSeed *seed){
    time_t t;
    // sfmt_init_gen_rand(&(seed -> sfmt), (uint32_t)time(&t));
    sfmt_init_gen_rand(&(seed -> sfmt), DBG_SEED);
    seed -> array = malloc(4000);
    sfmt_fill_array32(&(seed -> sfmt), seed -> array, 1000);
    seed -> used = 0;
}

int get_random_int(RandomSeed *seed){
    int ret = *((int *)(seed -> array + seed -> used));
    (seed -> used) += sizeof(int);
    if (seed -> used > 3990){
        sfmt_fill_array32(&(seed -> sfmt), seed -> array, 1000);
        seed -> used = 0;
    }
    return ret;
}

unsigned int get_random_sized(RandomSeed *seed, int n){
    unsigned int ret = *((unsigned int *)(seed -> array + seed -> used));
    (seed -> used) += n / 8 + 1;
    if (seed -> used > 3990){
        sfmt_fill_array32(&(seed -> sfmt), seed -> array, 1000);
        seed -> used = 0;
    }
    return ret;
}

int random_below(RandomSeed *seed, int n){
    int k;
    for (k = 0;n >> k;k++);
    int mask = (1 << k) - 1;
    int x = get_random_sized(seed, k) & mask;
    while (x >= n){
        x = get_random_sized(seed, k) & mask;
    }
    return x;
}

int randrange(RandomSeed *seed, int a, int b){
    return a + random_below(seed, b - a);
}
