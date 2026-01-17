#ifndef UTILS_H
#define UTILS_H

#include "types.h"

typedef struct Rect {
    val x, y, width, height;
} Rect;

bool check_collision(Rect *object1, Rect* object2);

#endif
