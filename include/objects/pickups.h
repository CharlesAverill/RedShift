#ifndef PICKUPS_H
#define PICKUPS_H

#include "types.h"

#define _PICKUP_ROW1 0xa0
#define _PICKUP_ROW2 0xb0
#define SMALL_POINTS_SPRITE   _PICKUP_ROW1 + 6
#define LARGE_POINTS_SPRITE_1 _PICKUP_ROW2 + 2
#define LARGE_POINTS_SPRITE_2 _PICKUP_ROW2 + 4
#define SHIELD_SPRITE         _PICKUP_ROW2 + 6
#define MAGNET_SPRITE         _PICKUP_ROW2 + 7
#define PICKUPS_PALETTE 2

typedef enum PickupType {
    SmallPoints,
    LargePoints,
    Shield,
    PowerupWideShot,
    PowerupLuck,
    PowerupMagnet,
    PowerupProtection,
    PickupTypeEnd
} PickupType;

#define MIN_POWERUP PowerupWideShot
#define MAX_POWERUP PowerupProtection
#define N_POWERUPS  (MAX_POWERUP - MIN_POWERUP + 1)

typedef struct Pickup {
    PickupType type;
    val x, y;
    bigval lifetime;
} Pickup;

routine(Pickups_init);
routine(Pickups_update);
render_routine(Pickups);

routine(destroy_all_pickups);
routine(spawn_boss_reward_pickups);

void add_pickup(PickupType type, val x, val y);

#endif /* PICKUPS_H */
