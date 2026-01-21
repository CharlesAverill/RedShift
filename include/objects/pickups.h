#ifndef PICKUPS
#define PICKUPS

#include "types.h"

#define _PICKUP_ROW1 0xa0
#define _PICKUP_ROW2 0xb0
#define SMALL_POINTS_SPRITE _PICKUP_ROW1 + 6
#define LARGE_POINTS_SPRITE_1 _PICKUP_ROW2 + 2
#define LARGE_POINTS_SPRITE_2 _PICKUP_ROW2 + 4
#define PICKUPS_PALETTE 2

typedef enum PickupType {
    SmallPoints,
    LargePoints,
    Shield,
    PickupTypeEnd
} PickupType;

typedef struct Pickup {
    PickupType type;
    val x, y;
    val lifetime;
} Pickup;

routine(Pickups_init);
routine(Pickups_update);
render_routine(Pickups);

routine(destroy_all_pickups);

void add_pickup(PickupType type, val x, val y);

#endif /* PICKUPS */
