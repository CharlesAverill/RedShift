#ifndef BULLETS_H
#define BULLETS_H

#include "types.h"

#define BULLET_VERT_SPRITE 0x86
#define BULLET_HORZ_SPRITE 0x87
#define BULLET_DIAG_SPRITE 0x96

#define BULLET_LIFETIME 24
#define BULLET_SPEED (sbigval)-6

#define BULLET_PALETTE 0x0f, 0x15, 0x27, 0x30

typedef struct bullet {
    bigval x, y;
    sbigval vx, vy;
    val sprite;
    val sprite_attrs;
    val lifetime;
} bullet;

#define MAX_BULLETS 2
extern bullet bullets[MAX_BULLETS];
extern val n_bullets;

routine(Bullets_init);
routine(Bullets_update);
render_routine(Bullets);

/**
 * @brief Add a bullet to the screen
 * 
 * @param x Bullet's x position
 * @param y Bullet's y position
 * @param vx Bullet's x velocity
 * @param vy Bullet's y velocity
 * @param f Direction bullet is facing
 */
void add_bullet(sbigval x, sbigval y, sbigval vx, sbigval vy, facing f);

#endif
