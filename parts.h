#ifndef SPRITE_INCLUDED
#define SPRITE_INCLUDED

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include"misc.h"

typedef struct particle_type {
    SDL_Texture *texture;
    int w, h;
} ParticleType;

typedef struct particle {
    SDL_Rect rect;
    int type;
} Particle;

typedef struct parts_type {
    ParticleType *types;
    int size;
    int m_size;
} PartsType;

typedef struct parts {
    PartsType *parent;
    Particle *particles;
    int size;
    int m_size;
} Parts;

int init_parts_type(PartsType *type);
int add_particle_type(char *path, PartsType *type, int w, int h, SDL_Renderer *render);
int init_parts(Parts *sprite, PartsType *type);
int add_particle(int type, int x, int y, Parts *sprite);
int blit_parts(Parts *sprite, SDL_Renderer *render);

#endif
