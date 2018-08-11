#include<stdlib.h>
#include<stdio.h>

#include"places.h"

#define CLR_EDGE (25)
#define PLACE_SIZE (10)

int init_places(Places *places, int w, int h, int screen_w, int screen_h, RandomSeed *seed){
    int Dx = (screen_w - CLR_EDGE * 2 - PLACE_SIZE) / (w - 1);
    int Dy = (screen_h - CLR_EDGE * 2 - PLACE_SIZE) / (h - 1);
    int x, y;
    places -> w = w;
    places -> h = h;
    places -> places = malloc(sizeof(Place) * w * h);
    if (!(places -> places)){
        fprintf(stderr, "Couldn't allocate places\n");
    }
    Place *f = places -> places;
    Place *tmp;
    for (y = h - 1;y >= 0;y--)
    for (x = w - 1;x >= 0;x--){
        tmp = f + (w * y) + x;
        tmp -> x = CLR_EDGE + PLACE_SIZE / 2 + x * Dx + randrange(seed, -(Dx / 3), Dx / 3);
        tmp -> y = CLR_EDGE + PLACE_SIZE / 2 + y * Dy + randrange(seed, -(Dy / 3), Dy / 3);
        (tmp -> rect).x = tmp -> x - PLACE_SIZE / 2;
        (tmp -> rect).y = tmp -> y - PLACE_SIZE / 2;
        (tmp -> rect).w = PLACE_SIZE;
        (tmp -> rect).h = PLACE_SIZE;
        if (y < h - 1){
            tmp -> down = f + (w * y) + w + x;
            if (x < w - 1) tmp -> r_diag = f + (w * y) + w + 1 + x;
            else tmp -> r_diag = NULL;
            if (x) tmp -> l_diag = f + (w * y) + w - 1 + x;
            else tmp -> l_diag = NULL;
        } else {
            tmp -> down = NULL;
            tmp -> r_diag = NULL;
            tmp -> l_diag = NULL;
        }
        if (x < w - 1) tmp -> right = f + (w * y) + 1 + x;
        else tmp -> right = NULL;
        tmp -> flag_d = random_below(seed, 2);
        tmp -> flag_r = random_below(seed, 2);
        tmp -> flag_rd = random_below(seed, 2);
        tmp -> flag_ld = random_below(seed, 2);
    }

    return 0;
}

int blit_places(Places *places, SDL_Renderer *render){
    Place *map;
    Place *cur;
    map = places -> places;
    int x, y, w, h;
    w = places -> w;
    h = places -> h;

    SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
    for (y = 0;y < h;y++)
    for (x = 0;x < w;x++){
        SDL_RenderFillRect(render, &(map[y * w + x].rect));
    }

    SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
    for (y = 0;y < h;y++)
    for (x = 0;x < w;x++){
        cur = map + y * w + x;
        if (cur -> right && cur -> flag_r) SDL_RenderDrawLine(render,
                cur -> x, cur -> y, cur -> right -> x, cur -> right -> y);
        if (cur -> down && cur -> flag_d) SDL_RenderDrawLine(render,
                cur -> x, cur -> y, cur -> down -> x, cur -> down -> y);
        if (cur -> r_diag && cur -> flag_rd) SDL_RenderDrawLine(render,
                cur -> x, cur -> y, cur -> r_diag -> x, cur -> r_diag -> y);
        if (cur -> l_diag && cur -> flag_ld) SDL_RenderDrawLine(render,
                cur -> x, cur -> y, cur -> l_diag -> x, cur -> l_diag -> y);
    }

    return 0;
}
