#include "score.h"
#include "nesdoug.h"
#include "neslib.h"
#include "string.h"
#include "objects/boss.h"
#include "utils.h"

#define PLAY_FONT_BASE  0xC1
#define DIGIT_TILE(ch)  ((ch) - ' ' + PLAY_FONT_BASE)

#define SCORE_X  2
#define SCORE_Y  2

#if DEBUG_MODE == 1
#define ROLLOVER_SCORE 2
#else
#define ROLLOVER_SCORE 1000
#endif
#define MAX_MULT       20

Score score;
unsigned char score_mult;

static const char suffix[MAX_TIER + 1] = { ' ', 'K', 'M', 'B', 'T', 'Q', 'P', 'S', 'H', 'O', 'N', 'D', 'U'};

// Cached decimal digits of score.mantissa
static val score_h, score_t, score_o;

static void refresh_score_digits(void) {
    static bigval m;
    m = score.mantissa;
    score_h = 0; while (m >= 100) { m -= 100; ++score_h; }
    score_t = 0; while (m >= 10)  { m -= 10;  ++score_t; }
    score_o = (val)m;
}

routine(reset_score) {
    score.mantissa = 0;
    score.tier = 0;
    score_mult = 1;
    refresh_score_digits();
}

routine(reset_score_multiplier) {
    score_mult = 1;
}

void __fastcall__ add_score(val delta) {
    score.mantissa += (unsigned)delta * score_mult;
    while (score.mantissa >= ROLLOVER_SCORE&& score.tier < MAX_TIER) {
        score.mantissa -= ROLLOVER_SCORE;
        ++score.tier;
        score_mult >>= 4;
        start_boss_encounter = true;
    }
    if (score.tier == MAX_TIER && score.mantissa > 999)
        score.mantissa = 999;
    if (score_mult < MAX_MULT && delta > 1)
        ++score_mult;
    refresh_score_digits();
}

render_routine(Score) {
    one_vram_buffer(DIGIT_TILE('0' + score_h), NTADR_A(SCORE_X+0, SCORE_Y));
    one_vram_buffer(DIGIT_TILE('0' + score_t), NTADR_A(SCORE_X+1, SCORE_Y));
    one_vram_buffer(DIGIT_TILE('0' + score_o), NTADR_A(SCORE_X+2, SCORE_Y));
    //one_vram_buffer(DIGIT_TILE('0' + score_mult), NTADR_A(SCORE_X+3, SCORE_Y));

    if (score.tier)
        one_vram_buffer(DIGIT_TILE(suffix[score.tier]), NTADR_A(SCORE_X+3, SCORE_Y));
    else
        one_vram_buffer(DIGIT_TILE(' '), NTADR_A(SCORE_X+3, SCORE_Y));

    return sprid;
}
