#include<stdlib.h>
#include<stdio.h>

#include"parts.h"


int load_particle_type(char *path, ParticleType *type, int w, int h, SDL_Renderer *render){
    type -> texture = NULL;
    SDL_Surface *surface;
    surface = IMG_Load(path);
    if (!surface){
        fprintf(stderr, "Image %s could not be loaded! Error: %s!\n", path, IMG_GetError());
        return IMG_ERROR;
    }

    if (w) type -> w = w;
    else type -> w = surface -> w;
    if (h) type -> h = h;
    else type -> h = surface -> h;

    type -> texture = SDL_CreateTextureFromSurface(render, surface);
    if (!(type -> texture)){
        fprintf(stderr, "Texture %s could not be created! Error: %s!\n", path, SDL_GetError());
        SDL_FreeSurface(surface);
        return SDL_ERROR;
    }
    SDL_FreeSurface(surface);
    return NO_ERROR;
}

int init_parts_type(PartsType *type){
    type -> types = malloc(sizeof(ParticleType) * 16);
    if (!(type -> types)){
        fprintf(stderr, "Couldn't init sprite type!\n");
        return MEM_ERROR;
    }
    type -> size = 0;
    type -> m_size = 16;
    return NO_ERROR;
}

int add_particle_type(char *path, PartsType *type, int w, int h, SDL_Renderer *render){
    if ((type -> size) == (type -> m_size)){
        (type -> m_size) *= 2;
        (type -> types) = realloc(type -> types, type -> m_size);
        if (!(type -> types)){
            fprintf(stderr, "Couldn't realloc memory!\n");
            return MEM_ERROR;
        }
    }
    int err;
    err = load_particle_type(path, (type -> types) + (type -> size), w, h, render);
    if (err) return err;
    (type -> size)++;
    return NO_ERROR;
}

int init_parts(Parts *sprite, PartsType *type){
    sprite -> parent = type;
    sprite -> particles = malloc(sizeof(ParticleType) * 16);
    if (!(sprite -> particles)){
        fprintf(stderr, "Couldn't init sprite!\n");
        return MEM_ERROR;
    }
    sprite -> size = 0;
    sprite -> m_size = 16;
    return NO_ERROR;
}

int add_particle(int type, int x, int y, Parts *sprite){
    if ((sprite -> size) == (sprite -> m_size)){
        (sprite -> m_size) *= 2;
        (sprite -> particles) = realloc(sprite -> particles,
                   sizeof(ParticleType) * (sprite -> m_size));
        if (!(sprite -> particles)){
            fprintf(stderr, "Couldn't realloc memory!\n");
            return MEM_ERROR;
        }
    }
    Particle *part;
    part = (sprite -> particles) + (sprite -> size);
    part -> type = type;
    part -> rect.x = x;
    part -> rect.y = y;
    part -> rect.w = (sprite -> parent -> types)[type].w;
    part -> rect.h = (sprite -> parent -> types)[type].h;
    (sprite -> size)++;
    return NO_ERROR;
}

int blit_parts(Parts *sprite, SDL_Renderer *render){
    Particle *parts;
    ParticleType *types;
    int i;
    parts = sprite -> particles;
    types = sprite -> parent -> types;
    for (i = 0;i < sprite -> size;i++){
        SDL_RenderCopy(render, types[parts[i].type].texture,
                       NULL, &(parts[i].rect));
    }
    return NO_ERROR;
}
