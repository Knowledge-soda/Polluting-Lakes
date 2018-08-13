#ifndef TOOLS_INCLUDED
#define TOOLS_INCLUDED

#include"misc.h"

typedef struct text_box {
    SDL_Texture *texture;
    TTF_Font *font;
    SDL_Color color;
    SDL_Renderer *render;
    SDL_Rect rect;
} TextBox;

SDL_Texture *load_texture(char *path, SDL_Renderer *render);
TextBox *init_text_box(TTF_Font *font, SDL_Renderer *render, int r, int g, int b);
void render_text_box(TextBox *target, char *text);
void blit(TextBox *dst, int x, int y);

#endif
