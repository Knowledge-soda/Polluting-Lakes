#include<stdlib.h>
#include<stdio.h>

#include"main.h"
#include"parts.h"
#include"places.h"
#include"tools.h"

int loop(Game *game){
    int run = 1, state = 0;
    char score[10];
    SDL_Event event;

    while (run){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) run = 0;
            else if (event.type == SDL_MOUSEBUTTONDOWN){
                if (event.button.button == SDL_BUTTON_LEFT){
                    click(game -> places, event.button.x, event.button.y, game -> render);
                }
            } else if (event.type == SDL_KEYDOWN){
                if (event.key.keysym.sym == SDLK_u){
                    undo(game -> places);
                }
                if (event.key.keysym.sym == SDLK_RETURN){
                    state++;
                }
            }
        }
        if (state == 0){
            SDL_SetRenderDrawColor(game -> render,
                                0, 0, 0, 255);
            SDL_RenderClear(game -> render);
            blit(game -> init_screen, 20, 20);
        } else if (state == 1){
            SDL_SetRenderDrawColor(game -> render,
                                255, 255, 255, 255);
            SDL_RenderClear(game -> render);
            blit_places(game -> places, game -> render);
            sprintf(score, "%i", game -> places -> score);
            render_text_box(game -> score, score);
            blit(game -> score, 20, 20);
            if (game -> places -> score == 10) state++;
        } else if (state == 2){
            SDL_SetRenderDrawColor(game -> render,
                                0, 0, 0, 255);
            SDL_RenderClear(game -> render);
            blit(game -> end_screen, 20, 20);
        } else {
            run = 0;
        }
        SDL_RenderPresent(game -> render);
    }
    return 0;
}
