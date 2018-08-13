#ifndef PLACES_INCLUDED
#define PLACES_INCLUDED

#include"misc.h"
#include"random.h"

typedef struct place {
    SDL_Rect rect, brect;
    int x, y;
    int X, Y;
    int type;
    struct place *dir[8];
    char flag[4];
    char conn[8];
    char path[8];
    char polluted;
    char selected;
    char comes_from;
} Place;

typedef struct places {
    int w, h;
    Place *src, *dst;
    Place *places;
} Places;

int init_places(Places *places, int w, int h, int screen_w, int screen_h, RandomSeed *seed);
int click(Places *places, int x, int y, SDL_Renderer *render);
int blit_places(Places *places, SDL_Renderer *render);

#endif
