#include "objects/ship.h"
#include "controls.h"
#include "math.h"
#include "objects/bullets.h"
#include "sound.h"
#include "events.h" 
#include "score.h"
#include "objects/boss.h"
#include "utils.h"

bigval ship_x, ship_y;
sbigval ship_vx, ship_vy;
val ship_rotation;
bool kill_ship_flag;
static val health;
static val iframe_ctr;
static val kill_ship_timer;
static val kill_ship_halve_timer;
static bool ship_dead;
static bool music_stopped;
static bool wide_shot;
static bool luck_charm;
static bool magnet_active;
static bool protection_ready;
static bool rapid_fire;
static bool rear_turret;
static bool repulsor_active;
static bool fast_turn;
static bool brake_active;
static val fire_cooldown;

#define SHIP_MAX_HEALTH 4
#define BASE_TURN_SPEED 2
#define FAST_TURN_SPEED 4
#define RAPID_FIRE_COOLDOWN 10  // frames between auto-fired shots
#define BRAKE_DIVISOR 16

routine(Ship_init) {
    ship_x = (bigval)128 << 8;
    ship_y = (bigval)128 << 8;

    ship_vx = 0;
    ship_vy = 0;

    ship_rotation = 0;

    kill_ship_flag = ship_dead = music_stopped = false;
    kill_ship_timer = 0;
    kill_ship_halve_timer = 0;
#if DEBUG_MODE == 1
    wide_shot        = true;
    luck_charm       = true;
    magnet_active    = true;
    protection_ready = true;
    rapid_fire       = true;
    rear_turret      = true;
    repulsor_active  = true;
    fast_turn        = true;
    brake_active     = true;
#else
    wide_shot = false;
    luck_charm = false;
    magnet_active = false;
    protection_ready = false;
    rapid_fire = false;
    rear_turret = false;
    repulsor_active = false;
    fast_turn = false;
    brake_active = false;
#endif

    health = SHIP_MAX_HEALTH;
    iframe_ctr = 1;
    fire_cooldown = 0;
}

static sbigval f_x, f_y;
static val thrust_counter = 0;
static bool rotating_retrograde = false;
static val target_rotation;
static sbigval diff1, diff2;
static bool facing_up, facing_down;
static val turn_speed;
routine(Ship_update) {
    if (kill_ship_flag) {
        ++kill_ship_timer;
        if (++kill_ship_halve_timer >= 20) {
            kill_ship_halve_timer = 0;
            ship_vx /= 2;
            ship_vy /= 2;
        }
        goto apply_velocity;
    }

    if (iframe_ctr)
        ++iframe_ctr;

    turn_speed = fast_turn ? FAST_TURN_SPEED : BASE_TURN_SPEED;

    // Rotation
    if (down(LEFT))
        ship_rotation += turn_speed;
    else if (down(RIGHT))
        ship_rotation -= turn_speed;

    if (triggered(DOWN)) {
        rotating_retrograde = true;
        if (ship_vx == 0 && ship_vy == 0)
            target_rotation = ship_rotation + 128;
        else
            target_rotation = atan2(ship_vx, ship_vy);
    } else if(!down(DOWN)) {
        rotating_retrograde = false;
        target_rotation = ship_rotation;
    } else {
        diff1 = (sbigval)ship_rotation - target_rotation;
        diff2 = (sbigval)target_rotation - ship_rotation;

        if (abs(diff1) < abs(diff2)) {
            ship_rotation += sign(diff1) * turn_speed;
        } else {
            ship_rotation += sign(diff2) * turn_speed;
        }
    }

    // Brake with B
    if (brake_active && down(B)) {
        if (ship_vx > 0) ship_vx -= max(1, ship_vx / BRAKE_DIVISOR);
        else if (ship_vx < 0) ship_vx -= min(-1, ship_vx / BRAKE_DIVISOR);
        if (ship_vy > 0) ship_vy -= max(1, ship_vy / BRAKE_DIVISOR);
        else if (ship_vy < 0) ship_vy -= min(-1, ship_vy / BRAKE_DIVISOR);
    }

    // Thrusters
    if ((thrust_counter & 3) == 0) {
        if (down(UP)) {
            thrust_counter = 0;
            f_x = SHIP_THRUST * ((sbigval)sin(ship_rotation) - 128);
            f_y = SHIP_THRUST * ((sbigval)cos(ship_rotation) - 128);
            
            ship_vx -= f_x / SHIP_MASS;
            ship_vy -= f_y / SHIP_MASS;
        } else {
            // Drag
            if (ship_vx > 0) {
                ship_vx -= max(1, ship_vx / SHIP_DRAG);
            } else if (ship_vx < 0) {
                ship_vx -= min(-1, ship_vx / SHIP_DRAG);
            }
            if (ship_vy > 0) {
                ship_vy -= max(1, ship_vy / SHIP_DRAG);
            } else if (ship_vy < 0) {
                ship_vy -= min(-1, ship_vy / SHIP_DRAG);
            }
        }
    }
    ++thrust_counter;
    
    // Speed cap
    #define MAX_SPEED 2048
    if (ship_vx > MAX_SPEED) ship_vx = MAX_SPEED;
    if (ship_vx < -MAX_SPEED) ship_vx = -MAX_SPEED;
    if (ship_vy > MAX_SPEED) ship_vy = MAX_SPEED;
    if (ship_vy < -MAX_SPEED) ship_vy = -MAX_SPEED;

    // Blasters
    if (fire_cooldown)
        --fire_cooldown;

    if ((triggered(A) || (rapid_fire && down(A) && fire_cooldown == 0)) && !boss_invincible) {
        static sbigval fire_x, fire_y, fire_vx, fire_vy, spread_x, spread_y;
        static val rear_rotation;

        facing_up = ship_rotation < 32 || ship_rotation > 220;
        facing_down = 96 < ship_rotation && ship_rotation < 160;

        fire_x = ((ship_x >> 8) + 5 + ((((sbigval)cos(ship_rotation & 31) - 128) << 3) / 128)) << 8;
        fire_y = ((ship_y >> 8) + (facing_up ? 8 : (facing_down ? 16 : 8)) + ((((sbigval)sin(ship_rotation & 31) - 128) << 3) / 128)) << 8;
        fire_vx = ship_vx + (BULLET_SPEED * ((sbigval)sin(ship_rotation) - 128) / 128 << 8);
        fire_vy = ship_vy + (BULLET_SPEED * ((sbigval)cos(ship_rotation) - 128) / 128 << 8);

        spread_x = ((sbigval)sin(ship_rotation + 32) - 128) / 8;
        spread_y = ((sbigval)cos(ship_rotation + 32) - 128) / 8;

        if (!wide_shot) {
            add_bullet(fire_x, fire_y, fire_vx, fire_vy, ((ship_rotation + 16) >> 5) & 7);
        } else {
            add_bullet(fire_x + (spread_x << 8), fire_y + (spread_y << 8), fire_vx, fire_vy,
                ((ship_rotation + 16) >> 5) & 7);
            add_bullet(fire_x - (spread_x << 8), fire_y - (spread_y << 8), fire_vx, fire_vy,
                ((ship_rotation + 16) >> 5) & 7);
        }

        if (rear_turret) {
            rear_rotation = (ship_rotation + 128) & 0xFF;
            fire_x = ((ship_x >> 8) + 5 + ((((sbigval)cos(rear_rotation & 31) - 128) << 3) / 128)) << 8;
            fire_y = ((ship_y >> 8) + 8 + ((((sbigval)sin(rear_rotation & 31) - 128) << 3) / 128)) << 8;
            fire_vx = ship_vx + (BULLET_SPEED * ((sbigval)sin(rear_rotation) - 128) / 128 << 8);
            fire_vy = ship_vy + (BULLET_SPEED * ((sbigval)cos(rear_rotation) - 128) / 128 << 8);
            add_bullet(fire_x, fire_y, fire_vx, fire_vy, ((rear_rotation + 16) >> 5) & 7);
        }

        if (rapid_fire)
            fire_cooldown = RAPID_FIRE_COOLDOWN;
    }

    // Apply velocity
apply_velocity:
    ship_x += ship_vx;
    ship_y += ship_vy;
}

static val frame_counter;
static val nxt;
render_routine(Ship) {
    nxt = sprid;

    // Flicker and show shields when invulnerable
    if (iframe_ctr) {
        switch(health) {
            case 4: // all sides
                nxt = oam_meta_spr((ship_x >> 8) + 8, (ship_y >> 8) + 28, nxt, shield_down);
                // intentional fall through
            case 3: // above, left, right
                nxt = oam_meta_spr((ship_x >> 8) + 8, (ship_y >> 8) - 4, nxt, shield_up);
                // intentional fall through
            case 2: // left, right
                nxt = oam_meta_spr((ship_x >> 8) - 4, (ship_y >> 8) + 8, nxt, shield_left);
                nxt = oam_meta_spr((ship_x >> 8) + 28, (ship_y >> 8) + 8, nxt, shield_right);
                break;
            case 1: // above
                nxt = oam_meta_spr((ship_x >> 8) + 8, (ship_y >> 8) - 4, nxt, shield_up);
                break;
        }

        if (iframe_ctr & 1)
            return nxt;
    }

    // There's some weirdness here if I try to assign to a `sprite` value that's
    // then passed to a single oam_meta_spr call
    if (kill_ship_flag && (ship_dead || kill_ship_timer > 240)) {
        if (!ship_dead && kill_ship_timer > 240) {
            ship_dead = true;
            music_stopped = true;
            kill_ship_timer = 0;
            ppu_mask(0x1e | (1 << 5)); // red emphasis
        } else if (music_stopped && kill_ship_timer > 120) {
            music_stopped = false;
            trigger_game_over();
        }

        return oam_meta_spr((ship_x >> 8) + 8, (ship_y >> 8) + 8, nxt, explosion_list[++frame_counter % 32 < 16]);
    } else
        return oam_meta_spr((ship_x >> 8) + 8, (ship_y >> 8) + 8, nxt, ship_list[((ship_rotation + 16) / 32) & 7]);
}

routine(ship_damage) {
    if (iframe_ctr || kill_ship_flag) {
        if (iframe_ctr >= 60)
            iframe_ctr = 0;

        return;
    }

    if (protection_ready) {
        protection_ready = false;
        iframe_ctr = 1;
        sfx_play(SFX_LARGE_PICKUP, SFX_CHANNEL);
        return;
    }

    if (health == 0) {
        kill_ship_flag = true;
        music_stop();
        sfx_play(SFX_GAME_OVER, SFX_CHANNEL);
    } else {
        reset_score_multiplier();
#if DEBUG_MODE != 1
        --health;
#endif
        iframe_ctr = 1;
        sfx_play(SFX_EXPLOSION, SFX_CHANNEL);
    }
}

routine(ship_regen_shield) {
    if (health < SHIP_MAX_HEALTH)
        ++health;
}

void ship_give_wide_shot(void) {
    wide_shot = true;
}

void ship_give_luck(void) {
    luck_charm = true;
}

void ship_give_magnet(void) {
    magnet_active = true;
}

void ship_activate_protection(void) {
    protection_ready = true;
}

void ship_give_rapid_fire(void) {
    rapid_fire = true;
}

void ship_give_rear_turret(void) {
    rear_turret = true;
}

void ship_give_repulsor(void) {
    repulsor_active = true;
}

void ship_give_fast_turn(void) {
    fast_turn = true;
}

void ship_give_brake(void) {
    brake_active = true;
}

bool __fastcall__ ship_has_wide_shot(void) {
    return wide_shot;
}

bool __fastcall__ ship_has_luck(void) {
    return luck_charm;
}

bool __fastcall__ ship_has_magnet(void) {
    return magnet_active;
}

bool __fastcall__ ship_has_protection(void) {
    return protection_ready;
}

bool __fastcall__ ship_has_rapid_fire(void) {
    return rapid_fire;
}

bool __fastcall__ ship_has_rear_turret(void) {
    return rear_turret;
}

bool __fastcall__ ship_has_repulsor(void) {
    return repulsor_active;
}

bool __fastcall__ ship_has_fast_turn(void) {
    return fast_turn;
}

bool __fastcall__ ship_has_brake(void) {
    return brake_active;
}

bool __fastcall__ ship_below_full_health(void) {
    return health < SHIP_MAX_HEALTH;
}

const val ship_0_data[]={

	  0,  0,0x80,0,
	  8,  0,0x81,0,
	  8,  8,0x91,0,
	  0,  8,0x90,0,
	128

};

const val ship_1_data[]={

	  0,  0,0xa0,0,
	  8,  0,0xa1,0,
	  8,  8,0xb1,0,
	  0,  8,0xb0,0,
	128

};

const val ship_2_data[]={

	  0,  0,0x82,0,
	  8,  0,0x83,0,
	  0,  8,0x92,0,
	  8,  8,0x93,0,
	128

};

const unsigned char ship_3_data[]={
	  0,  8,0xa0,0|OAM_FLIP_V,
	  8,  8,0xa1,0|OAM_FLIP_V,
	  8,  0,0xb1,0|OAM_FLIP_V,
	  0,  0,0xb0,0|OAM_FLIP_V,
	128
};

const val ship_4_data[]={

	  8,  8,0x80,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  0,0x91,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0x81,0|OAM_FLIP_H|OAM_FLIP_V,
	  8,  0,0x90,0|OAM_FLIP_H|OAM_FLIP_V,
	128

};

const val ship_5_data[]={

	  8,  8,0xa0,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0xa1,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  0,0xb1,0|OAM_FLIP_H|OAM_FLIP_V,
	  8,  0,0xb0,0|OAM_FLIP_H|OAM_FLIP_V,
	128

};

const val ship_6_data[]={

	  8,  8,0x82,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0x83,0|OAM_FLIP_H|OAM_FLIP_V,
	  8,  0,0x92,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  0,0x93,0|OAM_FLIP_H|OAM_FLIP_V,
	128

};

const val ship_7_data[]={

	  8,  0,0xa0,0|OAM_FLIP_H,
	  0,  0,0xa1,0|OAM_FLIP_H,
	  0,  8,0xb1,0|OAM_FLIP_H,
	  8,  8,0xb0,0|OAM_FLIP_H,
	128

};

const val* const ship_list[]={

	ship_0_data,
	ship_1_data,
	ship_2_data,
	ship_3_data,
	ship_4_data,
	ship_5_data,
	ship_6_data,
	ship_7_data

};

const unsigned char explosion_1[]={
	  0,  0,0xa2,2,
	  0,  8,0xa2,2|OAM_FLIP_V,
	128
};

const unsigned char explosion_2[]={
	  0,  0,0xa3,2,
	  0,  8,0xb3,2,
	128
};

const val* const explosion_list[] = {
    explosion_1,
    explosion_2
};

const unsigned char shield_up[]={
	  0,  0,0xa5,0,
	  8,  0,0xa5,0|OAM_FLIP_H,
	128
};

const unsigned char shield_down[]={
	  0,  0,0xa5,0|OAM_FLIP_V,
	  8,  0,0xa5,0|OAM_FLIP_H|OAM_FLIP_V,
	128
};

const unsigned char shield_left[]={
	  0,  0,0xb5,0|OAM_FLIP_V,
	  0,  8,0xb5,0,
	128
};

const unsigned char shield_right[]={
	  0,  0,0xb5,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0xb5,0|OAM_FLIP_H,
	128
};
