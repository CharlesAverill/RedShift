#include "neslib.h"
#include "objects/pickups.h"
#include "objects/ship.h"
#include "objects/bullets.h"
#include "objects/celestial_bodies.h"
#include "score.h"
#include "utils.h"
#include "sound.h"

#define MAX_PICKUPS 4
#define PICKUP_W    8
#define PICKUP_H    8
#define PICKUP_LIFETIME 255
#define MAGNET_RANGE 64

static val n_pickups;
static Pickup pickups[MAX_PICKUPS];
static Pickup *p;
static Rect r1, r2;
static val pickup_timer;
static val bullet_index;
static bool awaiting_boss_powerup_choice;

static bool is_powerup_pickup(PickupType type) {
    return MIN_POWERUP <= type && type <= MAX_POWERUP;
}

static void resolve_pickup(PickupType type) {
    switch(type) {
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
        case PowerupWideShot:
            ship_give_wide_shot();
            sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
            break;
        case PowerupLuck:
            ship_give_luck();
            sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
            break;
        case PowerupMagnet:
            ship_give_magnet();
            sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
            break;
        case PowerupProtection:
            ship_activate_protection();
            sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
            break;
    }
}

routine(Pickups_init) {
    n_pickups = 0;
    pickup_timer = 0;
    awaiting_boss_powerup_choice = false;
}

static void delete_pickup(val n) {
    pickups[n] = pickups[--n_pickups];
}

static val i;
#define pull 2
routine(Pickups_update) {
    static sbigval ship_px, ship_py, dx, dy, dist;

    ++pickup_timer;

    r1.width  = PICKUP_W;
    r1.height = PICKUP_H;

    for(i = 0; i < n_pickups; ++i) {
        p = &pickups[i];
        if (is_powerup_pickup(p->type)) {
            // Boss-reward powerups never time out; the player must pick one.
        } else if (!p->lifetime) {
            delete_pickup(i);
            --i;
            continue;
        } else if (pickup_timer & 0x1) {
            --p->lifetime;
        }

        if (ship_has_magnet() && !awaiting_boss_powerup_choice) {
            ship_px = ((ship_x >> 8) + 8);
            ship_py = ((ship_y >> 8) + 8);
            dx = ship_px - p->x;
            dy = ship_py - p->y;
            dist = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);

            if (dist > 0 && dist < MAGNET_RANGE) {
                p->x += (dx * pull) / (dist + 8);
                p->y += (dy * pull) / (dist + 8);
            }
        }

        // Set collision rect
        r1.x = p->x;
        r1.y = p->y;
        r1.width = PICKUP_W;
        r1.height = PICKUP_H;

        // Check for bullet collision
        for(bullet_index = 0; bullet_index < n_bullets; ++bullet_index) {
            r2.x = bullets[bullet_index].x >> 8;
            r2.y = bullets[bullet_index].y >> 8;
            r2.width = 8;
            r2.height = 8;
            if (is_powerup_pickup(p->type) && check_collision(&r1, &r2)) {
                resolve_pickup(p->type);
                delete_pickup(i);
                --i;
                if (awaiting_boss_powerup_choice) {
                    awaiting_boss_powerup_choice = false;
                    for(bullet_index = 0; bullet_index < n_pickups; ++bullet_index) {
                        if (is_powerup_pickup(pickups[bullet_index].type)) {
                            delete_pickup(bullet_index);
                            --bullet_index;
                        }
                    }
                    enable_asteroid_spawns();
                }
                goto next_pickup;
            }
        }

        // Check if collided with ship
        r2.x = (ship_x >> 8) + 8;
        r2.y = (ship_y >> 8) + 8;
        r2.width = 16;
        r2.height = 16;
        if (!is_powerup_pickup(p->type) && check_collision(&r1, &r2)) {
            resolve_pickup(p->type);
            delete_pickup(i);
            --i;
        }

next_pickup:
        ;
    }
}

val sprite_of_Pickup(Pickup *p) {
    switch(p->type) {
        case SmallPoints:
            return SMALL_POINTS_SPRITE;
        case LargePoints:
        case PowerupLuck:
            return pickup_timer % 16 < 8 ? LARGE_POINTS_SPRITE_1 : LARGE_POINTS_SPRITE_2;
        case Shield:
        case PowerupProtection:
            return SHIELD_SPRITE;
        case PowerupWideShot:
            return BULLET_DIAG_SPRITE;
        case PowerupMagnet:
            return MAGNET_SPRITE;
    }
    return SMALL_POINTS_SPRITE;
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

    p = &pickups[n_pickups];
    p->type = type;
    p->x = x;
    p->y = y;
    p->lifetime = (type == Shield) ? (PICKUP_LIFETIME * 3) : PICKUP_LIFETIME;
    ++n_pickups;
}

void spawn_boss_reward_pickups(void) {
    static val available[N_POWERUPS];
    static val available_count;
    static val choice;
    static val pos_a, pos_b;

    n_pickups = 0;
    available_count = 0;

    awaiting_boss_powerup_choice = true;
    disable_asteroid_spawns();

    for(choice = MIN_POWERUP; choice <= MAX_POWERUP; ++choice) {
        if (choice == PowerupWideShot && ship_has_wide_shot())
            continue;
        if (choice == PowerupLuck && ship_has_luck())
            continue;
        if (choice == PowerupMagnet && ship_has_magnet())
            continue;
        available[available_count++] = choice;
    }

    if (available_count == 0) {
        awaiting_boss_powerup_choice = false;
        enable_asteroid_spawns();
        return;
    } else if (available_count == 1) {
        add_pickup(available[0], 128, 104);
        return;
    }

    pos_a = rand8() % available_count;
    do {
        pos_b = rand8() % available_count;
    } while (pos_b == pos_a);

    add_pickup(available[pos_a], 88, 104);
    add_pickup(available[pos_b], 168, 104);
}

routine(destroy_all_pickups) {
    n_pickups = 0;
}
