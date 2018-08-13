#ifndef PLACES_INCLUDED
#define PLACES_INCLUDED

#include"misc.h"
#include"random.h"

typedef unsigned char byte;

typedef struct place {
    SDL_Rect rect, brect;
    int x, y;
    int X, Y;
    int type;
    struct place *dir[8];
    char flag[4];
    char conn[8];
    byte path[8];
    char polluted;
    char selected;
    char comes_from;
    char visible;
} Place;

typedef struct action {
    char dir, comes_from;
    Place *place;
} Action;

typedef struct places {
    int w, h;
    int ready;
    Action *history;
    int history_used, history_size;
    Place *src, *dst;
    Place *places;
    int score;
} Places;

int init_places(Places *places, int w, int h, int screen_w, int screen_h, RandomSeed *seed);
int click(Places *places, int x, int y, SDL_Renderer *render);
int blit_places(Places *places, SDL_Renderer *render);
void undo(Places *places);

#endif
