#include "neslib.h"
#include "palette.h"
#include "types.h"
#include "geometry.h"
#include "params.h"
#include "string.h"
#include "ship.h"
#include "celestial_bodies.h"
#include "math.h"
#include "controls.h"

#pragma bss-name(push, "ZEROPAGE")

const val bg_palette[] = {
    GRAYSCALE,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

val btn_down, btn_new;
val sprid;

void main(void) {
    // Turn off the screen
    ppu_off();

    // Load palettes
    pal_bg(bg_palette);
    pal_spr(ship_palette);

    // Select the third set of tiles (ship)
    bank_spr(1);

    ppu_on_all();

    Ship_init();
    CBodies_init();

    while(true) {
        ppu_wait_nmi();

        // Clear sprites
        sprid = 0;
        oam_clear();

        // Read controller input
        read_inputs();

        Ship_update();
        CBodies_update();

        render(Ship);
        render(CBodies);
    }
}
