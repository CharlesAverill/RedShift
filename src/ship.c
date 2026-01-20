#include "ship.h"
#include "controls.h"
#include "math.h"
#include "bullets.h"

bigval ship_x, ship_y;
sbigval ship_vx, ship_vy;
val ship_rotation;
bool kill_ship_flag;
static val kill_ship_timer;
static bool ship_dead;
static bool music_stopped;

routine(Ship_init) {
    ship_x = (bigval)128 << 8;
    ship_y = (bigval)128 << 8;

    ship_vx = 0;
    ship_vy = 0;

    ship_rotation = 0;

    kill_ship_flag = ship_dead = music_stopped = false;
    kill_ship_timer = 0;
}

sbigval f_x, f_y;
val thrust_counter = 0;
bool rotating_retrograde = false;
val target_rotation;
sbigval diff1, diff2;
sbigval dx, dy;
bool facing_up, facing_down;
routine(Ship_update) {
    if (kill_ship_flag) {
        if (++kill_ship_timer % 20 == 0) {
            ship_vx /= 2;
            ship_vy /= 2;
        }
        goto apply_velocity;
    }

    // Rotation
    if (down(LEFT))
        ship_rotation += 2;
    else if (down(RIGHT))
        ship_rotation -= 2;

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
            ship_rotation += sign(diff1) * 2;
        } else {
            ship_rotation += sign(diff2) * 2;
        }
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
    #define SHIP_RADIUS (sbigval)8
    if (triggered(A)) {
        facing_up = ship_rotation < 32 || ship_rotation > 220;
        facing_down = 96 < ship_rotation && ship_rotation < 160;
        add_bullet(
            ((ship_x >> 8) + 5 + (SHIP_RADIUS * ((sbigval)cos(ship_rotation & 31) - 128) / 128)) << 8,
            ((ship_y >> 8) + (facing_up ? 8 : (facing_down ? 16 : 8)) + (SHIP_RADIUS * ((sbigval)sin(ship_rotation & 31) - 128) / 128)) << 8,
            ship_vx + (BULLET_SPEED * ((sbigval)sin(ship_rotation) - 128) / 128 << 8),
            ship_vy + (BULLET_SPEED * ((sbigval)cos(ship_rotation) - 128) / 128 << 8),
            ((ship_rotation + 16) / 32) & 7
        );
    }
    
    // Apply velocity
apply_velocity:
    ship_x += ship_vx;
    ship_y += ship_vy;
}

static val sprite;
static val frame_counter;
render_routine(Ship) {
    // sprite = kill_ship_flag ? explosion_list[++frame_counter % 2] : ship_list[((ship_rotation + 16) / 32) & 7];
    if (kill_ship_flag && (ship_dead || kill_ship_timer > 240)) {
        if (!ship_dead && kill_ship_timer > 240) {
            ship_dead = true;
            music_stopped = true;
            kill_ship_timer = 0;
            ppu_mask(0x1e | (1 << 5)); // red emphasis
        } else if (music_stopped && kill_ship_timer > 120) {
            music_stopped = false;
            music_play(1);
        }

        return oam_meta_spr((ship_x >> 8) + 8, (ship_y >> 8) + 8, sprid, explosion_list[++frame_counter % 32 < 16]);
    } else
        return oam_meta_spr((ship_x >> 8) + 8, (ship_y >> 8) + 8, sprid, ship_list[((ship_rotation + 16) / 32) & 7]);
    // return oam_meta_spr((ship_x >> 8) + 8, (ship_y >> 8) + 8, sprid, ship_list[((ship_rotation + 16) / 32) & 7]);
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
	  0,  0,0xa2,3,
	  0,  8,0xa2,3|OAM_FLIP_V,
	128
};

const unsigned char explosion_2[]={
	  0,  0,0xa3,3,
	  0,  8,0xb3,3,
	128
};

const val* const explosion_list[] = {
    explosion_1,
    explosion_2
};
