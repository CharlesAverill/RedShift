#include "types.h"
#include "celestial_bodies.h"
#include "neslib.h"
#include "math.h"
#include "bullets.h"
#include "utils.h"

void delete_body(val n);

#define MAX_BODIES 20

static val n_bodies = 0;
static CBody bodies[MAX_BODIES];

static val i, j;
routine(CBodies_init) {
    n_bodies = 0;
    for(i = 0; i < 5; ++i) {
        add_body(rand8() << 8, rand8() << 8, (rand8() & 0x1) << 8, (rand8() & 0x1) << 8, rand8() & 0x2);
    }
}

static sbigval dx, dy, ax, ay;
static CBody bodyi, bodyj;
static bullet b;
static Rect r1, r2;
static bool deleted;
#define GRAVITY_RANGE 64
#define GRAVITY_STRENGTH 2

routine(CBodies_update) {
    // Cheap gravity simulation
    for(i = 0; i < n_bodies; ++i) {
        bodyi = bodies[i];
        for(j = i + 1; j < n_bodies; ++j) {
            bodyj = bodies[j];
            dx = ((sbigval)bodyj.x - (sbigval)bodyi.x) >> 8;
            dy = ((sbigval)bodyj.y - (sbigval)bodyi.y) >> 8;
            
            if (dx > GRAVITY_RANGE || dx < -GRAVITY_RANGE) continue;
            if (dy > GRAVITY_RANGE || dy < -GRAVITY_RANGE) continue;
            
            if (dx > 0) {
                bodyi.vx += GRAVITY_STRENGTH;
                bodyj.vx -= GRAVITY_STRENGTH;
            } else if (dx < 0) {
                bodyi.vx -= GRAVITY_STRENGTH;
                bodyj.vx += GRAVITY_STRENGTH;
            }
            
            if (dy > 0) {
                bodyi.vy += GRAVITY_STRENGTH;
                bodyj.vy -= GRAVITY_STRENGTH;
            } else if (dy < 0) {
                bodyi.vy -= GRAVITY_STRENGTH;
                bodyj.vy += GRAVITY_STRENGTH;
            }
        }
    }

    // Check for bullet collision, then apply velocity
    for(i = 0; i < n_bodies; ++i) {
        bodyi = bodies[i];
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

            deleted = false;
            if (check_collision(&r1, &r2)) {
                delete_body(i);
                --i;
                deleted = true;
                break;
            }
        }

        if (!deleted) {
            bodyi.x += bodyi.vx;
            bodyi.y += bodyi.vy;
            bodies[i] = bodyi;
        }
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
        nxt = oam_spr(bodies[i].x >> 8, bodies[i].y >> 8, CBody_sprite(bodies[i].type), 1, nxt);
    }
    return nxt;
}

val num_bodies(void) {
    return n_bodies;
}

void add_body(bigval x, bigval y, sbigval vx, sbigval vy, CBodyType type) {
    if (n_bodies >= MAX_BODIES)
        return;

    bodies[n_bodies].x = x;
    bodies[n_bodies].y = y;
    bodies[n_bodies].vx = vx;
    bodies[n_bodies].vy = vy;
    bodies[n_bodies].type = type;

    ++n_bodies;
}
