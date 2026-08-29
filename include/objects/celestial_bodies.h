#ifndef CELESTIAL_BODIES_H
#define CELESTIAL_BODIES_H

#include "neslib.h"
#include "types.h"

#define _ASTEROID_ROW1 128
#define _ASTEROID_ROW2 144
#define _ASTEROID_ROW3 160
#define ASTEROID_SPRITE _ASTEROID_ROW1 + 4
#define HALF_ASTEROID_SPRITE _ASTEROID_ROW1 + 5
#define QUARTER_ASTEROID_SPRITE _ASTEROID_ROW2 + 4
#define EXPLOSION_1_SPRITE _ASTEROID_ROW2 + 5
#define EXPLOSION_2_SPRITE _ASTEROID_ROW3 + 4

typedef enum CBodyType {
    Asteroid,
    HalfAsteroid,
    QuarterAsteroid,
    CBodyTypeEnd
} CBodyType;

typedef struct CBody {
    bigval x, y;
    sbigval vx, vy;
    CBodyType type;
    bool dead;
    val dead_frame;
    bool hasGravity;
    bool drop_pickup;
    val attrs;
    bool despawn_at_edge;
} CBody;

routine(CBodies_init);
routine(CBodies_update);
render_routine(CBodies);

void add_body(bigval x, bigval y, sbigval vx, sbigval vy, CBodyType type, bool hasGravity, val attrs, bool despawn_at_edge);
routine(destroy_all_bodies);

val random_attrs(void);

routine(enable_asteroid_spawns);
routine(disable_asteroid_spawns);

#endif
