#ifndef SHIP
#define SHIP

#include "neslib.h"
#include "palette.h"
#include "types.h"

extern const val* const ship_list[];
extern const val* const explosion_list[];
const val* const shield_list[];

#define SHIP_SPRID 0
#define SHIP_UP ship_list[0]
#define SHIP_UL ship_list[1]
#define SHIP_LEFT ship_list[2]
#define SHIP_DL ship_list[3]
#define SHIP_DOWN ship_list[4]
#define SHIP_DR ship_list[5]
#define SHIP_RIGHT ship_list[6]
#define SHIP_UR ship_list[7]

#define SHIP_PALETTE BLACK, BLUE_1, ORANGE_2, LIGHT_BLUE
#define EXPLOSION_PALETTE BLACK, ORANGE, LIGHT_BLUE, LIGHT_YELLOW

// kg
#define SHIP_MASS 64

// px/frame^2
#define SHIP_THRUST (sbigval)32

// inverse of drag
// v := v / drag
#define SHIP_DRAG 32

extern bigval ship_x, ship_y;
extern val ship_rotation;
extern sbigval ship_vx, ship_vy;
extern bool kill_ship_flag;

routine(Ship_init);
routine(Ship_update);
render_routine(Ship);
routine(ship_damage);

#endif /* SHIP */
