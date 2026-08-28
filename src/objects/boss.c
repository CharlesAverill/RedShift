#include "types.h"
#include "objects/boss.h"
#include "objects/celestial_bodies.h"
#include "events.h"
#include "sound.h"
#include "neslib.h"
#include "nesdoug.h"

bool start_boss_encounter = false;
bool boss_active = false;

static val music_timer;

routine(Boss_init) {
    boss_active = false;
    start_boss_encounter = false;
}

routine(trigger_boss_encounter) {
    start_boss_encounter = false;
    boss_active = true;
    destroy_all_bodies();

    enable_bgm = false;
    dim_bgm = true;
    music_timer = 128;

    disable_asteroid_spawns();
}

routine(Boss_update) {
    if (!boss_active)
        return;

    if (music_timer && --music_timer == 0) {
        enable_bgm = true;
        music_play(BGM_KNIGHTS);
    }

    // TODO: movement, attacks, collision, etc
    // On defeat, call end_boss_encounter()
}

routine(end_boss_encounter) {
    boss_active = false;

    enable_asteroid_spawns();

    dim_bgm = false;
    enable_bgm = true;
    music_play(BGM_PRELUDE);
}

render_routine(Boss) {
    if (!boss_active)
        return sprid;

    // TODO: draw boss
    return sprid;
}
