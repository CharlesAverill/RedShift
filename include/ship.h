#ifndef SHIP_H
#define SHIP_H

#include "palette.h"
#include "types.h"

const val ship_palette[] = {
    BLACK, BLUE_1, ORANGE_2, LIGHT_BLUE,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

const unsigned char ship_msprite[]={

	  0,  0,0x00,0,
	  8,  0,0x01,0,
	  8,  8,0x11,0,
	  0,  8,0x10,0,
	128

};

#define SHIP_SPRID 0

extern val ship_x, ship_y;

#endif SHIP
