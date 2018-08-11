#ifndef PLACES_INCLUDED
#define PLACES_INCLUDED

#include"misc.h"

typedef struct place {
    SDL_Rect rect;
    int x, y;
    struct place *right, *down, *r_diag, *l_diag;
} Place;

typedef struct places {
    int w, h;
    Place *places;
} Places;

int init_places(Places *places, int w, int h, int screen_w, int screen_h);
int blit_places(Places *places, SDL_Renderer *render);

#endif
