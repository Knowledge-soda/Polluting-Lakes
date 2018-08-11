#include<stdlib.h>

#include"main.h"
#include"parts.h"
#include"places.h"

int loop(Game *game){
    int run = 1;
    SDL_Event event;

    while (run){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) run = 0;
        }

        SDL_SetRenderDrawColor(game -> render,
                            255, 255, 255, 255);
        SDL_RenderClear(game -> render);
        blit_places(game -> places, game -> render);
        SDL_RenderPresent(game -> render);
    }
    return 0;
}
