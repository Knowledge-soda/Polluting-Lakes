#include<stdlib.h>

#include"outer.h"
#include"parts.h"
#include"places.h"

int init(Game *game){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    game -> window = SDL_CreateWindow(
        "Game", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP
    );
    if (!(game -> window)) return SDL_ERROR;
    game -> screen = SDL_GetWindowSurface(game -> window);
    game -> format = game -> screen -> format;
    game -> render = SDL_CreateRenderer(game -> window, -1,
                                        SDL_RENDERER_ACCELERATED);
    SDL_GetWindowSize(game -> window, &(game -> screen_w), &(game -> screen_h));

    game -> places = malloc(sizeof(Places) * 25);
    init_places(game -> places, 14, 10, game -> screen_w, game -> screen_h);

    return 0;
}

int end(Game *game){
    SDL_DestroyWindow(game -> window);
    SDL_Quit();
    return 0;
}
