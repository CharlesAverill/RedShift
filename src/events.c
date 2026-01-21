#include "types.h"
#include "events.h"
#include "sound.h"
#include "palette.h"
#include "neslib.h"

bool game_over_flag = false;

routine(trigger_game_over) {
    music_play(BGM_PRELUDE);
    ppu_mask(DEFAULT_MASK);
    pal_spr_bright(DarkMinus);
    game_over_flag = true;
}
