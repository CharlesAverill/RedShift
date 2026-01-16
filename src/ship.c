#include "ship.h"
#include "controls.h"
#include "math.h"

sbigval ship_x, ship_y;
sbigval ship_vx, ship_vy;
val ship_rotation;

void Ship_init(void) {
    ship_x = 128 << 8;
    ship_y = 128 << 8;

    ship_vx = 0;
    ship_vy = 0;

    ship_rotation = 0;
}

sbigval f_x, f_y;
val thrust_counter = 0;
void Ship_update(void) {
    // Rotation
    if (down(LEFT))
        ++ship_rotation;
    else if (down(RIGHT))
        --ship_rotation;
    
    // Thrusters
    if (thrust_counter % 3 == 0) {
        if (down(UP)) {
            thrust_counter = 0;
            f_x = (sbigval)SHIP_THRUST * ((sbigval)sin(ship_rotation) - 128);
            f_y = (sbigval)SHIP_THRUST * ((sbigval)cos(ship_rotation) - 128);
            
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
    #define MAX_SPEED 1280
    if (ship_vx > MAX_SPEED) ship_vx = MAX_SPEED;
    if (ship_vx < -MAX_SPEED) ship_vx = -MAX_SPEED;
    if (ship_vy > MAX_SPEED) ship_vy = MAX_SPEED;
    if (ship_vy < -MAX_SPEED) ship_vy = -MAX_SPEED;
    
    // Apply velocity
    ship_x += ship_vx;
    ship_y += ship_vy;
}

void Ship_render(void) {
    oam_meta_spr(ship_x >> 8, ship_y >> 8, SHIP_SPRID, ship_list[((ship_rotation + 16) / 32) % 8]);
}

const val ship_palette[] = {
    BLACK, BLUE_1, ORANGE_2, LIGHT_BLUE,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

const val ship_0_data[]={

	  0,  0,0x00,0,
	  8,  0,0x01,0,
	  8,  8,0x11,0,
	  0,  8,0x10,0,
	128

};

const val ship_1_data[]={

	  0,  0,0x20,0,
	  8,  0,0x21,0,
	  8,  8,0x31,0,
	  0,  8,0x30,0,
	128

};

const val ship_2_data[]={

	  0,  0,0x02,0,
	  8,  0,0x03,0,
	  0,  8,0x12,0,
	  8,  8,0x13,0,
	128

};

const val ship_3_data[]={

	  0,  8,0x20,0|OAM_FLIP_V,
	  8,  8,0x21,0|OAM_FLIP_V,
	  8,  0,0x31,0|OAM_FLIP_V,
	  0,  0,0x30,0|OAM_FLIP_V,
	128

};

const val ship_4_data[]={

	  8,  8,0x00,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  0,0x11,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0x01,0|OAM_FLIP_H|OAM_FLIP_V,
	  8,  0,0x10,0|OAM_FLIP_H|OAM_FLIP_V,
	128

};

const val ship_5_data[]={

	  8,  8,0x20,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0x21,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  0,0x31,0|OAM_FLIP_H|OAM_FLIP_V,
	  8,  0,0x30,0|OAM_FLIP_H|OAM_FLIP_V,
	128

};

const val ship_6_data[]={

	  8,  8,0x02,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0x03,0|OAM_FLIP_H|OAM_FLIP_V,
	  8,  0,0x12,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  0,0x13,0|OAM_FLIP_H|OAM_FLIP_V,
	128

};

const val ship_7_data[]={

	  8,  0,0x20,0|OAM_FLIP_H,
	  0,  0,0x21,0|OAM_FLIP_H,
	  0,  8,0x31,0|OAM_FLIP_H,
	  8,  8,0x30,0|OAM_FLIP_H,
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
