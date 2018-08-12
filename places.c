#include<stdlib.h>
#include<stdio.h>

#include"places.h"

#define CLR_EDGE (25)
#define PLACE_SIZE (10)

#define TYPE_CROSS (0)
#define TYPE_SOURCE (1)
#define TYPE_PLACE (2)

int places_reds[]   = {0,  0,255};
int places_greens[] = {255,0,0};
int places_blues[]  = {0,  0,0};

static int rnd_x[] = {1, 1, 0, -1, -1, -1,  0,  1};
static int rnd_y[] = {0, 1, 1,  1,  0, -1, -1, -1};

static int is_possible(Place *map, char *visited, int w, int h, int x, int y, int rnd){
    int dx = rnd_x[rnd];
    int dy = rnd_y[rnd];
    if (x + dx < 0) return 0;
    if (y + dy < 0) return 0;
    if (x + dx == w) return 0;
    if (y + dy == h) return 0;
    if (visited[(y + dy) * w + x + dx]) return 0;
    if (rnd < 4) return !(map[y * w + x].flag[rnd]);
    return !(map[(y + dy) * w + x + dx].flag[rnd - 4]); 
}

static int diff_dir(int dx, int dy){
    dx++;
    dy++;
    int ret = dy * 3 + dx;
    if (ret > 4) return ret - 1;
    return ret;
}

static void dir_diff(int dir, int *dx, int *dy){
    if (dir == -1){
        (*dx) = -2;
        (*dy) = -1;
        return;
    }
    if (dir > 4) dir++;
    (*dx) = dir % 3;
    (*dy) = dir / 3;
    (*dx)--;
    (*dy)--;
}

static int connect(Places *places, int x1, int y1, int x2, int y2, RandomSeed *seed){
    int w = places -> w;
    Place *map;
    map = places -> places;
    int dx = -2, dy = -1, oldir;

    while (x1 != x2 || y1 != y2){
        oldir = diff_dir(dx, dy);
        dy = 0;
        dx = 0;
        if (x2 - x1 > 0) dx = random_below(seed, 2);
        if (x2 - x1 < 0) dx = -random_below(seed, 2);
        if (y2 - y1 > 0) dy = random_below(seed, 2);
        if (y2 - y1 < 0) dy = -random_below(seed, 2);
        if (!dx && !dy){
            dir_diff(oldir, &dx, &dy);
            continue;
        }
        if (dy == 1){
            if (dx == 1) map[y1 * w + x1].flag[1] = 1;
            if (dx == 0) map[y1 * w + x1].flag[2] = 1;
            if (dx == -1) map[y1 * w + x1].flag[3] = 1;
        } else if (!dy && dx == 1){
            map[y1 * w + x1].flag[0] = 1;
        }

        if (oldir >= 0) map[y1 * w + x1].path[oldir] = diff_dir(dx, dy);

        x1 += dx;
        y1 += dy;
        if (dy == -1){
            if (dx == 1) map[y1 * w + x1].flag[3] = 1;
            if (dx == 0) map[y1 * w + x1].flag[2] = 1;
            if (dx == -1) map[y1 * w + x1].flag[1] = 1;
        } else if (!dy && dx == -1){
            map[y1 * w + x1].flag[0] = 1;
        }
    }
    return 0;
}


    /* old connect
    int w = places -> w;
    int h = places -> h;
    Place *map;
    map = places -> places;
    int dx, dy, rnd, poss, i;
    char visited[h * w];
    for (dy = 0;dy < h;dy++)
    for (dx = 0;dx < w;dx++)
        visited[dy * w + dx] = 0;

    int stack[w * h], s_size = 0;
    while (x1 != x2 || y1 != y2){
        poss = 0;
        for (i = 0;i < 8;i++){
            if (is_possible(map, visited, w, h, x1, y1, i)) poss = 1;
        }
        printf("poss=%i\n", poss);
        if (!poss){
            visited[y1 * w + x1] = 1;
            s_size--;
            printf("%i\n", s_size);
            if (s_size < 0) return 1;
            rnd = stack[s_size];
            printf("x=%i, y=%i\n", x1, y1);
            if (rnd > 3) map[y1 * w + x1].flag[rnd - 4] = 0;
            x1 += rnd_x[rnd];
            y1 += rnd_y[rnd];
            if (rnd < 4) map[y1 * w + x1].flag[rnd] = 0;
            continue;
        }
        printf("PROK\n");
        rnd = random_below(seed, 8);
        printf("OK\n");
        dx = rnd_x[rnd];
        dy = rnd_y[rnd];
        if (is_possible(map, visited, w, h, x1, y1, rnd)){
            if (rnd < 4) map[y1 * w + x1].flag[rnd] = 1;
            stack[s_size] = rnd;
            printf("ss=%i\n", s_size);
            s_size++;
            x1 += dx;
            y1 += dy;
            if (rnd > 3) map[y1 * w + x1].flag[rnd - 4] = 1;
        }
    }
    return 0;
    */

static void pick_rand_normal(Place *map, int *x, int *y, int w, int h, RandomSeed *seed){
    *x = random_below(seed, w);
    *y = random_below(seed, h);
    while (map[*y * w + *x].type != TYPE_CROSS){
        *x = random_below(seed, w);
        *y = random_below(seed, h);
    }
}

static int generate(Places *places, RandomSeed *seed){
    int w = places -> w;
    int h = places -> h;
    Place *map;
    map = places -> places;

    int s_x = random_below(seed, w);
    int s_y = random_below(seed, h);
    map[s_y * w + s_x].type = TYPE_SOURCE;

    int i, j, x, y, x2, y2;
    for (i = 0;i < 10;i++){
        x = s_x;
        y = s_y;
        for (j = 0;j < 3;j++){
            pick_rand_normal(map, &x2, &y2, w, h, seed);
            connect(places, x, y, x2, y2, seed);
            x = x2;
            y = y2;
        }
        map[y2 * w + x2].type = TYPE_PLACE;
    }
    return 0;
}

static int invalid_place(int x, int y, int screen_w, int screen_h){
    return (x - PLACE_SIZE / 2 < 0) || (x + PLACE_SIZE / 2 > screen_w) ||
           (y - PLACE_SIZE / 2 < 0) || (y + PLACE_SIZE / 2 > screen_h);
}

int init_places(Places *places, int w, int h, int screen_w, int screen_h, RandomSeed *seed){
    int Dx = (screen_w - CLR_EDGE * 2 - PLACE_SIZE) / (w - 1);
    int Dy = (screen_h - CLR_EDGE * 2 - PLACE_SIZE) / (h - 1);
    int x, y, i;
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
        tmp -> type = TYPE_CROSS;
        tmp -> x = CLR_EDGE + PLACE_SIZE / 2 + x * Dx + randrange(seed, -(Dx / 3), Dx / 3);
        tmp -> y = CLR_EDGE + PLACE_SIZE / 2 + y * Dy + randrange(seed, -(Dy / 3), Dy / 3);
        while (invalid_place(tmp -> x, tmp -> y, screen_w, screen_h)){
            tmp -> x = CLR_EDGE + PLACE_SIZE / 2 + x * Dx + randrange(seed, -(Dx / 3), Dx / 3);
            tmp -> y = CLR_EDGE + PLACE_SIZE / 2 + y * Dy + randrange(seed, -(Dy / 3), Dy / 3);
        }
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
        for (i = 0;i < 8;i++){
            tmp -> flag[i] = 0;
        }
        for (i = 0;i < 8;i++){
            tmp -> path[i] = -1;
        }
    }

    generate(places, seed);

    return 0;
}


int blit_places(Places *places, SDL_Renderer *render){
    Place *map;
    Place *cur;
    map = places -> places;
    int x, y, w, h;
    w = places -> w;
    h = places -> h;

    for (y = 0;y < h;y++)
    for (x = 0;x < w;x++){
        SDL_SetRenderDrawColor(render,
                places_reds[map[y * w + x].type],
                places_greens[map[y * w + x].type],
                places_blues[map[y * w + x].type],
                255);
        SDL_RenderFillRect(render, &(map[y * w + x].rect));
    }

    SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
    for (y = 0;y < h;y++)
    for (x = 0;x < w;x++){
        cur = map + y * w + x;
        if (cur -> right && cur -> flag[0]) SDL_RenderDrawLine(render,
                cur -> x, cur -> y, cur -> right -> x, cur -> right -> y);
        if (cur -> down && cur -> flag[2]) SDL_RenderDrawLine(render,
                cur -> x, cur -> y, cur -> down -> x, cur -> down -> y);
        if (cur -> r_diag && cur -> flag[1]) SDL_RenderDrawLine(render,
                cur -> x, cur -> y, cur -> r_diag -> x, cur -> r_diag -> y);
        if (cur -> l_diag && cur -> flag[3]) SDL_RenderDrawLine(render,
                cur -> x, cur -> y, cur -> l_diag -> x, cur -> l_diag -> y);
    }

    return 0;
}
