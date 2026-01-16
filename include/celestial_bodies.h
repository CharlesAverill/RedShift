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

enum CBodyType {
    Asteroid,
    HalfAsteroid,
    QuarterAsteroid
};

typedef struct CBody {
    val x, y;
    sbigval vx, vy;
    val sprite;
} CBody;

routine(CBodies_init);
routine(CBodies_update);
render_routine(CBodies);

#endif
