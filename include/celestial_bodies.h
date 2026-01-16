#ifndef CELESTIAL_BODIES_H
#define CELESTIAL_BODIES_H

#include "neslib.h"
#include "types.h"

#define ASTEROID_SPRITE 4
#define HALF_ASTEROID_SPRITE 5
#define QUARTER_ASTEROID_SPRITE 20
#define EXPLOSION_1_SPRITE 21
#define EXPLOSION_2_SPRITE 36

routine(CBodies_init);
routine(CBodies_update);
render_routine(CBodies);

#endif
