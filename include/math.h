#ifndef MATH_H
#define MATH_H

#include "types.h"

val sin(val x);
val cos(val x);
val isqrt(val x);
signed short isqrt16(signed short x);

#define max(x, y) (x < y ? y : x)
#define min(x, y) (x < y ? x : y)

#endif
