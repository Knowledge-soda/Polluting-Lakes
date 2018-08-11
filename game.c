#include<stdlib.h>
#include<stdio.h>

#include"game.h"
#include"outer.h"
#include"main.h"

int main(int argc, char **argv){
    Game *game;
    game = malloc(sizeof(Game));
    if (init(game)){
        fprintf(stderr, "Init Error, SDL: %s\n", SDL_GetError());
        return 1;
    }
    if(loop(game)){
        fprintf(stderr, "Loop Error, SDL: %s\n", SDL_GetError());
        return 1;
    }
    if(end(game)){
        fprintf(stderr, "End Error, SDL: %s\n", SDL_GetError());
        return 1;
    }
    return 0;
}
