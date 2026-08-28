#ifndef BOSS
#define BOSS

#include "types.h"

extern bool boss_active;

routine(Boss_init);
routine(Boss_update);
render_routine(Boss);

routine(trigger_boss_encounter);
routine(end_boss_encounter);

#endif /* BOSS */
