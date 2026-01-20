#include "neslib.h"
#include "palette.h"
#include "types.h"
#include "geometry.h"
#include "string.h"
#include "objects/ship.h"
#include "objects/celestial_bodies.h"
#include "math.h"
#include "controls.h"
#include "bg.h"
#include "objects/bullets.h"
#include "objects/pickups.h"
#include "title_screen.h"

const val bg_palette[] = {
    0x0f, 0x05, 0x03, 0x3C,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

const val title_screen_palette[] = {
    0x0f, 0x05, 0x10, 0x30,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

const val sprite_palettes[] = {
    SHIP_PALETTE,
    GRAYSCALE,
    EXPLOSION_PALETTE, // also bullets
    0, 0, 0, 0
};


#pragma bss-name(push, "ZEROPAGE")
const unsigned char bus_conflict_fix[4]={0,1,2,3};
#define POKE(addr,val)     (*(unsigned char*) (addr) = (val))

val btn_down, btn_new;
val sprid;

val frame_counter = 1;
val brightness = 1;
bool darkening = true;

void pal_fade_to(unsigned to)
{
  val bright = 3 + brightness;
  while(bright!=to)
  {
    delay(16);
    if(bright<to) ++bright; 
    else --bright;
    pal_bright(bright);
  }
}

void main(void) {
    // Turn off the screen
    ppu_off();

    POKE(bus_conflict_fix + 1, 1);

    // Set up title screen
    ppu_mask(0x1e | (1 << 5)); // red emphasis
    pal_bg(title_screen_palette);
    vram_adr(NAMETABLE_A);
    vram_unrle(title_screen);

    // Play bgm
    music_play(0);

    ppu_on_all();

    while(!triggered(START)) {
        ppu_wait_nmi();

        read_inputs();

        if (frame_counter % 64 == 0) {
            if (brightness == 2)
                darkening = true;
            else if (brightness == 0)
                darkening = false;

            if (darkening)
                --brightness;
            else
                ++brightness;

            switch(brightness) {
                case 0:
                    ppu_mask(0x1e | (1 << 6));
                    break;
                case 1:
                    ppu_mask(0x1e | (1 << 5));
                    break;
                case 2:
                    ppu_mask(0x1e);
                    break;
            }
            // POKE(bus_conflict_fix + 0, 0);
        }

        ++frame_counter;
    }

    set_rand(frame_counter);

    pal_fade_to(0);
    ppu_mask(0x1e);
    ppu_off();

    POKE(bus_conflict_fix + 0, 0);

    // Draw background
    bank_bg(1);
    pal_bg(bg_palette);
    vram_adr(NAMETABLE_A);
    vram_unrle(bg);

    // Prepare to draw sprites
    bank_spr(0);
    pal_spr(sprite_palettes);

    pal_bright(0);
    ppu_on_all();
    brightness = -2;
    pal_fade_to(4);

    Ship_init();
    CBodies_init();
    Bullets_init();
    Pickups_init();

    while(true) {
        // Read controller input
        read_inputs();

        Ship_update();
        Bullets_update();
        CBodies_update();
        Pickups_update();

        ppu_wait_nmi();
        // Clear sprites
        sprid = 0;
        oam_clear();

        render(Ship);
        render(Bullets);
        render(CBodies);
        render(Pickups);
    }
}
