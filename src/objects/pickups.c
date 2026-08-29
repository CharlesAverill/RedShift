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
#define MAGNET_MAX_SPEED 768
#define MAGNET_MIN_SPEED 64

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
        case PowerupRapidFire:
            ship_give_rapid_fire();
            sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
            break;
        case PowerupRearTurret:
            ship_give_rear_turret();
            sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
            break;
        case PowerupRepulsor:
            ship_give_repulsor();
            sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
            break;
        case PowerupTurnSpeed:
            ship_give_fast_turn();
            sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
            break;
        case PowerupBrake:
            ship_give_brake();
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
static bool cur_is_powerup;
static bigval px, py;
static Bullet *bp;
routine(Pickups_update) {
    static bigval ship_cx, ship_cy;
    static sbigval dx, dy;
    static val abs_dx, abs_dy, dist;
    static bigval speed, mag;

    ++pickup_timer;

    r1.width  = PICKUP_W;
    r1.height = PICKUP_H;

    for(i = 0; i < n_pickups; ++i) {
        p = &pickups[i];
        cur_is_powerup = is_powerup_pickup(p->type);
        if (cur_is_powerup) {
            // Boss-reward powerups never time out; the player must pick one.
        } else if (!p->lifetime) {
            delete_pickup(i);
            --i;
            continue;
        } else if (pickup_timer & 0x1) {
            --p->lifetime;
        }

        // Cache position once; only written back if the magnet actually moves it.
        px = p->x;
        py = p->y;

        if (ship_has_magnet() && !awaiting_boss_powerup_choice) {
            ship_cx = ship_x + ((bigval)8 << 8);
            ship_cy = ship_y + ((bigval)8 << 8);
            dx = (sbigval)(ship_cx - px) >> 8;
            dy = (sbigval)(ship_cy - py) >> 8;
            abs_dx = (dx < 0) ? -dx : dx;
            abs_dy = (dy < 0) ? -dy : dy;
            dist = abs_dx + abs_dy;

            if (dist > 0 && dist < MAGNET_RANGE) {
                speed = MAGNET_MAX_SPEED - ((bigval)dist * (MAGNET_MAX_SPEED - MAGNET_MIN_SPEED)) / MAGNET_RANGE;

                mag = ((bigval)speed * abs_dx) / dist;
                px += (dx < 0) ? -mag : mag;
                mag = ((bigval)speed * abs_dy) / dist;
                py += (dy < 0) ? -mag : mag;

                p->x = px;
                p->y = py;
            }
        }

        // Set collision rect
        r1.x = px >> 8;
        r1.y = py >> 8;
        r1.width = PICKUP_W;
        r1.height = PICKUP_H;

        // Check for bullet collision
        for(bullet_index = 0; bullet_index < n_bullets; ++bullet_index) {
            bp = &bullets[bullet_index];
            r2.x = bp->x >> 8;
            r2.y = bp->y >> 8;
            r2.width = 8;
            r2.height = 8;
            if (cur_is_powerup && check_collision(&r1, &r2)) {
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
        if (!cur_is_powerup && check_collision(&r1, &r2)) {
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
        case PowerupRapidFire:
            return BULLET_HORZ_SPRITE;
        case PowerupRearTurret:
            return BULLET_VERT_SPRITE;
        case PowerupRepulsor:
            return REPULSOR_SPRITE;
        case PowerupTurnSpeed:
            return TURN_SPEED_SPRITE;
        case PowerupBrake:
            return BRAKE_SPRITE;
    }
    return SMALL_POINTS_SPRITE;
}

static val nxt;
static val pickup_attrs;
render_routine(Pickups) {
    nxt = sprid;
    for(i = 0; i < n_pickups; ++i) {
        p = &pickups[i];
        if (is_powerup_pickup(p->type)) {
            pickup_attrs = (pickup_timer >> 2) & 3;
        } else {
            pickup_attrs = PICKUPS_PALETTE | (pickup_timer % 16 < 8 ? OAM_FLIP_H : 0);
        }
        nxt = oam_spr(p->x >> 8, p->y >> 8, sprite_of_Pickup(p), pickup_attrs, nxt);
    }
    return nxt;
}

void add_pickup(PickupType type, val x, val y) {
    if (n_pickups >= MAX_PICKUPS)
        return;

    p = &pickups[n_pickups];
    p->type = type;
    p->x = (bigval)x << 8;
    p->y = (bigval)y << 8;
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
        if (choice == PowerupRapidFire && ship_has_rapid_fire())
            continue;
        if (choice == PowerupRearTurret && ship_has_rear_turret())
            continue;
        if (choice == PowerupRepulsor && ship_has_repulsor())
            continue;
        if (choice == PowerupTurnSpeed && ship_has_fast_turn())
            continue;
        if (choice == PowerupBrake && ship_has_brake())
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
