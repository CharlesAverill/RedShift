#ifndef BOSS
#define BOSS

#include "types.h"

extern bool boss_active;
extern bool boss_invincible;
extern const unsigned char boss_metasprite[];

#define BOSS1_PALETTE 0x0f, 0x2a, 0x3d, 0x30
#define BOSS_PALETTE_NO 3

routine(Boss_init);
routine(Boss_update);
render_routine(Boss);

routine(trigger_boss_encounter);
routine(end_boss_encounter);

#endif /* BOSS */
