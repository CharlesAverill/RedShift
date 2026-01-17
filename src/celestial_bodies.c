#include "types.h"
#include "celestial_bodies.h"
#include "neslib.h"
#include "math.h"
#include "bullets.h"
#include "utils.h"

void delete_body(val n);

const val CBody_sprites[CBodyTypeEnd] = {ASTEROID_SPRITE, HALF_ASTEROID_SPRITE, QUARTER_ASTEROID_SPRITE};
const val CBody_dead_sprites[2] = {EXPLOSION_1_SPRITE, EXPLOSION_2_SPRITE};
const val CBody_widths[CBodyTypeEnd] = {8, 8, 8};
const val CBody_heights[CBodyTypeEnd] = {8, 8, 8};

#define CBody_sprite(body) (body.dead ? CBody_dead_sprites[(body.dead_frame & 8) >> 3] : CBody_sprites[body.type])
#define CBody_width(type) CBody_widths[type]
#define CBody_height(type) CBody_heights[type]

#define MAX_BODIES 20

static val n_bodies = 0;
static CBody bodies[MAX_BODIES];

static val i, j;
routine(CBodies_init) {
    n_bodies = 0;
    for(i = 0; i < 10; ++i) {
        add_body(rand8() << 8, rand8() << 8, (rand8() & 0x1) << 8, (rand8() & 0x1) << 8, rand8() & 0x2);
    }
}

static sbigval dx, dy, ax, ay;
static CBody bodyi, bodyj;
static bullet b;
static Rect r1, r2;
static bool deleted;
#define GRAVITY_RANGE 128
#define GRAVITY_STRENGTH 2

routine(CBodies_update) {
    // Cheap gravity simulation
    for(i = 0; i < n_bodies; ++i) {
        bodyi = bodies[i];
        if (bodyi.dead)
            continue;

        for(j = i + 1; j < n_bodies; ++j) {
            bodyj = bodies[j];
            
            // No gravity between large or dead objects
            if ((bodyi.type == Asteroid && bodyj.type == Asteroid) || bodyj.dead)
                continue;
            
            dx = ((sbigval)bodyj.x - (sbigval)bodyi.x) >> 8;
            dy = ((sbigval)bodyj.y - (sbigval)bodyi.y) >> 8;
            
            if (dx > GRAVITY_RANGE || dx < -GRAVITY_RANGE) continue;
            if (dy > GRAVITY_RANGE || dy < -GRAVITY_RANGE) continue;
            
            if (dx > 0) {
                if (bodyi.type != Asteroid) bodyi.vx += GRAVITY_STRENGTH;
                if (bodyj.type != Asteroid) bodyj.vx -= GRAVITY_STRENGTH;
            } else if (dx < 0) {
                if (bodyi.type != Asteroid) bodyi.vx -= GRAVITY_STRENGTH;
                if (bodyj.type != Asteroid) bodyj.vx += GRAVITY_STRENGTH;
            }
            
            if (dy > 0) {
                if (bodyi.type != Asteroid) bodyi.vy += GRAVITY_STRENGTH;
                if (bodyj.type != Asteroid) bodyj.vy -= GRAVITY_STRENGTH;
            } else if (dy < 0) {
                if (bodyi.type != Asteroid) bodyi.vy -= GRAVITY_STRENGTH;
                if (bodyj.type != Asteroid) bodyj.vy += GRAVITY_STRENGTH;
            }
            
            bodies[j] = bodyj;
        }
        bodies[i] = bodyi;
    }

    // Check for bullet collision, then apply velocity
    for(i = 0; i < n_bodies; ++i) {
        bodyi = bodies[i];
        if (bodyi.dead) {
            if (bodyi.dead_frame >= 24) {
                delete_body(i);
                --i;
            } else {
                ++bodyi.dead_frame;
                bodies[i] = bodyi;
            }
            continue;
        }

        r1.x = bodyi.x >> 8;
        r1.y = bodyi.y >> 8;
        r1.width = CBody_width(bodyi.type);
        r1.height = CBody_height(bodyi.type);

        for(j = 0; j < n_bullets; ++j) {
            b = bullets[j];
            r2.x = b.x >> 8;
            r2.y = b.y >> 8;
            r2.width = 8;
            r2.height = 8;

            if (check_collision(&r1, &r2)) {
                bodyi.dead = true;
                bodyi.dead_frame = 0;
                bodyi.vx = 0;
                bodyi.vy = 0;
                break;
            }
        }

        if (!bodyi.dead) {
            bodyi.x += bodyi.vx;
            bodyi.y += bodyi.vy;
        }

        bodies[i] = bodyi;
    }
}

void delete_body(val n) {
    for(; n < n_bodies; ++n) {
        bodies[n] = bodies[n + 1];
    }
    --n_bodies;
}

static val nxt;
render_routine(CBodies) {
    nxt = sprid;
    for(i = 0; i < n_bodies; ++i) {
        nxt = oam_spr(bodies[i].x >> 8, bodies[i].y >> 8, CBody_sprite(bodies[i]), 1, nxt);
    }
    return nxt;
}

val num_bodies(void) {
    return n_bodies;
}

void add_body(bigval x, bigval y, sbigval vx, sbigval vy, CBodyType type) {
    if (n_bodies >= MAX_BODIES)
        return;

    bodyi.x = x;
    bodyi.y = y;
    bodyi.vx = vx;
    bodyi.vy = vy;
    bodyi.type = type;
    bodyi.dead = false;
    bodies[n_bodies] = bodyi;

    ++n_bodies;
}
