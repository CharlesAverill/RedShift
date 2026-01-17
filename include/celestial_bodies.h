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

static const val CBody_sprites[CBodyTypeEnd] = {ASTEROID_SPRITE, HALF_ASTEROID_SPRITE, QUARTER_ASTEROID_SPRITE};
static const val CBody_dead_sprites[2] = {EXPLOSION_1_SPRITE, EXPLOSION_2_SPRITE};
static const val CBody_widths[CBodyTypeEnd] = {8, 8, 8};
static const val CBody_heights[CBodyTypeEnd] = {8, 8, 8};

#define CBody_sprite(body) (body.dead ? CBody_dead_sprites[(body.dead_frame & 8) >> 3] : CBody_sprites[body.type])
#define CBody_width(type) CBody_widths[type]
#define CBody_height(type) CBody_heights[type]

typedef struct CBody {
    bigval x, y;
    sbigval vx, vy;
    CBodyType type;
    bool dead;
    val dead_frame;
} CBody;

routine(CBodies_init);
routine(CBodies_update);
render_routine(CBodies);

void add_body(bigval x, bigval y, sbigval vx, sbigval vy, CBodyType type);

#endif
