#include<stdlib.h>
#include<stdio.h>

#include"places.h"

#define CLR_EDGE (25)
#define PLACE_SIZE (10)
#define PLACE_BSIZE (20)

#define TYPE_CROSS (0)
#define TYPE_SOURCE (1)
#define TYPE_PLACE (2)

int places_reds[]   = {0,  0,255};
int places_greens[] = {255,0,0};
int places_blues[]  = {0,  0,0};

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
    if (dir >= 4) dir++;
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
        if (diff_dir(dx, dy) >= 4){
            map[y1 * w + x1].flag[diff_dir(dx, dy) - 4] = 1;
        }
        map[y1 * w + x1].conn[diff_dir(dx, dy)] = 1;

        if (oldir >= 0) map[y1 * w + x1].path[oldir] = diff_dir(dx, dy);

        x1 += dx;
        y1 += dy;
        map[y1 * w + x1].conn[diff_dir(-dx, -dy)] = 1;
        if (diff_dir(dx, dy) <= 3){
            map[y1 * w + x1].flag[3 - diff_dir(dx, dy)] = 1;
        }
    }
    return 0;
}

static void pollute_dir(Place *map, int w, int x, int y, int dir){
    int dx, dy;
    dir_diff(dir, &dx, &dy);
    if (dir > 3){
        map[y * w + x].flag[dir - 4] = 2;
    } else {
        map[(y + dy) * w + x + dx].flag[3 - dir] = 2;
    }
}

static void click_place(Place *map, int w, int x, int y){
    int dir, dx, dy;
    for (dir = 0;dir < 8;dir++){
        if (map[y * w + x].conn[dir]){
            dir_diff(dir, &dx, &dy);
            if (!map[(y + dy) * w + x + dx].polluted){
                map[(y + dy) * w + x + dx].polluted = 2;
            }
        }
    }
}

static void declick_place(Place *map, int w, int x, int y){
    int dir, dx, dy;
    for (dir = 0;dir < 8;dir++){
        if (map[y * w + x].conn[dir]){
            dir_diff(dir, &dx, &dy);
            if (map[(y + dy) * w + x + dx].polluted == 2){
                map[(y + dy) * w + x + dx].polluted = 0;
            }
        }
    }
}

static int calc_free_dirs(Place *map, int xy){
    int dir, ret = 0;
    for (dir = 0;dir < 8;dir++){
        ret += (map[xy].conn[dir]);
    }
    return ret;
}

static void pollute_place(Place *map, int w, int x, int y, int dir){
    map[y * w + x].polluted = 1;
    int ndir, cnt = 0, ldir;
    for (ndir = 0;ndir < 8;ndir++){
        if (ndir + dir != 7 && map[y * w + x].conn[ndir]){
            ldir = ndir;
            cnt++;
        }
    }
    int dx, dy;
    if (cnt == 1){
        dir_diff(ldir, &dx, &dy);
        pollute_dir(map, w, x, y, ldir);
        pollute_place(map, w, x + dx, y + dy, ldir);
    }
}

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

    int i, j, x, y, x2, y2;
    int s_x = random_below(seed, w);
    int s_y = random_below(seed, h);
    map[s_y * w + s_x].type = TYPE_SOURCE;

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

    map[s_y * w + s_x].polluted = 1;
    for (i = 0;i < 8;i++){
        if (map[s_y * w + s_x].conn[i]){
            pollute_dir(map, w, s_x, s_y, i);
            dir_diff(i, &x, &y);
            pollute_place(map, w, s_x + x, s_y + y, i);
        }
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
        tmp -> polluted = 0;
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
        (tmp -> brect).x = tmp -> x - PLACE_BSIZE / 2;
        (tmp -> brect).y = tmp -> y - PLACE_BSIZE / 2;
        (tmp -> brect).w = PLACE_BSIZE;
        (tmp -> brect).h = PLACE_BSIZE;
        if (y < h - 1){
            (tmp -> dir)[2] = f + (w * y) + w + x;
            if (x < w - 1) (tmp -> dir)[3] = f + (w * y) + w + 1 + x;
            else (tmp -> dir)[3] = NULL;
            if (x) (tmp -> dir)[1] = f + (w * y) + w - 1 + x;
            else (tmp -> dir)[1] = NULL;
        } else {
            (tmp -> dir)[1] = NULL;
            (tmp -> dir)[2] = NULL;
            (tmp -> dir)[3] = NULL;
        }
        if (x < w - 1) (tmp -> dir)[0] = f + (w * y) + 1 + x;
        else (tmp -> dir)[0] = NULL;
        for (i = 0;i < 4;i++){
            tmp -> flag[i] = 0;
        }
        for (i = 0;i < 8;i++){
            tmp -> path[i] = -1;
        }
        for (i = 0;i < 8;i++){
            tmp -> conn[i] = 0;
        }
    }
    places -> l_click = -1;
    places -> dl_sel = 0;

    generate(places, seed);

    return 0;
}

int click(Places *places, int x, int y, SDL_Renderer *render){
    SDL_Point p;
    p.x = x;
    p.y = y;
    int i, dx, dy, px, py;
    Place *map;
    map = places -> places;
    int inr = 0;
    int w = places -> w;
    int ll_click = places -> l_click;
    for (i = 0;i < w * (places -> h);i++){
        if (SDL_PointInRect(&p, &map[i].brect)){
            inr = 1;
            if (map[places -> l_click].polluted == 1 &&
                map[i].polluted == 2){
                px = (places -> l_click) % w;
                py = (places -> l_click) / w;
                dx = i % w - px;
                dy = i / w - py;
                pollute_dir(map, w, px, py, diff_dir(dx, dy));
                pollute_place(map, w, i % w, i / w, diff_dir(dx, dy));
            } else {
                places -> l_click = i;
                click_place(map, w, i % w, i / w);
            }
        }
    }
    if (!inr || i == ll_click) places -> l_click = -1;
    if (ll_click >= 0){
        declick_place(map, w, ll_click % w, ll_click / w);
    }
    return 0;
}


int blit_places(Places *places, SDL_Renderer *render){
    Place *map;
    Place *cur;
    map = places -> places;
    int w, h, i;
    w = places -> w;
    h = places -> h;

    for (i = 0;i < w * h;i++){
        SDL_SetRenderDrawColor(render,
                places_reds[map[i].type],
                places_greens[map[i].type],
                places_blues[map[i].type],
                255);
        SDL_RenderFillRect(render, &(map[i].rect));
        if (map[i].polluted){
            if (map[i].polluted == 1) SDL_SetRenderDrawColor(render, 0, 0, 0, 100);
            if (map[i].polluted == 2) SDL_SetRenderDrawColor(render, 0, 0, 255, 100);
            SDL_RenderFillRect(render, &(map[i].brect));
        }
    }

    int j;
    for (i = 0;i < w * h;i++){
        cur = map + i;
        for (j = 0;j < 4;j++){
            if ((cur -> dir)[j] && cur -> flag[j]){ 
                if (cur -> flag[j] == 1)
                     SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
                else SDL_SetRenderDrawColor(render, 0, 25, 0, 255);
                SDL_RenderDrawLine(render, cur -> x, cur -> y,
                                   (cur -> dir)[j] -> x,
                                   (cur -> dir)[j] -> y);
            }
        }
    }

    return 0;
}
