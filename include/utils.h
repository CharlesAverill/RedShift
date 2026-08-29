#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include "nesdoug.h"

typedef struct Rect {
    val x, y, width, height;
} Rect;

// Set to 1 for
// 1. Boss triggers at score.mantissa == 2
// 2. All powerups
// 3. Title screen message
#define DEBUG_MODE 0

#endif
