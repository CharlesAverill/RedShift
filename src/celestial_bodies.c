#include "types.h"
#include "celestial_bodies.h"

routine(CBodies_init) {

}

static val i;
routine(CBodies_update) {
    ++i;
}

val sprite = ASTEROID_SPRITE;
render_routine(CBodies) {
    if (i % 30 == 0) {
        switch(sprite) {
            case ASTEROID_SPRITE:
                sprite = HALF_ASTEROID_SPRITE;
                break;
            case HALF_ASTEROID_SPRITE:
                sprite = QUARTER_ASTEROID_SPRITE;
                break;
            case QUARTER_ASTEROID_SPRITE:
                sprite = EXPLOSION_1_SPRITE;
                break;
            case EXPLOSION_1_SPRITE:
                sprite = EXPLOSION_2_SPRITE;
                break;
            case EXPLOSION_2_SPRITE:
                sprite = ASTEROID_SPRITE;
                break;
        }
        i = 0;
    }
    return oam_spr(0, 0, sprite, 1, sprid);
}
