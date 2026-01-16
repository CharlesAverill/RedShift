#ifndef SHIP_H
#define SHIP_H

#include "neslib.h"
#include "palette.h"
#include "types.h"

const val ship_palette[] = {
    BLACK, BLUE_1, ORANGE_2, LIGHT_BLUE,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

const unsigned char ship_0_data[]={

	  0,  0,0x00,0,
	  8,  0,0x01,0,
	  8,  8,0x11,0,
	  0,  8,0x10,0,
	128

};

const unsigned char ship_1_data[]={

	  0,  0,0x20,0,
	  8,  0,0x21,0,
	  8,  8,0x31,0,
	  0,  8,0x30,0,
	128

};

const unsigned char ship_2_data[]={

	  0,  0,0x02,0,
	  8,  0,0x03,0,
	  0,  8,0x12,0,
	  8,  8,0x13,0,
	128

};

const unsigned char ship_3_data[]={

	  0,  8,0x20,0|OAM_FLIP_V,
	  8,  8,0x21,0|OAM_FLIP_V,
	  8,  0,0x31,0|OAM_FLIP_V,
	  0,  0,0x30,0|OAM_FLIP_V,
	128

};

const unsigned char ship_4_data[]={

	  8,  8,0x00,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  0,0x11,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0x01,0|OAM_FLIP_H|OAM_FLIP_V,
	  8,  0,0x10,0|OAM_FLIP_H|OAM_FLIP_V,
	128

};

const unsigned char ship_5_data[]={

	  8,  8,0x20,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0x21,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  0,0x31,0|OAM_FLIP_H|OAM_FLIP_V,
	  8,  0,0x30,0|OAM_FLIP_H|OAM_FLIP_V,
	128

};

const unsigned char ship_6_data[]={

	  8,  8,0x02,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  8,0x03,0|OAM_FLIP_H|OAM_FLIP_V,
	  8,  0,0x12,0|OAM_FLIP_H|OAM_FLIP_V,
	  0,  0,0x13,0|OAM_FLIP_H|OAM_FLIP_V,
	128

};

const unsigned char ship_7_data[]={

	  8,  0,0x20,0|OAM_FLIP_H,
	  0,  0,0x21,0|OAM_FLIP_H,
	  0,  8,0x31,0|OAM_FLIP_H,
	  8,  8,0x30,0|OAM_FLIP_H,
	128

};

const unsigned char* const ship_list[]={

	ship_0_data,
	ship_1_data,
	ship_2_data,
	ship_3_data,
	ship_4_data,
	ship_5_data,
	ship_6_data,
	ship_7_data

};

#define SHIP_SPRID 0
#define SHIP_UP ship_list[0]
#define SHIP_UL ship_list[1]
#define SHIP_LEFT ship_list[2]
#define SHIP_DL ship_list[3]
#define SHIP_DOWN ship_list[4]
#define SHIP_DR ship_list[5]
#define SHIP_RIGHT ship_list[6]
#define SHIP_UR ship_list[7]

extern val ship_x, ship_y;

#endif SHIP
