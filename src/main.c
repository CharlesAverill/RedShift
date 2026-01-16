#include "neslib.h"
#include "palette.h"
#include "types.h"
#include "geometry.h"
#include "params.h"
#include "string.h"
#include "ship.h"
#include "math.h"

#pragma bss-name(push, "ZEROPAGE")

const val bg_palette[] = {
    GRAYSCALE,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

val text[32];
const val *hello = "Hello, World!";
val i;
val ship_x, ship_y;

void main(void) {
    // Turn off the screen
    ppu_off();

    // Load palettes
    pal_bg(bg_palette);
    pal_spr(ship_palette);

    // Select the third set of tiles (ship)
    bank_spr(1);

    strncpy(text, hello, strlen(hello));
    vram_adr(NTADR_A((TILE_X_MAX - strlen(text)) >> 1, TILE_Y_MID));
    vram_write(text, sizeof(text));

    ppu_on_all();

    ship_x = 0;
    ship_y = 0;

    while(true) {
        ppu_wait_nmi();

        // Clear all sprites from the screen
        oam_clear();

        // Push ship metasprite to (ship_x, ship_y)
        oam_meta_spr(ship_x, 7 * sin(ship_x + 8) / 8, SHIP_SPRID, ship_list[ship_x / 32]);

        ++ship_x;
        ++ship_y;
    }
}
