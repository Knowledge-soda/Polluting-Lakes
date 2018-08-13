#include<stdlib.h>
#include<stdio.h>

#include"tools.h"
#include"game.h"

SDL_Texture *load_texture(char *path, SDL_Renderer *render){
    SDL_Texture *ret = NULL;
    SDL_Surface *surface;
    surface = IMG_Load(path);
    if (!surface){
        fprintf(stderr, "Image %s could not be loaded! Error: %s!\n", path, IMG_GetError());
        return NULL;
    }
    ret = SDL_CreateTextureFromSurface(render, surface);
    if (!ret){
        fprintf(stderr, "Texture %s could not be created! Error: %s!\n", path, SDL_GetError());
    }
    SDL_FreeSurface(surface);
    return ret;
}

TextBox *init_text_box(TTF_Font *font, SDL_Renderer *render, int r, int g, int b){
    TextBox *ret;
    ret = malloc(sizeof(TextBox));
    ret -> font = font;
    ret -> render = render;
    ret -> color.r = r;
    ret -> color.g = g;
    ret -> color.b = b;
    return ret;
}

void render_text_box(TextBox *target, char *text){
    SDL_Surface *surface;
    surface = TTF_RenderText_Blended(target -> font, text, target -> color);
    target -> rect.w = surface -> w;
    target -> rect.h = surface -> h;
    if (!surface){
        fprintf(stderr, "Image could not be rendered! Error: %s!\n", IMG_GetError());
        return;
    }
    target -> texture = SDL_CreateTextureFromSurface(target -> render, surface);
    if (!target -> texture){
        fprintf(stderr, "Texture could not be created! Error: %s!\n", SDL_GetError());
    }
    SDL_FreeSurface(surface);
}

void blit(TextBox *dst, int x, int y){
    dst -> rect.x = x;
    dst -> rect.y = y;
    SDL_RenderCopy(dst -> render, dst -> texture, NULL, &(dst -> rect));
    return;
}

