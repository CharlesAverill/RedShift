#ifndef TYPES_H
#define TYPES_H

#ifndef __fastcall__
#define __fastcall__
#endif

typedef unsigned char val;
typedef signed char sval;
typedef unsigned short bigval;
typedef signed short sbigval;
typedef unsigned short ptr;

typedef val bool;

#define true 1
#define false 0

#define routine(id) void __fastcall__ id(void)

extern val sprid;
#define render_routine(id) val __fastcall__ Render_ ## id (void)
#define render(id) sprid = Render_ ## id ()

#define NULL 0

typedef enum facing {
    Up,
    UpLeft,
    Left,
    DownLeft,
    Down,
    DownRight,
    Right,
    UpRight
} facing;

#endif
