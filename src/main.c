#include "neslib.h"
#include "palette.h"
#include "types.h"
#include "geometry.h"
#include "params.h"
#include "string.h"
#include "ship.h"
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

    while(true) {
        ppu_wait_nmi();

        // Clear sprites
        oam_clear();

        // Read controller input
        read_inputs();

        Ship_update();
        Ship_render();
    }
}
