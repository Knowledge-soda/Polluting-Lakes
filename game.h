#ifndef GAME_INCLUDED
#define GAME_INCLUDED

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

#include"misc.h"
#include"parts.h"
#include"places.h"
#include"random.h"
#include"tools.h"

typedef struct game {
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_PixelFormat *format;
    SDL_Renderer *render;
    Places *places;
    int screen_w, screen_h;
    RandomSeed *seed;
    TTF_Font *font;
    TextBox *init_screen;
    TextBox *score;
    TextBox *end_screen;
} Game;

#endif
