#ifndef PLACES_INCLUDED
#define PLACES_INCLUDED

#include"misc.h"
#include"random.h"

typedef struct place {
    SDL_Rect rect;
    int x, y;
    int type;
    struct place *right, *down, *r_diag, *l_diag;
    char flag[4];
} Place;

typedef struct places {
    int w, h;
    Place *places;
} Places;

int init_places(Places *places, int w, int h, int screen_w, int screen_h, RandomSeed *seed);
int blit_places(Places *places, SDL_Renderer *render);

#endif
