#include "types.h"
#include "bullets.h"
#include "neslib.h"
#include "sound.h"

val n_bullets = 0;
bullet bullets[MAX_BULLETS];

routine(Bullets_init) {
    n_bullets = 0;
}

void delete_bullet(val n) {
    for(; n < n_bullets; ++n) {
        bullets[n] = bullets[n + 1];
    }
    --n_bullets;
}

static val i;
static bullet b;
routine(Bullets_update) {
    for(i = 0; i < n_bullets; ++i) {
        b = bullets[i];
        b.x += b.vx;
        b.y += b.vy;
        if (b.lifetime == BULLET_LIFETIME) {
            delete_bullet(i);
            --i;
        } else {
            ++b.lifetime;
            bullets[i] = b;
        }
    }
}

static val nxt;
render_routine(Bullets) {
    nxt = sprid;
    for(i = 0; i < n_bullets; ++i) {
        b = bullets[i];
        nxt = oam_spr(b.x >> 8, b.y >> 8, b.sprite, 2 | b.sprite_attrs, nxt);
    }
    return nxt;
}

val num_bullets(void) {
    return n_bullets;
}

static val sprite, attrs;
void add_bullet(sbigval x, sbigval y, sbigval vx, sbigval vy, facing f) {
    if (n_bullets >= MAX_BULLETS)
        return;

    b.x = x;
    b.y = y;
    b.vx = vx;
    b.vy = vy;
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
    b.sprite = sprite;
    b.sprite_attrs = attrs;

    b.lifetime = 0;

    bullets[n_bullets] = b;

    ++n_bullets;

    sfx_play(SFX_BLASTER, SFX_CHANNEL);
}
