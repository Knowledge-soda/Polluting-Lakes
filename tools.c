#include<stdlib.h>
#include<stdio.h>

#include"tools.h"


SDL_Texture *load_texture(char *path, Game *game){
    SDL_Texture *ret = NULL;
    SDL_Surface *surface;
    surface = IMG_Load(path);
    if (!surface){
        fprintf(stderr, "Image %s could not be loaded! Error: %s!\n", path, IMG_GetError());
        return NULL;
    }
    ret = SDL_CreateTextureFromSurface(game -> render, surface);
    if (!ret){
        fprintf(stderr, "Texture %s could not be created! Error: %s!\n", path, SDL_GetError());
    }
    SDL_FreeSurface(surface);
    return ret;
}
