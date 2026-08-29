#ifndef SHIP_H
#define SHIP_H

#include "neslib.h"
#include "palette.h"
#include "types.h"

extern const val* const ship_list[];
extern const val* const explosion_list[];

extern const val shield_up[];
extern const val shield_down[];
extern const val shield_left[];
extern const val shield_right[];

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
routine(ship_regen_shield);
void ship_give_wide_shot(void);
void ship_give_luck(void);
void ship_give_magnet(void);
void ship_activate_protection(void);
void ship_give_rapid_fire(void);
void ship_give_rear_turret(void);
void ship_give_repulsor(void);
void ship_give_fast_turn(void);
void ship_give_brake(void);
bool __fastcall__ ship_has_wide_shot(void);
bool __fastcall__ ship_has_luck(void);
bool __fastcall__ ship_has_magnet(void);
bool __fastcall__ ship_has_protection(void);
bool __fastcall__ ship_has_rapid_fire(void);
bool __fastcall__ ship_has_rear_turret(void);
bool __fastcall__ ship_has_repulsor(void);
bool __fastcall__ ship_has_fast_turn(void);
bool __fastcall__ ship_has_brake(void);
bool __fastcall__ ship_below_full_health(void);

#endif /* SHIP_H */
