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
#include "bg.h"

const val bg_palette[] = {
    0x0f, 0x05, 0x03, 0x3C,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

const val sprite_palettes[] = {
    SHIP_PALETTE,
    GRAYSCALE,
    0,0,0,0,
    0,0,0,0
};

val btn_down, btn_new;
val sprid;

void main(void) {
    // Turn off the screen
    ppu_off();

    // Draw background
    bank_bg(1);
    pal_bg(bg_palette);
    vram_adr(NAMETABLE_A);
    vram_unrle(bg);

    // Prepare to draw sprites
    bank_spr(0);
    pal_spr(sprite_palettes);

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
