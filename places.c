#include<stdlib.h>
#include<stdio.h>

#include"places.h"

#define CLR_EDGE (25)
#define PLACE_SIZE (10)
#define PLACE_BSIZE (20)

#define TYPE_CROSS (0)
#define TYPE_SOURCE (1)
#define TYPE_PLACE (2)

#define NOT_SELECTED (0)
#define SELECTED_SRC (1)
#define SELECTED_DST (2)
#define SELECTED_POS (3)

int places_reds[]   = {0,  0,255};
int places_greens[] = {255,0,0};
int places_blues[]  = {0,  0,0};

static byte set_nth_bit(byte b, int n){
    return b | 1 << n;
}

static int get_nth_bit(byte b, int n){
    return (b >> n) & 1;
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
    if (dir >= 4) dir++;
    (*dx) = dir % 3;
    (*dy) = dir / 3;
    (*dx)--;
    (*dy)--;
}

static int connect(Place *src, Place *dst, RandomSeed *seed){
    int dx = -2, dy = -1, oldir, dir;

    while (src != dst){
        oldir = diff_dir(dx, dy);
        dy = 0;
        dx = 0;
        if (dst -> X - src -> X > 0) dx = random_below(seed, 2);
        if (dst -> X - src -> X < 0) dx = -random_below(seed, 2);
        if (dst -> Y - src -> Y > 0) dy = random_below(seed, 2);
        if (dst -> Y - src -> Y < 0) dy = -random_below(seed, 2);
        if (!dx && !dy){
            dir_diff(oldir, &dx, &dy);
            continue;
        }
        dir = diff_dir(dx, dy);
        if (dir >= 4){
            src -> flag[dir - 4] = 1;
        }
        src -> conn[dir] = 1;

        if (oldir >= 0) src -> path[oldir] = set_nth_bit(src -> path[oldir], dir);

        src = src -> dir[dir];
        src -> conn[diff_dir(-dx, -dy)] = 1;
        if (dir <= 3){
            src -> flag[3 - dir] = 1;
        }
    }
    return 0;
}

static void pollute_dir(Place *place, int dir){
    int dx, dy;
    dir_diff(dir, &dx, &dy);
    if (dir > 3){
        place -> flag[dir - 4] = 2;
    } else {
        place -> dir[dir] -> flag[3 - dir] = 2;
    }
}

static void unpollute_dir(Place *place, int dir){
    int dx, dy;
    dir_diff(dir, &dx, &dy);
    if (dir > 3){
        place -> flag[dir - 4] = 1;
    } else {
        place -> dir[dir] -> flag[3 - dir] = 1;
    }
}

static int dir_polluted(Place *place, int dir){
    int dx, dy;
    dir_diff(dir, &dx, &dy);
    if (dir > 3){
        return place -> flag[dir - 4] == 2;
    } else {
        return place -> dir[dir] -> flag[3 - dir] == 2;
    }
}

static int click_place(Place *place, char comes_from){
    int dir;
    byte chs;
    place -> selected = SELECTED_DST;
    chs = place -> path[comes_from];
    for (dir = 0;dir < 8;dir++){
        if (get_nth_bit(chs, dir)){
            place -> dir[dir] -> selected = SELECTED_POS;
        }
    }
    return (comes_from == place -> comes_from);
    /*
    if (place -> comes_from >= 0 && place -> path[place -> comes_from] >= 0)
        place -> dir[place -> path[place -> comes_from]] -> selected = SELECTED_POS;
        */
}

static void declick_place(Place *place){
    int dir;
    place -> selected = NOT_SELECTED;
    for (dir = 0;dir < 8;dir++){
        if (place -> conn[dir]){
            if (place -> dir[dir] -> selected == SELECTED_POS){
                place -> dir[dir] -> selected = NOT_SELECTED;
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

static void pollute_place(Place *place, int dir){
    place -> dir[7 - dir] -> comes_from = -1;
    place -> comes_from = dir;
    place -> polluted = 1;
    int ndir, cnt = 0, ldir;
    for (ndir = 0;ndir < 8;ndir++){
        if (ndir + dir != 7 && place -> conn[ndir]){
            ldir = ndir;
            cnt++;
        }
    }
    if (cnt == 1){
        pollute_dir(place, ldir);
        pollute_place(place -> dir[ldir], ldir);
    }
}

static void unpollute_place(Place *place, int dir){
    place -> comes_from = -1;
    int ndir, cnt = 0, ldir, cnt_pol = 0;
    for (ndir = 0;ndir < 8;ndir++){
        if (ndir + dir != 7 && place -> conn[ndir]){
            ldir = ndir;
            cnt++;
            if (dir_polluted(place, ndir)) cnt_pol++;
        }
    }
    if (cnt == 1){
        place -> polluted = 0;
        unpollute_dir(place, ldir);
        unpollute_place(place -> dir[ldir], ldir);
    }
    if (!cnt_pol) place -> polluted = 0;
}

static void pick_rand_normal(Place *map, int *xy, int size, RandomSeed *seed){
    *xy = random_below(seed, size);
    while (map[*xy].type != TYPE_CROSS){
        *xy = random_below(seed, size);
    }
}

static int generate(Places *places, RandomSeed *seed){
    int w = places -> w;
    int h = places -> h;
    Place *map;
    map = places -> places;

    int i, j, xy, xy2;
    int s = random_below(seed, w * h);
    map[s].type = TYPE_SOURCE;

    for (i = 0;i < 10;i++){
        xy = s;
        for (j = 0;j < 3;j++){
            pick_rand_normal(map, &xy2, w * h, seed);
            connect(map + xy, map + xy2, seed);
            xy = xy2;
        }
        map[xy2].type = TYPE_PLACE;
    }

    map[s].polluted = 1;
    for (i = 0;i < 8;i++){
        if (map[s].conn[i]){
            pollute_dir(map + s, i);
            pollute_place(map[s].dir[i], i);
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
    places -> ready = 0;
    places -> history = malloc(sizeof(Action) * 200);
    places -> history_size = 200;
    places -> history_used = 0;
    places -> score = 0;

    for (y = h - 1;y >= 0;y--)
    for (x = w - 1;x >= 0;x--){
        tmp = f + (w * y) + x;
        tmp -> polluted = 0;
        tmp -> selected = NOT_SELECTED;
        tmp -> comes_from = -1;
        tmp -> X = x;
        tmp -> Y = y;
        tmp -> type = TYPE_CROSS;
        tmp -> x = CLR_EDGE + PLACE_SIZE / 2 + x * Dx ;//+ randrange(seed, -(Dx / 3), Dx / 3);
        tmp -> y = CLR_EDGE + PLACE_SIZE / 2 + y * Dy ;//+ randrange(seed, -(Dy / 3), Dy / 3);
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

        (tmp -> dir)[0] = NULL;
        (tmp -> dir)[1] = NULL;
        (tmp -> dir)[2] = NULL;
        (tmp -> dir)[3] = NULL;
        if (y < h - 1){
            (tmp -> dir)[6] = f + (w * y) + w + x;
            f[(y + 1) * w + x].dir[1] = tmp;
            if (x < w - 1){
                (tmp -> dir)[7] = f + (w * y) + w + 1 + x;
                f[(y + 1) * w + x + 1].dir[0] = tmp;
            } else (tmp -> dir)[7] = NULL;
            if (x){
                (tmp -> dir)[5] = f + (w * y) + w - 1 + x;
                f[(y + 1) * w + x - 1].dir[2] = tmp;
            } else (tmp -> dir)[5] = NULL;
        } else {
            (tmp -> dir)[5] = NULL;
            (tmp -> dir)[6] = NULL;
            (tmp -> dir)[7] = NULL;
        }
        if (x < w - 1){
            (tmp -> dir)[4] = f + (w * y) + 1 + x;
            f[y * w + x + 1].dir[3] = tmp;
        } else (tmp -> dir)[4] = NULL;
        for (i = 0;i < 4;i++){
            tmp -> flag[i] = 0;
        }
        for (i = 0;i < 8;i++){
            tmp -> path[i] = 0;
        }
        for (i = 0;i < 8;i++){
            tmp -> conn[i] = 0;
        }
    }
    places -> src = NULL;
    places -> dst = NULL;

    generate(places, seed);

    return 0;
}

void undo(Places *places){
    if (!places -> history_used) return;
    char dir, comes_from;
    Place *place;
    (places -> history_used)--;
    dir = (places -> history)[places -> history_used].dir;
    comes_from = (places -> history)[places -> history_used].comes_from;
    place = (places -> history)[places -> history_used].place;
    unpollute_dir(place, 7 - dir);
    unpollute_place(place, dir);
    place -> dir[7 - dir] -> comes_from = comes_from;
}

static void append_history(Places *places, Place *place, char dir, char comes_from){
    if (places -> history_used == places -> history_size){
        (places -> history_size) *= 2;
        places -> history = realloc(places -> history, places -> history_size);
    }
    (places -> history)[places -> history_used].place = place;
    (places -> history)[places -> history_used].dir = dir;
    (places -> history)[places -> history_used].comes_from = comes_from;
    (places -> history_used)++;
}

static int places_dir(Place *src, Place *dst){
    int ret;
    for (ret = 0;ret < 8;ret++){
        if (src -> dir[ret] == dst) return ret;
    }
    return -1;
}

int click(Places *places, int x, int y, SDL_Renderer *render){
    SDL_Point p;
    p.x = x;
    p.y = y;
    int i;
    Place *map;
    map = places -> places;
    int inr = 0;
    int w = places -> w;
    int dir;
    for (i = 0;i < w * (places -> h);i++){
        if (SDL_PointInRect(&p, &map[i].brect)){
            inr = 1;
            break;
        }
    }

    Place *will_declick = NULL;
    if (inr){
        if (places -> src && places -> dst){
            will_declick = places -> src;
            if (map[i].selected == SELECTED_POS && places -> ready){
                dir = places_dir(places -> dst, map + i);
                append_history(places, map + i, dir, places -> dst -> comes_from);
                pollute_dir(places -> dst, dir);
                pollute_place(map + i, dir);
                places -> ready = 0;
            } else {
                places -> src = map + i;
                map[i].selected = SELECTED_SRC;
                declick_place(places -> dst);
                places -> dst = NULL;
            }
        } else if (places -> src){
            dir = places_dir(places -> src, map + i);
            if (dir >= 0){
                places -> dst = map + i;
                map[i].selected = SELECTED_SRC;
                places -> ready = click_place(map + i, dir);
            }
        } else {
            places -> src = map + i;
            map[i].selected = SELECTED_SRC;
        }
    } else {
        if (places -> dst) declick_place(places -> dst);
        if (places -> src) places -> src -> selected = NOT_SELECTED;
        places -> src = NULL;
        places -> dst = NULL;
    }
    if (will_declick) declick_place(will_declick);
    return 0;
}


int blit_places(Places *places, SDL_Renderer *render){
    Place *map;
    Place *cur;
    map = places -> places;
    int w, h, i;
    w = places -> w;
    h = places -> h;

    places -> score = 0;
    for (i = 0;i < w * h;i++){
        SDL_SetRenderDrawColor(render,
                places_reds[map[i].type],
                places_greens[map[i].type],
                places_blues[map[i].type],
                255);
        SDL_RenderFillRect(render, &(map[i].rect));
        if (map[i].polluted || map[i].selected){
            if (map[i].selected == SELECTED_SRC) SDL_SetRenderDrawColor(render, 150, 0, 0, 150);
            else if (map[i].selected == SELECTED_DST) SDL_SetRenderDrawColor(render, 150, 150, 0, 150);
            else if (map[i].selected == SELECTED_POS) SDL_SetRenderDrawColor(render, 0, 0, 255, 100);
            else if (map[i].polluted == 1) SDL_SetRenderDrawColor(render, 0, 0, 0, 100);
            SDL_RenderFillRect(render, &(map[i].brect));
        }
        if (map[i].type == TYPE_PLACE && map[i].polluted) (places -> score)++;
    }

    int j;
    for (i = 0;i < w * h;i++){
        cur = map + i;
        for (j = 0;j < 4;j++){
            if ((cur -> dir)[j + 4] && cur -> flag[j]){ 
                if (cur -> flag[j] == 1)
                     SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
                else SDL_SetRenderDrawColor(render, 0, 25, 0, 255);
                SDL_RenderDrawLine(render, cur -> x, cur -> y,
                                   (cur -> dir)[j + 4] -> x,
                                   (cur -> dir)[j + 4] -> y);
            }
        }
    }

    return 0;
}
