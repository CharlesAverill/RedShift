#include "neslib.h"
#include "objects/pickups.h"

#define MAX_PICKUPS 4

static val n_pickups;
static Pickup pickups[MAX_PICKUPS];
static Pickup *p;
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

    for(i = 0; i < n_pickups; ++i) {
        p = &pickups[i];
        if (!p->lifetime) {
            delete_pickup(i);
            --i;
        } else if (pickup_timer & 0x1) {
            --p->lifetime;
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
            return '!';
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
    p->lifetime = 255;
    pickups[n_pickups] = *p;
    ++n_pickups;
}
