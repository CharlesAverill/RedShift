#include "neslib.h"
#include "nesdoug.h"
#include "palette.h"
#include "types.h"
#include "geometry.h"
#include "string.h"
#include "objects/ship.h"
#include "objects/celestial_bodies.h"
#include "math.h"
#include "controls.h"
#include "screens/screens.h"
#include "objects/bullets.h"
#include "objects/pickups.h"
#include "events.h"

#pragma bss-name(push, "ZEROPAGE")
const unsigned char bus_conflict_fix[4]={0,1,2,3};
#define POKE(addr,val)     (*(unsigned char*) (addr) = (val))

val btn_down, btn_new;
val sprid;

val frame_counter;
val brightness;
bool darkening;

static void pal_fade_to(unsigned to)
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
main_top: // Jump here if a game over occurs
    game_over_flag = false;
    brightness = 1;
    frame_counter = 1;
    darkening = true;

    // Turn off the screen
    ppu_off();

    POKE(bus_conflict_fix + 1, 1);

    // Set up title screen
    set_screen(TitleScreen);

    ppu_on_all();

    reset_inputs();

    while(!triggered(START)) {
        ppu_wait_nmi();

        read_inputs();

        if (++frame_counter % 64 == 0) {
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
                    ppu_mask(DEFAULT_MASK | GREEN_MASK);
                    break;
                case 1:
                    ppu_mask(DEFAULT_MASK | RED_MASK);
                    break;
                case 2:
                    ppu_mask(DEFAULT_MASK);
                    break;
            }
            // POKE(bus_conflict_fix + 0, 0);
        }
    }

    seed_rng();

    pal_fade_to(0);
    ppu_mask(DEFAULT_MASK);
    ppu_off();

    POKE(bus_conflict_fix + 0, 0);

    // Draw background
    set_screen(MainPlayScreen);

    pal_bright(0);
    ppu_on_all();
    brightness = -2;
    pal_fade_to(4);

    Ship_init();
    CBodies_init();
    Bullets_init();
    Pickups_init();

    // Main game loop
    while(!game_over_flag) {
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

    destroy_all_pickups();

    // Game over screen
    set_screen(GameOverScreen);

    while(true) {
        // Break on start button press
        read_inputs();
        if (triggered(START))
            break;

        CBodies_update();

        ppu_wait_nmi();
        sprid = 0;
        oam_clear();

        render(CBodies);
    }

    // Fade and clear the screen
    brightness = 1;
    pal_fade_to(0);

    destroy_all_bodies();
    sprid = 0;
    oam_clear();

    goto main_top;
}
