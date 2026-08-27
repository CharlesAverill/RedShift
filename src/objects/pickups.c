#include "neslib.h"
#include "objects/pickups.h"
#include "objects/ship.h"
#include "score.h"
#include "utils.h"
#include "sound.h"

#define MAX_PICKUPS 4
#define PICKUP_W    8
#define PICKUP_H    8
#define PICKUP_LIFETIME 255

static val n_pickups;
static Pickup pickups[MAX_PICKUPS];
static Pickup *p;
static Rect r1, r2;
static val pickup_timer;

routine(Pickups_init) {
    n_pickups = 0;
    pickup_timer = 0;
}

void delete_pickup(val n) {
    for(; n < n_pickups; ++n) {
        pickups[n] = pickups[n + 1];
    }
    --n_pickups;
}

static val i;
routine(Pickups_update) {
    ++pickup_timer;

    r1.width  = PICKUP_W;
    r1.height = PICKUP_H;

    for(i = 0; i < n_pickups; ++i) {
        p = &pickups[i];
        if (!p->lifetime) {
            delete_pickup(i);
            --i;
        } else if (pickup_timer & 0x1) {
            --p->lifetime;
        }

        // Set collision rect        
        r1.x = p->x;
        r1.y = p->y;

        // Check if collided with ship
        r2.x = (ship_x >> 8) + 8;
        r2.y = (ship_y >> 8) + 8;
        r2.width = 16;
        r2.height = 16;
        if (check_collision(&r1, &r2)) {
            switch(p->type) {
                case SmallPoints:
                    add_score(SMALL_POINTS);
                    sfx_play(SFX_SMALL_PICKUP, SFX_CHANNEL);
                    break;
                case LargePoints:
                    add_score(LARGE_POINTS);
                    sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
                    break;
                case Shield:
                    ship_regen_shield();
                    sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
                    break;
            }
            delete_pickup(i);
        }
    }
}

val sprite_of_Pickup(Pickup *p) {
    switch(p->type) {
        case SmallPoints:
            return SMALL_POINTS_SPRITE;
        case LargePoints:
            return pickup_timer % 16 < 8 ? LARGE_POINTS_SPRITE_1 : LARGE_POINTS_SPRITE_2;
        case Shield:
            return SHIELD_SPRITE;
    }
}

static val nxt;
render_routine(Pickups) {
    nxt = sprid;
    for(i = 0; i < n_pickups; ++i) {
        p = &pickups[i];
        nxt = oam_spr(p->x, p->y, sprite_of_Pickup(p), PICKUPS_PALETTE | (pickup_timer % 16 < 8 ? OAM_FLIP_H : 0), nxt);
    }
    return nxt;
}

void add_pickup(PickupType type, val x, val y) {
    if (n_pickups >= MAX_PICKUPS)
        return;
    
    p->type = type;
    p->x = x;
    p->y = y;
    p->lifetime = (type == Shield) ? (PICKUP_LIFETIME * 3) : PICKUP_LIFETIME;
    pickups[n_pickups] = *p;
    ++n_pickups;
}

routine(destroy_all_pickups) {
    n_pickups = 0;
} 
