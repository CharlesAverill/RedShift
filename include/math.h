#ifndef MATH_H
#define MATH_H

#include "types.h"

val sin(val x);
val cos(val x);
val isqrt(val x);
val atan2(sbigval x, sbigval y);
sbigval isqrt16s(sbigval x);
bigval isqrt16(bigval x);

#define max(x, y) (x < y ? y : x)
#define min(x, y) (x < y ? x : y)
#define abs(x) (x < 0 ? -x : x)
#define sign(x) (x < 0 ? -1 : 1)

#endif
