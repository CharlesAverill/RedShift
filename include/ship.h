#ifndef SHIP_H
#define SHIP_H

#include "neslib.h"
#include "palette.h"
#include "types.h"

extern const val ship_palette[];
extern const val ship_0_data[];
extern const val ship_1_data[];
extern const val ship_2_data[];
extern const val ship_3_data[];
extern const val ship_4_data[];
extern const val ship_5_data[];
extern const val ship_6_data[];
extern const val ship_7_data[];
extern const val* const ship_list[];

#define SHIP_SPRID 0
#define SHIP_UP ship_list[0]
#define SHIP_UL ship_list[1]
#define SHIP_LEFT ship_list[2]
#define SHIP_DL ship_list[3]
#define SHIP_DOWN ship_list[4]
#define SHIP_DR ship_list[5]
#define SHIP_RIGHT ship_list[6]
#define SHIP_UR ship_list[7]

// kg
#define SHIP_MASS 64

// px/frame^2
#define SHIP_THRUST 32

#define SHIP_DRAG 8

extern sbigval ship_x, ship_y;
extern val ship_rotation;
extern sbigval ship_vx, ship_vy;

void Ship_init(void);
void Ship_update(void);
void Ship_render(void);

#endif
