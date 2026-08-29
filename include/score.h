#ifndef SCORE_H
#define SCORE_H

#include "types.h"

#define SMALL_POINTS 4
#define LARGE_POINTS 8

typedef struct {
    bigval mantissa;
    val tier;
} Score;

#define MAX_TIER 12

extern Score score;
extern val score_mult;

void __fastcall__ add_score(val delta);
routine(reset_score);
routine(reset_score_multiplier);
render_routine(Score);

#endif /* SCORE_H */
