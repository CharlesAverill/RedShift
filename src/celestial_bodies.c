#include "types.h"
#include "celestial_bodies.h"
#include "neslib.h"
#include "math.h"
#include "bullets.h"
#include "utils.h"
#include "sound.h"
#include "ship.h"

void delete_body(val n);

const val CBody_sprites[CBodyTypeEnd] = {ASTEROID_SPRITE, HALF_ASTEROID_SPRITE, QUARTER_ASTEROID_SPRITE};
const val CBody_dead_sprites[2] = {EXPLOSION_1_SPRITE, EXPLOSION_2_SPRITE};
const val CBody_widths[CBodyTypeEnd] = {8, 8, 8};
const val CBody_heights[CBodyTypeEnd] = {8, 8, 8};

#define CBody_sprite(body) (body.dead ? CBody_dead_sprites[(body.dead_frame & 8) >> 3] : CBody_sprites[body.type])
#define CBody_width(type) CBody_widths[type]
#define CBody_height(type) CBody_heights[type]

#define MAX_BODIES 9
#define MIN_ASTEROIDS 3  // Try to keep at least this many
#define MAX_ASTEROIDS 7  // Don't exceed this many

static val n_bodies = 0;
static CBody bodies[MAX_BODIES];
static val i, j;
static val spawn_timer;

static val out;
val random_attrs(void) {
    out = 0;
    out |= rand8() & OAM_FLIP_H;
    out |= rand8() & OAM_FLIP_V;
    return out;
}

routine(CBodies_init) {
    n_bodies = 0;
    spawn_timer = 0;
    // for(i = 0; i < MAX_BODIES / 2; ++i) {
    //     add_body(rand8() << 8, rand8() << 8, (rand8() & 0x1) << 8, (rand8() & 0x1) << 8, rand8() % 0x2, rand8() < 64);
    // }
}

static sbigval dx, dy;
static CBody *bodyi_ptr, *bodyj_ptr;
static Bullet *b_ptr;
static val typei, typej;
static Rect r1, r2;
static sbigval bodyi_x, bodyi_y, bodyj_x, bodyj_y;
static sbigval bodyi_vx, bodyi_vy, bodyj_vx, bodyj_vy;

#define GRAVITY_RANGE 128
#define GRAVITY_STRENGTH 4

static val gravity_timer;
static val n_bodies;
static val spawn_chance;

static val bullet_x[MAX_BULLETS];
static val bullet_y[MAX_BULLETS];

routine(CBodies_update) {
    if (++gravity_timer != 3)
        goto collision_check;
    gravity_timer = 0;
    
    // Cheap gravity simulation
    for(i = 0; i < n_bodies; ++i) {
        bodyi_ptr = &bodies[i];
        if (bodyi_ptr->dead || !bodyi_ptr->hasGravity)
            continue;
        typei = bodyi_ptr->type;
        bodyi_x = bodyi_ptr->x;
        bodyi_y = bodyi_ptr->y;
        bodyi_vx = bodyi_ptr->vx;
        bodyi_vy = bodyi_ptr->vy;
        for(j = i + 1; j < n_bodies; ++j) {
            bodyj_ptr = &bodies[j];
            if (bodyj_ptr->dead || !bodyj_ptr->hasGravity)
                continue;
            // No gravity between asteroids
            typej = bodyj_ptr->type;
            if (typei == Asteroid && typej == Asteroid)
                continue;
            bodyj_x = bodyj_ptr->x;
            bodyj_y = bodyj_ptr->y;
            dx = (bodyj_x - bodyi_x) >> 8;
            dy = (bodyj_y - bodyi_y) >> 8;
            if (dx > GRAVITY_RANGE || dx < -GRAVITY_RANGE ||
                dy > GRAVITY_RANGE || dy < -GRAVITY_RANGE)
                continue;
            // Cache bodyj velocities
            bodyj_vx = bodyj_ptr->vx;
            bodyj_vy = bodyj_ptr->vy;
            // Apply gravity
            if (dx > 0) {
                bodyi_vx += GRAVITY_STRENGTH;
                bodyj_vx -= GRAVITY_STRENGTH;
            } else if (dx < 0) {
                bodyi_vx -= GRAVITY_STRENGTH;
                bodyj_vx += GRAVITY_STRENGTH;
            }
            if (dy > 0) {
                bodyi_vy += GRAVITY_STRENGTH;
                bodyj_vy -= GRAVITY_STRENGTH;
            } else if (dy < 0) {
                bodyi_vy -= GRAVITY_STRENGTH;
                bodyj_vy += GRAVITY_STRENGTH;
            }
            // Write back bodyj velocities
            bodyj_ptr->vx = bodyj_vx;
            bodyj_ptr->vy = bodyj_vy;
        }
        // Write back bodyi velocities
        bodyi_ptr->vx = bodyi_vx;
        bodyi_ptr->vy = bodyi_vy;
    }

collision_check:    
    // Spawn new asteroids based on current count
    // Fewer asteroids = higher spawn chance
    if (++spawn_timer > 100 && n_bodies < MAX_BODIES && n_bodies < MAX_ASTEROIDS) {
        // Calculate spawn chance: more aggressive when fewer asteroids
        // spawn_chance ranges from 255 (when few asteroids) to ~32 (when many)
        if (n_bodies < MIN_ASTEROIDS) {
            spawn_chance = 255;  // Very high chance
        } else {
            // Decreasing chance as we approach MAX_ASTEROIDS
            // spawn_chance = 255 - (n_bodies - MIN_ASTEROIDS) * 56
            spawn_chance = 255 - ((n_bodies - MIN_ASTEROIDS) * 56);
        }
        
        if (rand8() < spawn_chance) {
            // Spawn from a random edge
            val spawn_edge = rand8() & 0x3;  // 0-3 for four edges
            sbigval spawn_x, spawn_y, spawn_vx, spawn_vy;
            
            if (spawn_edge == 0) {  // Top edge
                spawn_x = rand8() << 8;
                spawn_y = 0;
                spawn_vx = (rand8() >> 1) * (rand8() % 0x1 ? 1 : -1);
                spawn_vy = 0x80;  // Move down
            } else if (spawn_edge == 1) {  // Right edge
                spawn_x = 0xFF00;
                spawn_y = rand8() << 8;
                spawn_vx = -0x80;  // Move left
                spawn_vy = (rand8() >> 1) * (rand8() % 0x1 ? 1 : -1);
            } else if (spawn_edge == 2) {  // Bottom edge
                spawn_x = rand8() << 8;
                spawn_y = 0xF000;
                spawn_vx = (rand8() >> 1) * (rand8() % 0x1 ? 1 : -1);
                spawn_vy = -0x80;  // Move up
            } else {  // Left edge
                spawn_x = 0;
                spawn_y = rand8() << 8;
                spawn_vx = 0x80;  // Move right
                spawn_vy = (rand8() >> 1) * (rand8() % 0x1 ? 1 : -1);
            }
            
            add_body(spawn_x, spawn_y, spawn_vx, spawn_vy, rand8() % CBodyTypeEnd, rand8() < 85, random_attrs());
            spawn_timer = 0;
        }
    }

    // Cache bullet positions once
    for(j = 0; j < n_bullets; ++j) {
        bullet_x[j] = bullets[j].x >> 8;
        bullet_y[j] = bullets[j].y >> 8;
    }

    // Check for bullet collision, and apply velocity
    for(i = 0; i < n_bodies; ++i) {
        bodyi_ptr = &bodies[i];
        if (bodyi_ptr->dead) {
            if (bodyi_ptr->dead_frame >= 24) {
                delete_body(i);
                --i;
            } else {
                ++bodyi_ptr->dead_frame;
            }
            continue;
        }
        
        if (!kill_ship_flag) {
            r1.x = bodyi_ptr->x >> 8;
            r1.y = bodyi_ptr->y >> 8;
            r1.width = CBody_width(bodyi_ptr->type);
            r1.height = CBody_height(bodyi_ptr->type);
            
            for(j = 0; j < n_bullets; ++j) {
                // // Fast early rejection
                dx = bullet_x[j] - r1.x;
                if (dx > r1.width || dx < -8)
                    continue;
                    
                dy = bullet_y[j] - r1.y;
                if (dy > r1.height || dy < -8)
                    continue;
                
                // Passed quick check, do full collision
                r2.x = bullet_x[j];
                r2.y = bullet_y[j];
                r2.width = r2.height = 8;
                if (check_collision(&r1, &r2)) {
                    bodyi_ptr->dead = true;
                    bodyi_ptr->dead_frame = 0;
                    bodyi_ptr->vx = 0;
                    bodyi_ptr->vy = 0;
                    if (!kill_ship_flag)
                        sfx_play(SFX_EXPLOSION, SFX_CHANNEL);
                    break;
                }
            }
            
            if (bodyi_ptr->dead)
                continue;

            // Check if collided with ship
            r2.x = (ship_x >> 8) + 8;
            r2.y = (ship_y >> 8) + 8;
            r2.width = 16;
            r2.height = 16;
            if (check_collision(&r1, &r2)) {
                kill_ship_flag = true;
                music_stop();
                sfx_play(SFX_GAME_OVER, SFX_CHANNEL);

                // Destroy asteroid too
                bodyi_ptr->dead = true;
                bodyi_ptr->dead_frame = 0;
                bodyi_ptr->vx = 0;
                bodyi_ptr->vy = 0;
            }
        }
        
        // Apply velocity to position
        bodyi_ptr->x += bodyi_ptr->vx;
        bodyi_ptr->y += bodyi_ptr->vy;
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
        nxt = oam_spr(bodies[i].x >> 8, bodies[i].y >> 8, CBody_sprite(bodies[i]), 1 | bodies[i].attrs, nxt);
    }
    return nxt;
}

val num_bodies(void) {
    return n_bodies;
}

static CBody body;
void add_body(bigval x, bigval y, sbigval vx, sbigval vy, CBodyType type, bool hasGravity, val attrs) {
    if (n_bodies >= MAX_BODIES)
        return;

    body.x = x;
    body.y = y;
    body.vx = vx;
    body.vy = vy;
    body.type = type;
    body.dead = false;
    body.hasGravity = hasGravity;
    body.attrs = attrs;
    bodies[n_bodies] = body;

    ++n_bodies;
}
