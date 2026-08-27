#include "score.h"
#include "nesdoug.h"
#include "neslib.h"
#include "string.h"

#define PLAY_FONT_BASE  0xC1
#define DIGIT_TILE(ch)  ((ch) - ' ' + PLAY_FONT_BASE)

#define SCORE_X  2
#define SCORE_Y  2

#define MAX_MULT     20

Score score;
unsigned char score_mult;

static const char suffix[] = { ' ', 'K', 'M', 'B', 'T', 'Q', 'P', 'S', 'H', 'O', 'N', 'D' };
#define MAX_TIER 11

routine(reset_score) {
    score.mantissa = 0;
    score.tier = 0;
    score_mult = 1;
}

routine(reset_score_multiplier) {
    score_mult = 1;
}

void __fastcall__ add_score(val delta) {
    score.mantissa += (unsigned)delta * score_mult;
    while (score.mantissa >= 1000 && score.tier < MAX_TIER) {
        score.mantissa -= 1000;
        ++score.tier;
    }
    if (score.tier == MAX_TIER && score.mantissa > 999)
        score.mantissa = 999;
    if (score_mult < MAX_MULT)
        ++score_mult;
}

render_routine(Score) {
    val h, t, o;
    bigval m = score.mantissa;

    h = 0; while (m >= 100) { m -= 100; ++h; }
    t = 0; while (m >= 10)  { m -= 10;  ++t; }
    o = (val)m;

    one_vram_buffer(DIGIT_TILE('0' + h), NTADR_A(SCORE_X+0, SCORE_Y));
    one_vram_buffer(DIGIT_TILE('0' + t), NTADR_A(SCORE_X+1, SCORE_Y));
    one_vram_buffer(DIGIT_TILE('0' + o), NTADR_A(SCORE_X+2, SCORE_Y));
    //one_vram_buffer(DIGIT_TILE('0' + score_mult), NTADR_A(SCORE_X+3, SCORE_Y));

    if (score.tier)
        one_vram_buffer(DIGIT_TILE(suffix[score.tier]), NTADR_A(SCORE_X+3, SCORE_Y));
    else
        one_vram_buffer(DIGIT_TILE(' '), NTADR_A(SCORE_X+3, SCORE_Y));

    return sprid;
}
