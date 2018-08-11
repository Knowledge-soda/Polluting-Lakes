#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#include"random.h"

void init_random(RandomSeed *seed){
    time_t t;
    sfmt_init_gen_rand(&(seed -> sfmt), (uint32_t)time(&t));
    seed -> array = malloc(sizeof(uint32_t) * 1000);
    sfmt_fill_array32(&(seed -> sfmt), seed -> array, 1000);
    seed -> used = 0;
}

int get_random_int(RandomSeed *seed){
    int ret = (seed -> array)[seed -> used];
    (seed -> used)++;
    if (seed -> used == 1000){
        sfmt_fill_array32(&(seed -> sfmt), seed -> array, 1000);
        seed -> used = 0;
    }
    return ret;
}

int rand_below(RandomSeed *seed, int n){
    int 
}
