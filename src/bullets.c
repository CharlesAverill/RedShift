#include "types.h"
#include "bullets.h"
#include "neslib.h"

#define MAX_BULLETS 5

static val n_bullets = 0;
static bullet bullets[MAX_BULLETS];

routine(Bullets_init) {
    n_bullets = 0;
}

static val i;
routine(Bullets_update) {
    for(i = 0; i < n_bullets; ++i) {
        bullets[i].x += bullets[i].vx;
        bullets[i].y += bullets[i].vy;
    }
}

void delete_bullet(val n) {
    for(; n < n_bullets; ++n) {
        bullets[n] = bullets[n + 1];
    }
    --n_bullets;
}

static val nxt;
render_routine(Bullets) {
    nxt = sprid;
    for(i = 0; i < n_bullets; ++i) {
        if (++bullets[i].lifetime == BULLET_LIFETIME) {
            delete_bullet(i);
            --i;
        } else {
            nxt = oam_spr(bullets[i].x >> 8, bullets[i].y >> 8, bullets[i].sprite, 2 | bullets[i].sprite_attrs, nxt);
        }
    }
}

val num_bullets(void) {
    return n_bullets;
}

static bullet new_bullet;
static val sprite, attrs;
void add_bullet(sbigval x, sbigval y, sbigval vx, sbigval vy, facing f) {
    if (n_bullets >= MAX_BULLETS)
        return;

    bullets[n_bullets].x = x;
    bullets[n_bullets].y = y;
    bullets[n_bullets].vx = vx;
    bullets[n_bullets].vy = vy;
    switch(f) {
        case Up:
            sprite = BULLET_VERT_SPRITE;
            attrs = 0;
            break;
        case UpLeft:
            sprite = BULLET_DIAG_SPRITE;
            attrs = 0;
            break;
        case Left:
            sprite = BULLET_HORZ_SPRITE;
            attrs = 0;
            break;
        case DownLeft:
            sprite = BULLET_DIAG_SPRITE;
            attrs = OAM_FLIP_V;
            break;
        case Down:
            sprite = BULLET_VERT_SPRITE;
            attrs = OAM_FLIP_V;
            break;
        case DownRight:
            sprite = BULLET_DIAG_SPRITE;
            attrs = OAM_FLIP_H | OAM_FLIP_V;
            break;
        case Right:
            sprite = BULLET_HORZ_SPRITE;
            attrs = OAM_FLIP_H;
            break;
        case UpRight:
            sprite = BULLET_DIAG_SPRITE;
            attrs = OAM_FLIP_H;
            break;
    }
    bullets[n_bullets].sprite = sprite;
    bullets[n_bullets].sprite_attrs = attrs;

    bullets[n_bullets].lifetime = 0;

    ++n_bullets;
}
