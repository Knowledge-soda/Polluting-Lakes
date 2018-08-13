#include<stdlib.h>

#include"outer.h"
#include"parts.h"
#include"places.h"
#include"tools.h"

int init(Game *game){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    game -> window = SDL_CreateWindow(
        "Game", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP
    );
    if (!(game -> window)) return SDL_ERROR;
    TTF_Init();
    game -> screen = SDL_GetWindowSurface(game -> window);
    game -> format = game -> screen -> format;
    game -> render = SDL_CreateRenderer(game -> window, -1,
                                        SDL_RENDERER_ACCELERATED);
    SDL_GetWindowSize(game -> window, &(game -> screen_w), &(game -> screen_h));
    SDL_SetRenderDrawBlendMode(game -> render, SDL_BLENDMODE_BLEND);

    game -> seed = malloc(sizeof(RandomSeed));
    init_random(game -> seed);

    game -> font = TTF_OpenFont("trench.100.otf", 48);
    
    game -> init_screen = init_text_box(game -> font, game -> render, 255, 0, 0);
    render_text_box(game -> init_screen, "Your company needs more lakes to pollute!");
    
    game -> end_screen = init_text_box(game -> font, game -> render, 255, 0, 0);
    render_text_box(game -> end_screen, "Congratulations, you and your company destroyed all nature here!");

    game -> score = init_text_box(game -> font, game -> render, 255, 0, 0);
    render_text_box(game -> score, "0");
    
    game -> places = malloc(sizeof(Places) * 25);
    init_places(game -> places, 14, 10, game -> screen_w, game -> screen_h, game -> seed);

    return 0;
}

int end(Game *game){
    SDL_DestroyWindow(game -> window);
    SDL_Quit();
    return 0;
}
